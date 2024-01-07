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
#include "cbus_board14.h"
#include "sound_chip.h"
#include "eeprom_config.h"
#include "usb_vendor_request.h"
#include "sidbench.h"
#include "tick.h"


// MESSAGE DEFINITION ------------------------------------------------------
// 00 ee aa dd  chip write: ee=([7:4]slot,[3:1]chip,[0:0]ex) aa=addr dd=data
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

volatile uint8_t __attribute__((aligned(4))) ep2inbuf[32];

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
	if (sw)	led_on(idx);
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
			if (d&0xffffff){
				// タイミングの基準は前回sync時の値。現在値ではない。
				next_tick = next_tick + (d&0xffffff);
			} else {
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
	if (USBFS_GetEPState(USB_OUT_EP) == USBFS_OUT_BUFFER_FULL) {
		uint8_t count = USBFS_wGetEPCount(USB_OUT_EP);
		uint8_t intcount = count>>2;
		uint32_t widx = IDX(msg_widx);

		if (intcount<=REMAIN()) {
			if (intcount <= (MSGBUF_SIZE - widx)) {
				// 分断せず
				USBFS_ReadOutEP(USB_OUT_EP, (uint8_t*)&msgbuf[widx], count);
			} else {
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


// ----------------------------------------
// MIDI part

USBFS_MIDI_RX_STATUS midiRxStatus[NMAXBOARDS];

/* The Size of the MIDI messages (MIDI Table 4-1) */
static const uint8 CYCODE USBFS_MIDI_SIZE[] = {
    /*  Miscellaneous function codes(Reserved)  */ 0x03u,
    /*  Cable events (Reserved)                 */ 0x03u,
    /*  Two-byte System Common messages         */ 0x02u,
    /*  Three-byte System Common messages       */ 0x03u,
    /*  SysEx starts or continues               */ 0x03u,
    /*  Single-byte System Common Message or
        SysEx ends with following single byte   */ 0x01u,
    /*  SysEx ends with following two bytes     */ 0x02u,
    /*  SysEx ends with following three bytes   */ 0x03u,
    /*  Note-off                                */ 0x03u,
    /*  Note-on                                 */ 0x03u,
    /*  Poly-KeyPress                           */ 0x03u,
    /*  Control Change                          */ 0x03u,
    /*  Program Change                          */ 0x02u,
    /*  Channel Pressure                        */ 0x02u,
    /*  PitchBend Change                        */ 0x03u,
    /*  Single Byte                             */ 0x01u
};


// Local processing of the USB MIDI out-events.
void USBFS_callbackLocalMidiEvent(uint8 cable, uint8 *midiMsg) CYREENTRANT
{
	cable >>= 4;
	if (cable > 1) return;

	// NOTE: USBFSのフレームワークがバグっていて、byte0が取得出来ないので無理矢理インデックス-1で参照する
	uint8_t cmd = midiMsg[-1] & USBFS_CIN_MASK;
	if ((cmd == USBFS_RESERVED0) || (cmd == USBFS_RESERVED1))
		return;


	led_on(LEDID_DATAA + cable);

	/* Puts data into the MIDI TX output buffer.*/
	uint8_t len = USBFS_MIDI_SIZE[cmd];
	for (uint8_t i=0; i<len; i++){
		cbus_midi_write(cable, midiMsg[i]);
	}

	led_off(LEDID_DATAA + cable);
}


/*******************************************************************************
* Function Name: USBFS_ProcessMidiIn
****************************************************************************//**
*
*  Processes one byte of incoming MIDI data.
*
*   mData = current MIDI input data byte
*   *rxStat = pointer to a MIDI_RX_STATUS structure
*
* \return
*   0, if no complete message
*   1 - 4, if message complete
*   MIDI_SYSEX, if start or continuation of system exclusive
*   MIDI_EOSEX, if end of system exclusive
*   0xf8 - 0xff, if single byte real time message
*
*******************************************************************************/
uint8_t ProcessMidiIn(uint8_t mData, USBFS_MIDI_RX_STATUS *rxStat)
{
	uint8 midiReturn = 0u;

	/* Check for a MIDI status byte.  All status bytes, except real time messages,
	 *  which are a single byte, force the start of a new buffer cycle.
	 */
	if ((mData & USBFS_MIDI_STATUS_BYTE_MASK) != 0u) {
		if ((mData & USBFS_MIDI_STATUS_MASK) == USBFS_MIDI_STATUS_MASK) {
			if ((mData & USBFS_MIDI_SINGLE_BYTE_MASK) != 0u) {
				/* System Real-Time Messages(single byte) */
				midiReturn = mData;
			} else {
				/* System Common Messages */
				switch (mData) {
				case USBFS_MIDI_SYSEX:
					rxStat->msgBuff[0u] = USBFS_MIDI_SYSEX;
					rxStat->runstat = USBFS_MIDI_SYSEX;
					rxStat->count = 1u;
					rxStat->length = 3u;
					break;
				case USBFS_MIDI_EOSEX:
					rxStat->runstat = 0u;
					rxStat->size = rxStat->count;
					rxStat->count = 0u;
					midiReturn = USBFS_MIDI_EOSEX;
					break;
				case USBFS_MIDI_SPP:
					rxStat->msgBuff[0u] = USBFS_MIDI_SPP;
					rxStat->runstat = 0u;
					rxStat->count = 1u;
					rxStat->length = 3u;
					break;
				case USBFS_MIDI_SONGSEL:
					rxStat->msgBuff[0u] = USBFS_MIDI_SONGSEL;
					rxStat->runstat = 0u;
					rxStat->count = 1u;
					rxStat->length = 2u;
					break;
				case USBFS_MIDI_QFM:
					rxStat->msgBuff[0u] = USBFS_MIDI_QFM;
					rxStat->runstat = 0u;
					rxStat->count = 1u;
					rxStat->length = 2u;
					break;
				case USBFS_MIDI_TUNEREQ:
					rxStat->msgBuff[0u] = USBFS_MIDI_TUNEREQ;
					rxStat->runstat = 0u;
					rxStat->size = 1u;
					rxStat->count = 0u;
					midiReturn = rxStat->size;
					break;
				default:
					break;
				}
			}
		} else {
			/* Channel Messages */
			rxStat->msgBuff[0u] = mData;
			rxStat->runstat = mData;
			rxStat->count = 1u;
			switch (mData & USBFS_MIDI_STATUS_MASK) {
			case USBFS_MIDI_NOTE_OFF:
			case USBFS_MIDI_NOTE_ON:
			case USBFS_MIDI_POLY_KEY_PRESSURE:
			case USBFS_MIDI_CONTROL_CHANGE:
			case USBFS_MIDI_PITCH_BEND_CHANGE:
				rxStat->length = 3u;
				break;
			case USBFS_MIDI_PROGRAM_CHANGE:
			case USBFS_MIDI_CHANNEL_PRESSURE:
				rxStat->length = 2u;
				break;
			default:
				rxStat->runstat = 0u;
				rxStat->count = 0u;
				break;
			}
		}
	} else {
		/* Otherwise, it's a data byte */
		if (rxStat->runstat == USBFS_MIDI_SYSEX) {
			rxStat->msgBuff[rxStat->count] = mData;
			rxStat->count++;
			if (rxStat->count >= rxStat->length) {
				rxStat->size = rxStat->count;
				rxStat->count = 0u;
				midiReturn = USBFS_MIDI_SYSEX;
			}
		} else if (rxStat->count > 0u) {
			rxStat->msgBuff[rxStat->count] = mData;
			rxStat->count++;
			if (rxStat->count >= rxStat->length) {
				rxStat->size = rxStat->count;
				rxStat->count = 0u;
				midiReturn = rxStat->size;
			}
		} else if (rxStat->runstat != 0u) {
			rxStat->msgBuff[0u] = rxStat->runstat;
			rxStat->msgBuff[1u] = mData;
			rxStat->count = 2u;
			switch (rxStat->runstat & USBFS_MIDI_STATUS_MASK) {
			case USBFS_MIDI_NOTE_OFF:
			case USBFS_MIDI_NOTE_ON:
			case USBFS_MIDI_POLY_KEY_PRESSURE:
			case USBFS_MIDI_CONTROL_CHANGE:
			case USBFS_MIDI_PITCH_BEND_CHANGE:
				rxStat->length = 3u;
				break;
			case USBFS_MIDI_PROGRAM_CHANGE:
			case USBFS_MIDI_CHANNEL_PRESSURE:
				rxStat->size = rxStat->count;
				rxStat->count = 0u;
				midiReturn = rxStat->size;
				break;
			default:
				rxStat->count = 0u;
				break;
			}
		} else {
		}
	}
	
	return (midiReturn);
}

void ProcessMidi()
{
	for (int i=0; i<NMAXBOARDS; i++) {
		uint8_t msgRtn = 0u;
		uint8_t rxData;
		uint8_t hasData = 0;
		do {
			hasData = cbus_midi_read(i, &rxData);
			if (hasData) {
				msgRtn = ProcessMidiIn(rxData, &midiRxStatus[i]);
			}
		} while (hasData && msgRtn == 0);
		
		if (msgRtn != 0) {
			USBFS_PutUsbMidiIn(msgRtn, (const uint8_t*)&midiRxStatus[i].msgBuff[0], i);
		}
	}
	
	/* Call this API from UART RX ISR for Auto DMA mode */
#if(!USB_EP_MANAGEMENT_DMA_AUTO)
	USBFS_MIDI_IN_Service();
#endif
	/* In Manual EP Memory Management mode OUT_EP_Service()
	 *  may have to be called from main foreground or from OUT EP ISR
	 */
#if(!USB_EP_MANAGEMENT_DMA_AUTO)
	USBFS_MIDI_OUT_Service();
#endif
}


// -------------------------------------

static void _ReadDieID(uint8 descr[]) 
{
	uint8 i;
	uint8 j = 0u;
	uint8 value;
	const char8 CYCODE hex[16u] = "0123456789ABCDEF";
	const char8 prefix[] = "honet.kk@gmail.com:";  //19文字
	int len = strlen(prefix);

	/* Check descriptor validation */
	if(descr != NULL) {
		j=2u;

		for (i=0u; i<len; i++){
			descr[j++] = prefix[i];
			descr[j++] = 0;
		}

		/* fill descriptor : WAFER-ID 16文字*/
		for (i = 0u; i<8; i++) {
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
	uint8_t hasMidi = 0;
	
	msg_widx = 0;
	msg_ridx = 0;
    msg_length = 0;
    last_tick = 0;

    EEPROM_Start();
	led_init();
	led_on(LEDID_POWER);
	
	// generate USB S/N string descriptor
	_ReadDieID(SNStringDesc);
	USBFS_SerialNumString(SNStringDesc);

	/* Enable global interrupts */
	CYGlobalIntEnable;

	// setup
	TickSetup();
	cbus_reset();
	cbus_board_setup();
//    while(1) sidbench();

	for (int i=0; i<NMAXBOARDS; i++){
		if (cbus_has_midi(i)) hasMidi = 1;
	}
	
	// wait board init.
	while (0<msg_length);

	// start the USB.
	USBFS_Start(hasMidi ? 1 : 0, USBFS_DWR_VDDD_OPERATION);

	/* Wait until device address is assigned */
	while (USBFS_deviceAddress == UnassignedAddress);

	for (;;) {
		/* Host can send double SET_INTERFACE request */
		if (USBFS_IsConfigurationChanged() != 0) {
			/* Initialize IN endpoints when device configured */
			if (USBFS_GetConfiguration() != 0) {
				USBFS_EnableOutEP(USB_OUT_EP);
				led_on(LEDID_CONNECT);
				
				if (hasMidi) {
					for (int i=0; i<NMAXBOARDS; i++){
						memset(&midiRxStatus[i], 0, sizeof(USBFS_MIDI_RX_STATUS));
					}
					
					/* Enable output endpoint */
					USBFS_MIDI_Init();
				}
			}
		}
		
		if (USBFS_GetConfiguration() != 0) {
			BulkTransfer();
			ProcessData();
			if (hasMidi) ProcessMidi();

			if (USBFS_transferState == USBFS_TRANS_STATE_IDLE) {
				if (usbReq_reset){
                    // BUSリセットしてしまうとMPU使用時にMIDIが吹っ飛んでしまうのでここではやらない
					//cbus_reset();
					for(int i=0; i<NMAXBOARDS; i++){
						cbus_board_init(i);
					}
					usbReq_reset = 0;
				}

				if (usbReq_setBoardType) {
					for (int i=0; i<NMAXBOARDS; i++){
						if (usbReq_setBoardType & (0x01<<i)){
							conf_set_board_type( i, usbReq_boardType[i] );
						}
					}
					usbReq_setBoardType = 0;
				}
			}
		} else {
			led_off(LEDID_CONNECT);
		}
	}

	// never reach.
	led_off(0);
}


/* [] END OF FILE */

