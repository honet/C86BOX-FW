/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include <project.h>
#include "cbus.h"
#include "cbus_board.h"
#include "eeprom_config.h"
#include "usb_vendor_request.h"

#define WBVAL(x)  (x & 0xFF),((x >> 8) & 0xFF)
#define B3VAL(x)  (x & 0xFF),((x >> 8) & 0xFF),((x >> 16) & 0xFF)
#define DWBVAL(x) (x & 0xFF),((x >> 8) & 0xFF),((x >> 16) & 0xFF),((x >> 24) & 0xFF)

#define EXTENDED_COMPAT_ID		0x0004		// WBVAL(0x0004)
#define EXTENDED_PROPERTIES		0x0005		// WBVAL(0x0005)

#define PROPERTY_DATA_TYPE_REG_SZ					DWBVAL(1)
#define PROPERTY_DATA_TYPE_REG_EXPAND_SZ			DWBVAL(2)
#define PROPERTY_DATA_TYPE_REG_BINARY				DWBVAL(3)
#define PROPERTY_DATA_TYPE_REG_DWORD_LITTLE_ENDIAN	DWBVAL(4)
#define PROPERTY_DATA_TYPE_REG_DWORD_BIG_ENDIAN		DWBVAL(5)
#define PROPERTY_DATA_TYPE_REG_LINK					DWBVAL(6)
#define PROPERTY_DATA_TYPE_REG_MULTI_SZ				DWBVAL(7)

// VENDOR REQUEST ------------------------------------------------------------
#define C86_VENDOR_GET_BOARD_INFO		0x81
// SETUP:
//   bmRequestType: DeviceToHost, vendor, device
//   bRequest     : C86_VENDOR_GET_BOARD_INFO
//   wValue       : ignore
//   wIndex       : ボード番号(0~4)
//   wLength      : sizeof(BOARD_INFO)
// DATA STAGE:
//   BOARD_INFO

#define C86_VENDOR_GET_FW_VER			0x82
// SETUP:
//   bmRequestType: DeviceToHost, vendor, device
//   bRequest     : C86_VENDOR_GET_FW_VER
//   wValue       : ignore
//   wIndex       : ignore
//   wLength      : 4
// DATA STAGE:
//   (uint32_t)verison

#define C86_VENDOR_GET_CONF_BOARD_TYPE		0x13
// SETUP:
//   bmRequestType: DeviceToHost, vendor, device
//   bRequest     : C86_VENDOR_GET_BOARD_INFO
//   wValue       : ignore
//   wIndex       : ボード番号(0~4)
//   wLength      : 4
// DATA STAGE:
//   (uint32_t)board_type

#define C86_VENDOR_SET_CONF_BOARD_TYPE		0x14
// SETUP:
//   bmRequestType: HostToDevice, vendor, device
//   bRequest     : C86_VENDOR_CBUS_RESET
//   wValue       : ignore
//   wIndex       : ボード番号(0~4)
//   wLength      : 4
// DATA STAGE:
//   (uint32_t)board_type

#define C86_VENDOR_SYSTEM_RESET			0x11
// SETUP:
//   bmRequestType: HostToDevice, vendor, device
//   bRequest     : C86_VENDOR_SYSTEM_RESET
//   wValue       : ignore
//   wIndex       : ignore
//   wLength      : 0


// ---------------------------------------------------------------------------
typedef struct {
	uint32_t type;
	uint32_t nchips;
	uint32_t chiptype[NMAXCHIPS];
} BOARD_INFO;

BOARD_INFO board_info;

uint32_t config_board_type = 0;

uint32_t fwversion = 0x5;

volatile uint8_t usbReq_reset = 0;
volatile uint8_t usbReq_setBoardType = 0;
volatile uint32_t usbReq_boardType[NMAXBOARDS] = {0};


// Microsoft Specific---------------------------------------------------------
const uint8_t USB_MSOSFeatureDescriptor[] = {
	// HEADER Section  ------
	DWBVAL(0x28),			/* dwLength   : length of this descriptor */
	WBVAL(0x0100),			/* bcdVersion : version */
	WBVAL(0x0004),			/* wIndex     : Extended compat ID descriptor */
	0x01,					/* bCount     : Number of function sections */
	0,0,0,0,0,0,0,			/* RESERVED */
	
	// FUNCTION Section  ------
	0x00,						/* bFirstInterfaceNumber : Interface number */
	0x01,						/* RESERVED */
	0x57, 0x49, 0x4e, 0x55,		/* compatibleID : "WINUSB" */
	0x53, 0x42, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,		/* subCompatibleID : Secondary ID */
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,		/* RESERVED */
	0x00, 0x00
};

const uint8_t USB_MSOSExtPropDescriptor[] = {
	// HEADER Section ---------------------------
	DWBVAL(142),		/* dwLength   : The length, in bytes, of the complete extended properties descriptor */
	WBVAL(0x0100),		/* bcdVersion : The descriptor's version number, in binary coded decimal (BCD) format */
	WBVAL(0x0005),		/* wIndex     : The index for extended properties OS descriptors */
	WBVAL(0x0001),		/* wCount     : The number of custom property sections that follow the header section */

	// Custom property section(1) --------------
	DWBVAL(132), 					/* dwSize               : Length of this custom property section is 132 bytes.  */
	PROPERTY_DATA_TYPE_REG_SZ,		/* dwPropertyDataType   : Property value stores a Unicode string. */
	WBVAL(40),						/* wPropertyNameLength  : Length of the property name string is 40 bytes. */
	'D',0,'e',0,'v',0,'i',0,'c',0,'e',0,'I',0,'n',0,
	't',0,'e',0,'r',0,'f',0,'a',0,'c',0,'e',0,'G',0,
	'U',0,'I',0,'D',0,0,0,			/* bPropertyName        : Property name is "DeviceInterfaceGUID". */
	DWBVAL(78),						/* dwPropertyDataLength : Length of the property value string is 78 bytes. */
	'{',0,'b',0,'0',0,'3',0,'2',0,'0',0,'d',0,'0',0,
	'9',0,'-',0,'0',0,'7',0,'9',0,'1',0,'-',0,'4',0,
	'c',0,'3',0,'f',0,'-',0,'a',0,'7',0,'4',0,'1',0,
	'-',0,'9',0,'e',0,'f',0,'9',0,'7',0,'c',0,'8',0,
	'8',0,'8',0,'5',0,'d',0,'6',0,'}',0,0,0
									/* bPropertyData        : Property value is "{b0320d09-0791-4c3f-a741-9ef97c8885d6}".  */
};


// -------------------
uint8 USBFS_HandleVendorRqst(void) 
{
	uint8 requestHandled = USBFS_FALSE;
	uint8_t request_type = CY_GET_REG8(USBFS_bmRequestType);
	uint8_t req_dir = request_type & USBFS_RQST_DIR_MASK;
	//uint8_t req_type = request_type & USBFS_RQST_TYPE_MASK;
	uint8_t req_recipient = request_type & USBFS_RQST_RCPT_MASK;
	
	if (req_dir == USBFS_RQST_DIR_D2H) {
		/* Control Read */
		switch (CY_GET_REG8(USBFS_bRequest)) {
		case USBFS_GET_EXTENDED_CONFIG_DESCRIPTOR:
			// Request for Microsoft OS Extended Descriptor
			switch (req_recipient){
			case USBFS_RQST_RCPT_DEV:
				if (CY_GET_REG16(USBFS_wIndex) == EXTENDED_COMPAT_ID){
					// Microsoft MSOS Feature Descriptor
					USBFS_currentTD.pData = (volatile uint8 *)&USB_MSOSFeatureDescriptor[0];
					USBFS_currentTD.count = USB_MSOSFeatureDescriptor[0];
					requestHandled  = USBFS_InitControlRead();
				}
				break;
				
			case USBFS_RQST_RCPT_IFC:
				if (CY_GET_REG16(USBFS_wIndex) == EXTENDED_PROPERTIES){
					// Microsoft MSOS Extended Property Descriptor
					USBFS_currentTD.pData = (volatile uint8 *)&USB_MSOSExtPropDescriptor[0];
					USBFS_currentTD.count = USB_MSOSExtPropDescriptor[0];
					requestHandled  = USBFS_InitControlRead();
				}
				break;
				
			case USBFS_RQST_RCPT_EP:
			case USBFS_RQST_RCPT_OTHER:
				break;
			}
			//#if defined(USBFS_ENABLE_MSOS_STRING)
			//			USBFS_currentTD.pData = (volatile uint8 *)&USBFS_MSOS_CONFIGURATION_DESCR[0u];
			//			USBFS_currentTD.count = USBFS_MSOS_CONFIGURATION_DESCR[0u];
			//			requestHandled  = USBFS_InitControlRead();
			//#endif /*  USBFS_ENABLE_MSOS_STRING */
			break;

		case C86_VENDOR_GET_BOARD_INFO:
			{
				// board info
				uint16_t index = CY_GET_REG16(USBFS_wIndex);
				if (index<NMAXBOARDS){
					board_info.type = cbus_get_board_type((uint8_t)index);
					for (int i=0; i<NMAXCHIPS; i++){
						board_info.nchips = cbus_get_nchips((uint8_t)index);
						board_info.chiptype[i] = cbus_get_chip_type((uint8_t)index, i);
					}
					USBFS_currentTD.pData = (volatile uint8 *)&board_info;
					USBFS_currentTD.count = 24;
					requestHandled  = USBFS_InitControlRead();
				}
			}
			break;
			
		case C86_VENDOR_GET_CONF_BOARD_TYPE:
			{
				uint16_t index = CY_GET_REG16(USBFS_wIndex);
				if (index<NMAXBOARDS){
					config_board_type = conf_get_board_type(index);
					USBFS_currentTD.pData = (volatile uint8 *)&config_board_type;
					USBFS_currentTD.count = 4;
					requestHandled  = USBFS_InitControlRead();
				}
			}
			break;
			
		case C86_VENDOR_GET_FW_VER:
			{
				// firmware version
				USBFS_currentTD.pData = (volatile uint8 *)&fwversion;
				USBFS_currentTD.count = 4;
				requestHandled  = USBFS_InitControlRead();
			}
			break;
			
		default:
			break;
		}
	}
	else
	{
		/* Control Write */
		switch (CY_GET_REG8(USBFS_bRequest)) {
		case C86_VENDOR_SYSTEM_RESET:
			// reset
			usbReq_reset = 1;
			requestHandled = USBFS_InitNoDataControlTransfer();
			break;
			
		case C86_VENDOR_SET_CONF_BOARD_TYPE:
			{
				uint16_t index = CY_GET_REG16(USBFS_wIndex);
				if (index<NMAXBOARDS){
                    usbReq_setBoardType |= (1<<index);
					
					USBFS_currentTD.count = 4;
					USBFS_currentTD.pData = (volatile uint8_t*)&usbReq_boardType[index];

					requestHandled = USBFS_InitControlWrite();
				}
			}
			break;
		}
	}
	
	return(requestHandled);
}


/* [] END OF FILE */
