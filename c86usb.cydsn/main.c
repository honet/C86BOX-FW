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
#include "sound_chip.h"
#include "usb_vendor_request.h"

// 00 [7:4]slot,[3:1]chip,[0:0]ex addr data
// FE nn nn nn nsync
#define MSGBUF_SIZE      1024//(1024*8) // must be equal to 2^n
#define MSGBUF_SIZEMASK  (MSGBUF_SIZE-1)

volatile uint32_t msgbuf[MSGBUF_SIZE];
volatile uint32_t msg_widx;
volatile uint32_t msg_ridx;
volatile uint8_t ticked = 0;

#define IDX(x)		( (x)&MSGBUF_SIZEMASK )
#define LENGTH()	(msg_widx-msg_ridx)
#define REMAIN()	(MSGBUF_SIZE - LENGTH())

// -------------------------------------
static uint8_t ledval = 0xff;
void led_init(){
	LEDControl_Write(0xff);
}
void led_on(uint8_t idx){
    ledval &= ~(0x01<<idx);
	LEDControl_Write( ledval );
}
void led_off(uint8_t idx){
    ledval |= (0x01<<idx);
	LEDControl_Write( ledval );
}
void led_set(uint8_t idx, uint8_t sw){
	if(sw)	led_on(idx);
	else	led_off(idx);
}

// ----------------------------------------

CY_ISR(Tick)
{
	ticked = 1;
	// clear sticky bits.
	TickTimer_ReadStatusRegister();
}

void ProcessData()
{
	if(ticked && LENGTH()>0){
		uint32_t d = msgbuf[IDX(msg_ridx)];
		uint32_t cmd = d&0xff000000;

		if(cmd==0){
			uint8_t ex = (d>>16)&0xff;
			uint8_t addr = (d>>8)&0xff;
			uint8_t data = (d&0xff);
			uint8_t slot = (ex>>4)&0x01;

			led_on(2+slot);
			write_chip(ex, addr, data);
			led_off(2+slot);

			msg_ridx++;
		}else if(cmd==0xfe000000){
			// sync
			if(d&0xffffff){
				//nsync
				// FIXME: いまいち。
				msgbuf[IDX(msg_ridx)] = d-1; // = 0xfe000000 | (n-1);
			}else{
				//1sync
				msg_ridx++;
			}
            ticked = 0;
		}else{
			//unknown command.
			msg_ridx++;
		}
	}
}


#define UnassignedAddress 0

#define USB_OUT_EP 0x01
#define USB_IN_EP  0x02 // 今のところ使っていない

void BulkTransfer(void)
{
	if (USBFS_bGetEPState(USB_OUT_EP) == USBFS_OUT_BUFFER_FULL)
	{
		uint8_t count = USBFS_wGetEPCount(USB_OUT_EP);
        uint8_t intcount = count>>2;
		uint32_t widx = IDX(msg_widx);

		if (intcount<=REMAIN()){
			if (intcount <= (MSGBUF_SIZE - widx)){
				// 分断せず
				USBFS_ReadOutEP(USB_OUT_EP, (uint8_t*)&msgbuf[widx], count);
			}else{
				// 分断
				USBFS_ReadOutEP(USB_OUT_EP, (uint8_t*)&msgbuf[widx], (MSGBUF_SIZE-widx));
				USBFS_ReadOutEP(USB_OUT_EP, (uint8_t*)&msgbuf[0], widx+intcount-MSGBUF_SIZE);
			}
			msg_widx = msg_widx + intcount;
		}
	}
}


static void _ReadDieID(uint8 descr[]) 
{
    uint8 i;
    uint8 j = 0u;
    uint8 value;
    const char8 CYCODE hex[16u] = "0123456789ABCDEF";
    const char8 prefix[] = "honet.kk@gmail.com:";  //19文字
    int len = strlen(prefix);
    
    /* Check descriptor validation */
    if( descr != NULL)
    {
        j=2u;
        
        for (i=0u; i<len; i++){
            descr[j++] = prefix[i];
            descr[j++] = 0;
        }

        /* fill descriptor : WAFER-ID 16文字*/
        for(i = 0u; i<8; i++)
        {
            value = CY_GET_XTND_REG8((void CYFAR *)(CYDEV_FLSHID_CUST_TABLES_BASE+i));
            descr[j++] = (uint8)hex[value >> 4u];
            descr[j++] = 0u;
            descr[j++] = (uint8)hex[value & 0x0Fu];
            descr[j++] = 0u;
        }
        
        descr[0u] = j;
        descr[1u] = USBFS_DESCR_STRING;
    }
}
    
uint8_t SNStringDesc[128];

// -------------------------------------
int main()
{
	EEPROM_Start();
	led_init();

	led_on(0);

	cbus_reset();
	cbus_board_setup();

	msg_widx = 0;
	msg_ridx = 0;

	/* Enable global interrupts */
	CYGlobalIntEnable;

	// setup
	TickTimerISR_StartEx(Tick);
	TickTimer_Start();

	// board init.
	while(0<LENGTH());

	// generage USB S/N string descriptor
	_ReadDieID(SNStringDesc);
	USBFS_SerialNumString(SNStringDesc);

	// start the USB.
	USBFS_Start( 0, USBFS_DWR_VDDD_OPERATION );

	/* Wait until device address is assigned */
	while(USBFS_deviceAddress == UnassignedAddress);

	for (;;){
		if (USBFS_GetConfiguration() != 0){
			if (USBFS_IsConfigurationChanged() != 0){
				USBFS_EnableOutEP(1);
				led_on(1);
			}

			BulkTransfer();
			ProcessData();

			if( USBFS_transferState == USBFS_TRANS_STATE_IDLE ){
				// reset request.
				if ( usbReq_reset ){
					cbus_reset();
					for(int i=0; i<NMAXBOARDS; i++){
						cbus_board_init(i);
					}
                    usbReq_reset = 0;
				}
				// control request
				else if ( usbReq_control ){
					cbus_board_control_write( usbReq_boardIdx, usbReq_controlIdx, usbReq_controlValue );
                    usbReq_control = 0;
				}
			}
		}else{
			led_off(1);
		}
	}

	// never reach.
	led_off(0);
}


/* [] END OF FILE */
