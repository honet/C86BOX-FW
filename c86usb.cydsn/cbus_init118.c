
#include <project.h>
#include <stdint.h>
#include "cbus.h"
#include "cbus_init118.h"
#include "tick.h"

#define TRUE  0x0001
#define FALSE 0x0000


static void board118_write_ctrl148e(uint8_t bidx, uint8_t reg, uint8_t data){
	cbus_write8(bidx, 0x148e, reg);
	cbus_write8(bidx, 0x148f, data);
}

static uint8_t board118_read_ctrl148e(uint8_t bidx, uint8_t reg){
	cbus_write8(bidx, 0x148e, reg);
	return cbus_read8(bidx, 0x148f);
}

// INIT118.COM相当の初期化処理。詳細不明。
static void initOPL(uint8_t bidx)
{
	int cnt;

	/* Initialize OPL for MIDI */
	
	cbus_write8(bidx, 0x0485, 0xaa);
	cbus_write8(bidx, 0x0485, 0x99);
	cbus_write8(bidx, 0x0485, 0x2a);

	for (int i=0; i<5; i++) {
		cbus_write8(bidx, 0x0485, Data0485_99[i]);
        CyDelayUs(1);
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
    		CyDelayUs(1);
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
        CyDelayUs(1);
	}
	/* 処理終了 */
	cbus_write8(bidx, 0x0485+1, 0x00);

    CyDelayUs(1200); //wait(1500);
	cbus_write8(bidx, 0x0485, 0xaa);
	cbus_write8(bidx, 0x0485, 0x0c);
	cbus_write8(bidx, 0x0485, 0x20);

	for (int i=0; i<3; i++){
		cbus_write8(bidx, 0x0485, Data0485_0C[i]);
        CyDelayUs(1);
	}
	/* 処理終了 */
	cbus_write8(bidx, 0x0485+1, 0x00);

	cbus_write8(bidx, 0x0485, 0xaa);
	cbus_write8(bidx, 0x0485, 0x66);
	cbus_write8(bidx, 0x0485, 0x20);

	for (int i=0; i<6; i++) {
		cbus_write8(bidx, 0x0485, Data0485_66[i]);
   		CyDelayUs(1);
	}
	/* 処理終了 */
	cbus_write8(bidx, 0x0485+1, 0x00);

	cbus_write8(bidx, 0x0485, 0xaa);
	cbus_write8(bidx, 0x0485, 0x60);
	cbus_write8(bidx, 0x0485, 0x20);

	for (int i=0; i<3; i++) {
		cbus_write8(bidx, 0x0485, Data0485_60[i]);
   		CyDelayUs(1);
	}
	/* 処理終了 */
	cbus_write8(bidx, 0x0485+1, 0x00);
}

static int mode118(uint8_t bidx)
{
    // OPL設定変更開始
	board118_write_ctrl148e(bidx, 0x05, 0x0c);
	cbus_read8(bidx, 0x148c);

    // ここのロングウエイト重要。無いとボードがフリーズしてIORDYがアサートされたままになる。
    TickWaitDelta(WAIT1MS*200);// 100ms

	initOPL(bidx);

    // 設定変更終了
	board118_write_ctrl148e(bidx, 0x05, 0x00);

	CyDelayUs(21000); //wait(35000);

	return(FALSE);
}

extern void led_set(uint8_t idx, uint8_t sw);

static int exist118(uint8_t bidx) 
{
    uint8_t reg;
	// 118ボード存在確認ビットチェック
    // ->リセット後の状態がなんだか安定しない。意味が違うのかも。
	//reg = board118_read_ctrl148e(bidx, 0);
	//if ((reg & 0x08) != 0){
	//	return FALSE;
    //}

	// 割り込みは使わないので設定しない
	// MIDI割り込みレベル設定 INT41
	//board118_write_ctrl148e(bidx, 0x21, 0);
	// MIDI割込変更F/F 設定変更禁止
	//board118_write_ctrl148e(bidx, 0x20, 0);
	
	// モード設定開始
	board118_write_ctrl148e(bidx, 0x05, 0x04);
	CyDelayUs(21000);

	uint8_t ack = FALSE;

	// モード設定変更開始状態に遷移したかチェック
	for (int i=0; i<100; i++) {
		reg = board118_read_ctrl148e(bidx, 0x05);
		if (reg == 4){
			ack = TRUE;
			break;
		}
		CyDelayUs(1000);
	}
	if (ack == FALSE) return FALSE;


	// モード設定終了
	board118_write_ctrl148e(bidx, 0x05, 0x00);
	CyDelayUs(21000);
    
	ack = FALSE;
	for(int i=0; i<100; i++){
		reg = board118_read_ctrl148e(bidx, 0x05);
		if (reg == 0){
			ack = TRUE;
			break;
		}
		CyDelayUs(1000);
	}
	if (ack == FALSE) return FALSE;

	return(TRUE);
}

void board118_init(uint8_t slot)
{
    // ここのウエイト削ると認識失敗する.なぜ？
    //TickWaitDelta(WAIT1MS*200);
    
	if (!exist118(slot)) {
		// 118存在チェック失敗
        led_set(3,1);
		return;
	}

	// 118初期化処理
	if (!mode118(slot)) {
		// 118設定失敗
		return;
	}
	
	// MPUの初期化は別途cbus_mpuのルーチンで実施
}

