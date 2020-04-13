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
#include "eeprom_config.h"
#include "usb_vendor_request.h"
#include "sidbench.h"
#include "tick.h"

// MESSAGE DEFINITION ------------------------------------------------------
// 00 [7:4]slot,[3:1]chip,[0:0]ex addr data
// 10 cc dd dd  slot.A write control : cc=control no, dd=data
// 11 cc dd dd  slot.B write control : cc=control no, dd=data
// 12 cc dd dd  slot.C write control(reserved) : cc=control no, dd=data
// 13 cc dd dd  slot.D write control(reserved) : cc=control no, dd=data

// f0 sa aa aa  C-BUS address set for data write : aa=addr[19:0]
// f1 s0 dd dd  CBUS data write : s=slot no
// f2 sa aa aa  CBUS data read
// FD nn nn nn  ntick wait
// FE nn nn nn  (deprecated) 1ms*nsync

#define MSGBUF_SIZE      1024//(1024*8) // must be equal to 2^n
#define MSGBUF_SIZEMASK  (MSGBUF_SIZE-1)

volatile uint8_t ep2inbuf[32];

volatile uint32_t msgbuf[MSGBUF_SIZE];
volatile uint32_t msg_widx;
volatile uint32_t msg_ridx;
volatile uint32_t msg_length;

volatile uint64_t last_tick;
volatile uint64_t next_tick;

uint8_t SNStringDesc[128];

#define UnassignedAddress 0

#define USB_OUT_EP 0x01
#define USB_IN_EP  0x02

#define IDX(x)		( (x)&MSGBUF_SIZEMASK )
#define REMAIN()	(MSGBUF_SIZE - msg_length)

// -------------------------------------
#define LEDID_POWER     0
#define LEDID_CONNECT   1
#define LEDID_DATAA     2
#define LEDID_DATAB     3

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

void ProcessData()
{
	static uint16_t cbus_addr = 0;
    
    uint64_t tick = GetTick();
    if (tick<next_tick)
        return;

	if(msg_length>0){
		uint32_t d = msgbuf[IDX(msg_ridx)];
		uint8_t cmd = d>>24;

		switch(cmd){
		case 0x0:
			{
				uint8_t ex = (d>>16)&0xff;
				uint8_t addr = (d>>8)&0xff;
				uint8_t data = (d&0xff);
				uint8_t slot = (ex>>4)&0x01;

				led_on(2+slot);
				write_chip(ex, addr, data);
				led_off(2+slot);

				msg_ridx++;
                msg_length--;
				break;
			}
            
        case 0xfd:
			if(d&0xffffff){
                // タイミングの基準は前回sync時の値。現在値ではない。
                next_tick = next_tick + (d&0xffffff);
            }else{
                // n=0なら現在カウンタ値と同期させる
                next_tick = tick;
            }
			msg_ridx++;
            msg_length--;
            break;
#if 0
		case 0xfe:
			// sync
			if(d&0xffffff){
				//nsync
				// FIXME: いまいち。
				msgbuf[IDX(msg_ridx)] = d-1; // = 0xfe000000 | (n-1);
			}else{
				//1sync
				msg_ridx++;
                msg_length--;
			}
			break;
#endif

		case 0x10: // 10 cc dd dd  slot.A write control : cc=control no, dd=data
			cbus_board_control_write(0, (d>>16)&0xff, d&0xffff);
			msg_ridx++;
            msg_length--;
			break;
		case 0x11: // 11 cc dd dd  slot.B write control : cc=control no, dd=data
			cbus_board_control_write(1, (d>>16)&0xff, d&0xffff);
			msg_ridx++;
            msg_length--;
			break;
			
		case 0xf0: // f0 0a aa aa  C-BUS address set for data write
			cbus_addr = d&0x0fffff;
			msg_ridx++;
            msg_length--;
			break;

		case 0xf1: // f1 s0 dd dd  CBUS data write
			{
				uint8_t slot = (d>>20)&0x0f;
				if (slot<NMAXBOARDS){
					led_on(2+slot);
					cbus_write16( slot, cbus_addr, d&0xffff );
					led_off(2+slot);
				}
				msg_ridx++;
				msg_length--;
			}
			break;

		case 0xf2: // f2 sa aa aa  CBUS data read
			{
                uint8_t slot = (d>>20)&0x0f;
				cbus_addr = d&0x0fffff;
                led_on(2+slot);
				uint16_t x = cbus_read8( slot, cbus_addr );
                led_off(2+slot);
				
				// set data
				*((volatile uint16_t*)&ep2inbuf[0]) = x;
				USBFS_LoadInEP(USB_IN_EP, (uint8_t*)ep2inbuf, 2);
			}
			msg_ridx++;
            msg_length--;
			break;
			
		default:
			//unknown command.
			msg_ridx++;
            msg_length--;
			break;
		}
	}
}


void BulkTransfer(void)
{
	if (USBFS_GetEPState(USB_OUT_EP) == USBFS_OUT_BUFFER_FULL)
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
				USBFS_ReadOutEP(USB_OUT_EP, (uint8_t*)&msgbuf[widx], 4*(MSGBUF_SIZE-widx));
				USBFS_ReadOutEP(USB_OUT_EP, (uint8_t*)&msgbuf[0], 4*(widx+intcount-MSGBUF_SIZE));
			}
			msg_widx += intcount;
            msg_length += intcount;
			//USBFS_EnableOutEP(USB_OUT_EP); ReadOutEP内で実行されてる
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
	if( descr != NULL) {
		j=2u;

		for (i=0u; i<len; i++){
			descr[j++] = prefix[i];
			descr[j++] = 0;
		}

		/* fill descriptor : WAFER-ID 16文字*/
		for(i = 0u; i<8; i++) {
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

// -------------------------------------
int main()
{
	msg_widx = 0;
	msg_ridx = 0;
    msg_length = 0;
    last_tick = 0;

    EEPROM_Start();
	led_init();
	led_on(LEDID_POWER);
	
	// generage USB S/N string descriptor
	_ReadDieID(SNStringDesc);
	USBFS_SerialNumString(SNStringDesc);

    /* Enable global interrupts */
	CYGlobalIntEnable;

    // setup
    TickSetup();
	cbus_reset();
	cbus_board_setup();
//    while(1) sidbench();

	// wait board init.
	while(0<msg_length);

	// start the USB.
	USBFS_Start( 0, USBFS_DWR_VDDD_OPERATION );

	/* Wait until device address is assigned */
	while(USBFS_deviceAddress == UnassignedAddress);

	for (;;){
		if (USBFS_GetConfiguration() != 0){
			if (USBFS_IsConfigurationChanged() != 0){
				USBFS_EnableOutEP(USB_OUT_EP);
				led_on(LEDID_CONNECT);
			}

			BulkTransfer();
			ProcessData();

			if( USBFS_transferState == USBFS_TRANS_STATE_IDLE ){
				if (usbReq_reset){
					cbus_reset();
					for(int i=0; i<NMAXBOARDS; i++){
						cbus_board_init(i);
					}
					usbReq_reset = 0;
				}

				if (usbReq_setBoardType){
					for(int i=0; i<NMAXBOARDS; i++){
						if (usbReq_setBoardType & (0x01<<i)){
							conf_set_board_type( i, usbReq_boardType[i] );
						}
					}
					usbReq_setBoardType = 0;
				}
			}
		}else{
			led_off(LEDID_CONNECT);
		}
	}

	// never reach.
	led_off(0);
}


/* [] END OF FILE */

