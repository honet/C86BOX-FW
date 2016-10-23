
#include <project.h>
#include <stdint.h>
#include "cbus.h"
#include "cbus_init118.h"

#define TRUE  0x0001
#define FALSE 0x0000


static void initOPL(uint8_t bidx)
{
	int cnt;

	/* Initialize OPL for MIDI */

	cbus_write8(bidx, 0x0485, 0xaa);
	cbus_write8(bidx, 0x0485, 0x99);
	cbus_write8(bidx, 0x0485, 0x2a);

	for (int i=0; i<5; i++) {
		cbus_write8(bidx, 0x0485, Data0485_99[i]);
        CyDelayUs(1);//cbus_write8(bidx, 0x5f, 0x00);
	}

	/* 処理終了 */
	cbus_write8(bidx, 0x0485+1, 0x00);

	cbus_write8(bidx, 0x0485, 0xaa);
	cbus_write8(bidx, 0x0485, 0x9e);
	cbus_write8(bidx, 0x0485, 0x2a);

	cnt=3;
	for (int i=0; i<3; i++) {
		if (cbus_read8(bidx, 0x0485) != Data0485_9E[i])
			i=0xff;
		cnt--;
	}

	/* 処理終了 */
	cbus_write8(bidx, 0x0485+1,0x00);

	CyDelayUs(1500); //wait(2000);


	if (cnt != 0) {
		cbus_write8(bidx, 0x0485, 0xaa);
		cbus_write8(bidx, 0x0485, 0x36);
		cbus_write8(bidx, 0x0485, 0x28);

		for (int i=0; i<14; i++){
			cbus_write8(bidx, 0x0485, Data0485_36[i]);
    		CyDelayUs(1);//cbus_write8(bidx, 0x5f, 0x00);
		}

		/* 処理終了 */
		cbus_write8(bidx, 0x0485+1,0x00);

		CyDelayUs(1200); //wait(1500);

		cbus_write8(bidx, 0x148e, 0x05);
		cnt = cbus_read8(bidx, 0x148f) | 0x08;
		cbus_write8(bidx, 0x148f, cnt);

		cbus_write8(bidx, 0x148c, 0xff);
		cbus_write8(bidx, 0x148e, 0x05);
		cnt=cbus_read8(bidx, 0x148f) & 0xf7;
		cbus_write8(bidx, 0x148f, cnt);

		CyDelayUs(1200); //wait(1500);
	}

	cbus_write8(bidx, 0x0485, 0xaa);
	cbus_write8(bidx, 0x0485, 0xa9);
	cbus_write8(bidx, 0x0485, 0x21);

	for (int i=0; i<3159; i++){
		cbus_write8(bidx, 0x0485, Data0485_A9[i]);
        CyDelayUs(1);	//cbus_write8(bidx, 0x5f, 0x00);
	}
	/* 処理終了 */
	cbus_write8(bidx, 0x0485+1, 0x00);

    CyDelayUs(1200); //wait(1500);

	cbus_write8(bidx, 0x0485, 0xaa);
	cbus_write8(bidx, 0x0485, 0x0c);
	cbus_write8(bidx, 0x0485, 0x20);

	for (int i=0; i<3; i++){
		cbus_write8(bidx, 0x0485, Data0485_0C[i]);
        CyDelayUs(1); //cbus_write8(bidx, 0x5f, 0x00);
	}
	/* 処理終了 */
	cbus_write8(bidx, 0x0485+1, 0x00);


	cbus_write8(bidx, 0x0485, 0xaa);
	cbus_write8(bidx, 0x0485, 0x66);
	cbus_write8(bidx, 0x0485, 0x20);

	for (int i=0; i<6; i++) {
		cbus_write8(bidx, 0x0485, Data0485_66[i]);
   		CyDelayUs(1); //cbus_write8(bidx, 0x5f, 0x00);
	}
	/* 処理終了 */
	cbus_write8(bidx, 0x0485+1, 0x00);


	cbus_write8(bidx, 0x0485, 0xaa);
	cbus_write8(bidx, 0x0485, 0x60);
	cbus_write8(bidx, 0x0485, 0x20);

	for (int i=0; i<3; i++) {
		cbus_write8(bidx, 0x0485, Data0485_60[i]);
   		CyDelayUs(1); //cbus_write8(bidx, 0x5f, 0x00);
	}
	/* 処理終了 */
	cbus_write8(bidx, 0x0485+1, 0x00);

}

static void sendMPU(uint8_t bidx, int cmd)
{
	uint16_t i;

	for (i=0; i<65000; i++) {
		cbus_read8(bidx, 0x148c);
		if (!(cbus_read8(bidx, 0x148c+1)&0x40)) // check DRR bit.
			break;
	}

	if (i<65000) {
		cbus_write8(bidx, 0x148c+1, cmd);
		for (i=0; i<65000; i++){
			if (!(cbus_read8(bidx, 0x148c+1)&0x80)) { // check DSR bit.
				if(cbus_read8(bidx, 0x148c)==0xfe) // ACK?
					break;
			}
		}
	}
	else
		cbus_write8(bidx, 0x148c+1, 0xff);

}

static void setUART(uint8_t bidx)
{
	sendMPU(bidx, 0xff); // MPU-reset

	for (int i=0;i<310;i++)
		CyDelayUs(600); //wait(1000);

	sendMPU(bidx, 0x3f); // UART-mode
}

static int mode118(uint8_t bidx)
{
	cbus_write8(bidx, 0x148e, 0x05);
	cbus_write8(bidx, 0x148f, 0x0c);
	cbus_read8(bidx, 0x148c);

	initOPL(bidx);

	cbus_write8(bidx, 0x148e, 0x05);
	cbus_write8(bidx, 0x148f, 0x04);

	/* mode seting end */
	cbus_write8(bidx, 0x148e, 0x05);
	cbus_write8(bidx, 0x148f, 0x00);

	CyDelayUs(21000); //wait(35000);

	cbus_write8(bidx, 0x148e, 0x05);

	for(uint16_t i=0; i<65535; i++){
		if(cbus_read8(bidx, 0x148f)==0){

			//;148D<=3F(MIDI UART)
			cbus_write8(bidx, 0x148c+1,0x3f);

			return(TRUE);
		}
	}

	return(FALSE);
}

static int exist118(uint8_t bidx) {
	/* 118reg->21h  midi int seting */
	cbus_write8(bidx, 0x148e, 0x21);

	/* int num */
	cbus_write8(bidx, 0x148f, 0);


	cbus_write8(bidx, 0x148e, 0x20);
	cbus_write8(bidx, 0x148f, 0x00);
	/* モード設定開始 */
	cbus_write8(bidx, 0x148e, 0x05);
	cbus_write8(bidx, 0x148f, 0x04);

	CyDelayUs(21000); //wait(35000);

	cbus_write8(bidx, 0x148e, 0x05);

	for (uint16_t i=0; i<65535; i++) {
		if (cbus_read8(bidx, 0x148f)==4) {
			return(TRUE);
		}
	}

	return(FALSE);
}

void board118_init(uint8_t slot){
	if (!exist118(slot)) {
		// 118存在チェック失敗
		return;
	}

    // 118がハングアップしてIORDYがアサートされっぱなしになってしまうので断念。
    // なにやってるか不明だし、OPNA/OPL3についてはやらなくても音出るみたいなのでまあいいか。
//	if (!mode118(slot)) {
//		// 118設定失敗
//		return;
//	}
	
	setUART(slot);
}

