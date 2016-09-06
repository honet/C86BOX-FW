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

// MESSAGE DEFINITION ------------------------------------------------------
// 00 [7:4]slot,[3:1]chip,[0:0]ex addr data
// 10 cc dd dd  slot.A write control : cc=control no, dd=data
// 11 cc dd dd  slot.B write control : cc=control no, dd=data
// 12 cc dd dd  slot.C write control(reserved) : cc=control no, dd=data
// 13 cc dd dd  slot.D write control(reserved) : cc=control no, dd=data

// f0 sa aa aa  C-BUS address set for data write : aa=addr[19:0]
// f1 s0 dd dd  CBUS data write : s=slot no
// f2 sa aa aa  CBUS data read
// FE nn nn nn  nsync

#define MSGBUF_SIZE      1024//(1024*8) // must be equal to 2^n
#define MSGBUF_SIZEMASK  (MSGBUF_SIZE-1)

volatile uint8_t ep2inbuf[32];

volatile uint32_t msgbuf[MSGBUF_SIZE];
volatile uint32_t msg_widx;
volatile uint32_t msg_ridx;
volatile uint32_t msg_length;
volatile uint8_t ticked = 0;


#define UnassignedAddress 0

#define USB_OUT_EP 0x01
#define USB_IN_EP  0x02

#define IDX(x)		( (x)&MSGBUF_SIZEMASK )
#define REMAIN()	(MSGBUF_SIZE - msg_length)

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
	static uint16_t cbus_addr = 0;

	if(ticked && msg_length>0){
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
            ticked = 0;
			break;

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
					cbus_write( slot, cbus_addr, d&0xffff );
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
				uint16_t x = cbus_read( slot, cbus_addr );
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
				USBFS_ReadOutEP(USB_OUT_EP, (uint8_t*)&msgbuf[widx], (MSGBUF_SIZE-widx));
				USBFS_ReadOutEP(USB_OUT_EP, (uint8_t*)&msgbuf[0], widx+intcount-MSGBUF_SIZE);
			}
			msg_widx += intcount;
            msg_length+=intcount;
			USBFS_EnableOutEP(USB_OUT_EP);
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

void bus_write(uint8_t addr, uint8_t data){
    cbus_write(1, 0x789, addr); // addr
	cbus_write(1, 0x788, data); // data
}

void wait(){
    for(int i=0; i<10000; i++);
}
void sidbench(){
	// SIDBENCH
	//120 v(0)=54272:v(1)=54279:v(2)=54286
	//130 poke54296,15:fori=0to2
	//140 pokev(i)+3,8:pokev(i)+1,0
	//150 pokev(i)+5,8:pokev(i)+6,198:next
	//160 fora=16to128step16:fori=0to2
	//170 if a>64 then pokev(i)+3,0
	//180 pokev(i)+4,a+1
	//190 forf=0to254step2:pokev(i)+1,f:nextf
	//200 pokev(i)+4,a:forw=0to200:nextw
	//210 pokev(i)+4,8:pokev(i)+1,0
	//220 nexti,a:a=1
	//230 fori=0to2:pokev(i)+1,255
	//240 poke54296,(a*16)+15:poke54295,2^i
	//250 pokev(i)+4,129
	//260 forf=0to255:poke54294,f:nextf
	//270 pokev(i)+4,136:nexti
	//280 a=a*2:if a<8 then goto 230
	//310 poke54295,0
	
	//120 v(0)=54272:v(1)=54279:v(2)=54286
	uint16_t v[3];
	v[0] = 0x00; //54272;
	v[1] = 0x07; //54279;
	v[2] = 0x0e; //54286;
    
    for(int i=0; i<=255; i++){
    	bus_write(0x18, i);
    }

	//130 poke54296,15:fori=0to2
	bus_write(0x18, 15);//poke 54296,15:

	//130 poke54296,15:fori=0to2
	for (int i=0; i<3; i++) {
		//140 pokev(i)+3,8:pokev(i)+1,0
		//150 pokev(i)+5,8:pokev(i)+6,198:next
		bus_write(v[i]+3, 0x08); // PW HI
        wait();
		bus_write(v[i]+1, 0x00); // FREQ HI
        wait();
		bus_write(v[i]+5, 0x08); // Attack/Decay      //poke v(i)+5,8:
        wait();
		bus_write(v[i]+6, 0xc6); // Sustain/Release   //poke v(i)+6,198:
        wait();
	}
#if 0
	//160 fora=16to128step16:fori=0to2
	for (int a=16; a<=128; a+=16) {
		for (int i=0; i<3; i++) {
			TRACE("CH%d ", i);

			//170 if a>64 then pokev(i)+3,0
			if (a>64) {
				pRC->out(v[i]+3, 0); // PW HI
			}
			//180 pokev(i)+4,a+1
			pRC->out(v[i]+4, a+1); // Control
			if (a&0x10) { TRACE("TRI "); }
			if (a&0x20) { TRACE("SAW "); }
			if (a&0x40) { TRACE("SQR "); }
			if (a&0x80) { TRACE("NOISE "); }
			TRACE("\n");

			//190 forf=0to254step2:pokev(i)+1,f:nextf
			for (int f=0; f<=254; f+=2) {
				pRC->out(v[i]+1, f); // Freq Hi
				Sleep(10);
			}

			//200 pokev(i)+4,a:forw=0to200:nextw
			pRC->out(v[i]+4, a|0x01); // Control
			Sleep(500); // wait

			//210 pokev(i)+4,8:pokev(i)+1,0
			pRC->out(v[i]+4, 8); // Control  //poke v(i)+4,8:
			pRC->out(v[i]+1, 0); // Freq Hi //poke v(i)+1,0
			//220 nexti,a:a=1
		}
	}
#endif

	//230 fori=0to2:pokev(i)+1,255
	//for (int a=1; a<8; a=a*2) {
int a=1;
int i=0;
//while(1)
    {
		//for (int i=0; i<3; i++) { // to 2:
        {
			//230 fori=0to2:pokev(i)+1,255
			bus_write(v[i]+1, 0xff);      // Freq Hi   //poke v(i)+1,255
            wait();

			//240 poke54296,(a*16)+15:poke54295,2^i
			bus_write(0x18, (a<<4)+0xf); // Mode/Vol  //poke 54296,(a*16)+15:
            wait();
            
			int filt = 0x01<<i;// (int)pow(2.0, i);
			//TRACE("FLT%d", filt);
			bus_write(0x17, filt);       // RES/Filt  // poke 54295,2^i
            wait();

			//250 pokev(i)+4,129
			bus_write(v[i]+4, 0x81);       // Control
            wait();

				bus_write(0x16, 0);    // FC Hi     // poke 54294,f:
				bus_write(0x16, 255);    // FC Hi     // poke 54294,f:
				bus_write(0x16, 0);    // FC Hi     // poke 54294,f:
				bus_write(0x16, 0);    // FC Hi     // poke 54294,f:
			//260 forf=0to255:poke54294,f:nextf
			for (int f=0; f<=255; f++) {
				bus_write(0x16, 255);    // FC Hi     // poke 54294,f:
                for(int k=0; k<10000; k++);
			}
			//270 pokev(i)+4,136:nexti
			bus_write(v[i]+4, 0x88);       // Control

            for(int k=0; k<100000; k++);
			//280 a=a*2:if a<8 then goto 230
		}
	}
	//310 poke54295,0
	bus_write(0x17, 0); // RES/Filt  //poke 54295,0
    wait();

    //cbus_reset();
	//cbus_board_setup();
}

// -------------------------------------
int main()
{
	EEPROM_Start();
	led_init();
    
	led_on(0);

	cbus_reset();
	cbus_board_setup();

//    while(1) sidbench();
	
	msg_widx = 0;
	msg_ridx = 0;
    msg_length = 0;

	// generage USB S/N string descriptor
	_ReadDieID(SNStringDesc);
	USBFS_SerialNumString(SNStringDesc);

    /* Enable global interrupts */
	CYGlobalIntEnable;

	// setup
	TickTimerISR_StartEx(Tick);
	TickTimer_Start();

	// board init.
	while(0<msg_length);

	// start the USB.
	USBFS_Start( 0, USBFS_DWR_VDDD_OPERATION );

	/* Wait until device address is assigned */
	while(USBFS_deviceAddress == UnassignedAddress);

	for (;;){
		if (USBFS_GetConfiguration() != 0){
			if (USBFS_IsConfigurationChanged() != 0){
				USBFS_EnableOutEP(USB_OUT_EP);
				led_on(1);
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
			led_off(1);
		}
	}

	// never reach.
	led_off(0);
}


/* [] END OF FILE */

