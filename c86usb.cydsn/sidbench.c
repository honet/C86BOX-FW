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
#include <stdint.h>
#include <cbus.h>

void bus_write(uint8_t addr, uint8_t data){
    cbus_write8(1, 0x789, addr); // addr
	cbus_write8(1, 0x788, data); // data
}

static void wait(){
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

/* [] END OF FILE */
