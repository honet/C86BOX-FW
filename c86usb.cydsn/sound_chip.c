#include <project.h>
#include <stdint.h>
#include "timer_if.h"
#include "cbus.h"
#include "sound_chip.h"

// TODO: だいぶいい加減なので直す（OPNとっても遅いよ！）
const uint16_t waitdef_2203_MC4[5] = { 100,120,150,200,1152 };

// データシート通り（@8MHz値。本当は7.987MHzだけれど、誤差範囲なので気にしないことにする）
const uint16_t waitdef_2608[5] = { 0, 17, 47, 83, 576 };
// 上位4bit:アドレスライトウェイト  下位4bit:データライトウェイト
const uint8_t waitidx_2608[512] = {
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x41,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x00,0x00,
	0x00,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,
	0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,
	0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,
	0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,
	0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,
	0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,
	0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,
	0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x01,
	0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,
	0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,
	0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,
	0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,
	0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,
	0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,
	0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,
	0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,
	0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x01,
	0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,
	0x21,0x21,0x21,0x21,0x21,0x21,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};


// exaddr = [7:4]slot,[3:1]chip,[0:0]ex
void ym2203_write(CHIP_INFO *chip, uint8_t exaddr, uint8_t addr, uint8_t data)
{
	//uint8_t ex = exaddr&0x01;
	uint16_t widx = addr;
	uint8_t  waitidx;
	uint16_t waitcount;

	// wait
	chip->wait_timerif->Wait();

	// write address ------------
	cbus_write(chip->slot, chip->areg_addr[0], addr);
	// set a after wait timer
	waitidx = chip->waitidx[widx];
	waitcount = chip->waitdef[waitidx&0x0f];
	chip->wait_timerif->Set(waitcount);
	// wait
	chip->wait_timerif->Wait();

	// write data --------------
	cbus_write(chip->slot, chip->dreg_addr[0], data);
	// set a after wait timer
	waitcount = chip->waitdef[(waitidx>>4)&0x0f];
	chip->wait_timerif->Set(waitcount);
}

// exaddr = [7:4]slot,[3:1]chip,[0:0]ex
void ym2608_write(CHIP_INFO *chip, uint8_t exaddr, uint8_t addr, uint8_t data)
{
	uint8_t ex = exaddr&0x01;
	uint16_t widx = ((uint16_t)ex<<8) | addr;
	uint8_t slot = chip->slot;
	uint8_t  waitidx;
	uint16_t waitcount;

	// wait
	chip->wait_timerif->Wait();

	if (ex==1 && addr==0x08){
		uint32_t areg = chip->areg_addr[1];
		uint32_t dreg = chip->dreg_addr[1];
		
		// ADPCMはウェイト無し
		// write address
		cbus_write(slot, areg, 0x08);
		// write data
		cbus_write(slot, dreg, data);
		
		// reset BRDY flag --------------
		cbus_write(slot, areg, OPNA_ADPCM_FLAG_REG);
		cbus_write(slot, dreg,
						OPNA_ADPCM_FLAG_BIT_MASK_ALL & ~OPNA_ADPCM_FLAG_BIT_MASK_EOS );
		
		// enable BRDY and EOS flag. ----
		cbus_write(slot, dreg,
						OPNA_ADPCM_FLAG_BIT_MASK_ALL &
						~( OPNA_ADPCM_FLAG_BIT_MASK_BRDY |
						   OPNA_ADPCM_FLAG_BIT_MASK_EOS ) );

		// status check.
		for( int retry = 0; retry<100; retry++ ){
			volatile uint8_t status = ym2608_read_status( chip, 1 );
			if( status & OPNA_STATUS_BIT_BRDY )
				break;
			chip->wait_timerif->Set(100);
			chip->wait_timerif->Wait();
		}
	}
	else{
		// CHECK ME!!
		// リズムイベント発行前に強制ウェイトを入れないとなぜかtomtomが暴発する。
		if( ex==0 && addr==0x10 ){
			chip->wait_timerif->Set(1000);
			chip->wait_timerif->Wait();
		}

		// write address ------------
		cbus_write(slot, chip->areg_addr[ex], addr);
		// set a after wait timer
		waitidx = chip->waitidx[widx];
		waitcount = chip->waitdef[waitidx&0x0f];
		chip->wait_timerif->Set(waitcount);
		// wait
		chip->wait_timerif->Wait();

		// write data --------------
		cbus_write(slot, chip->dreg_addr[ex], data);
		// set a after wait timer
		waitcount = chip->waitdef[(waitidx>>4)&0x0f];
		chip->wait_timerif->Set(waitcount);
	}
}


uint8_t ym2608_read(CHIP_INFO *chip, uint8_t exaddr, uint8_t addr)
{
	uint8_t ex = (exaddr & 0x01) ? 1 : 0;
	uint16_t widx = ((uint16_t)ex<<8) | addr;
	uint8_t  waitidx;
	uint16_t waitcount;
	
	// write address ------------
	cbus_write(chip->slot, chip->areg_addr[ex], addr);
	// set a after wait timer
	waitidx = chip->waitidx[widx];
	waitcount = chip->waitdef[waitidx&0x0f];
	chip->wait_timerif->Set(waitcount);
	// wait
	chip->wait_timerif->Wait();

	// read data はウエイトいらないよね？
	return (cbus_read(chip->slot, chip->dreg_addr[ex]) & 0xff);
}

uint8_t ym2608_read_status(CHIP_INFO *chip, uint8_t ex)
{
	return (cbus_read(chip->slot, chip->areg_addr[ex]) & 0xff);
}

// ------------------------------------
void ym2608_init(CHIP_INFO *chip)
{
	int i=0;

	// FM全チャネルキーオフ
	ym2608_write(chip, 0, 0x28, 0x00);
	ym2608_write(chip, 0, 0x28, 0x01);
	ym2608_write(chip, 0, 0x28, 0x02);
	ym2608_write(chip, 0, 0x28, 0x04);
	ym2608_write(chip, 0, 0x28, 0x05);
	ym2608_write(chip, 0, 0x28, 0x06);
	// FM TL=127
	for(i=0x40;i<=0x4F;i++)
	{
		ym2608_write(chip, 0, i, 0x7F);
		ym2608_write(chip, 1, i, 0x7F);
	}
	// FM ML/DT
	for(i=0x30;i<0x3F+1;i++)
	{
		ym2608_write(chip, 0, i, 0x00);
		ym2608_write(chip, 1, i, 0x00);
	}
	// FM AR,DR,SR,KS,AMON
	for(i=0x50;i<=0x7F;i++)
	{
		ym2608_write(chip, 0, i, 0x00);
		ym2608_write(chip, 1, i, 0x00);
	}
	// FM SL,RR
	for(i=0x80;i<=0x8F;i++)
	{
		ym2608_write(chip, 0, i, 0xFF);
		ym2608_write(chip, 1, i, 0xFF);
	}
	// FM F-Num, FB/CONNECT
	for(i=0x90;i<=0xBF;i++)
	{
		ym2608_write(chip, 0, i, 0x00);
		ym2608_write(chip, 1, i, 0x00);
	}
	// FM PAN/AMS/PMS
	for(i=0xB4;i<=0xB6;i++)
	{
		ym2608_write(chip, 0, i, 0xC0);
		ym2608_write(chip, 1, i, 0xC0);
	}
	ym2608_write(chip, 0, 0x22, 0x00); // HW LFO
	ym2608_write(chip, 0, 0x24, 0x00); // Timer-A(1)
	ym2608_write(chip, 0, 0x25, 0x00); // Timer-A(2)
	ym2608_write(chip, 0, 0x26, 0x00); // Timer-B
	ym2608_write(chip, 0, 0x27, 0x30); // Timer Control
	ym2608_write(chip, 0, 0x29, 0x80); // FM4-6 Enable, All IRQ Disable

	// SSG 音程(2byte*3ch)
	for(i=0x00;i<=0x05;i++)
	{
		ym2608_write(chip, 0, i, 0x00);
	}
	ym2608_write(chip, 0, 0x06, 0x00); // SSG ノイズ周波数
	ym2608_write(chip, 0, 0x06, 0x38); // SSG ミキサ
	// SSG ボリューム(3ch)
	for(i=0x08;i<=0x0A;i++)
	{
		ym2608_write(chip, 0, i, 0x00);
	}
	// SSG Envelope
	for(i=0x0B;i<=0x0D;i++)
	{
		ym2608_write(chip, 0, i, 0x00);
	}

	// RHYTHM
	ym2608_write(chip, 0, 0x10, 0xBF); // 強制発音停止
	ym2608_write(chip, 0, 0x11, 0x00); // Rhythm Total Level
	ym2608_write(chip, 0, 0x18, 0x00); // BD音量
	ym2608_write(chip, 0, 0x19, 0x00); // SD音量
	ym2608_write(chip, 0, 0x1A, 0x00); // CYM音量
	ym2608_write(chip, 0, 0x1B, 0x00); // HH音量
	ym2608_write(chip, 0, 0x1C, 0x00); // TOM音量
	ym2608_write(chip, 0, 0x1D, 0x00); // RIM音量
}


// addr, size は32byte単位なので注意。
void ym2608_write_adpcm(CHIP_INFO *chip, uint16_t addr, uint8_t *data, uint16_t size)
{
	uint32_t i, num = ((uint32_t)size)<<5;
	uint16_t high, low, stop, limit;
	
	stop = addr + size - 1;
	limit = stop;
//	stop = 0xffff;
//	limit = 0xffff;

	ym2608_write(chip, 1, OPNA_ADPCM_CTRL1_REG, OPNA_ADPCM_CTRL1_BIT_RESET );


	// Enable BRDY and EOS flag
	ym2608_write(chip, 1, OPNA_ADPCM_FLAG_REG,
					OPNA_ADPCM_FLAG_BIT_MASK_ALL &
					~( OPNA_ADPCM_FLAG_BIT_MASK_BRDY |
					   OPNA_ADPCM_FLAG_BIT_MASK_EOS ) );

	// IRQ flag reset.
	ym2608_write(chip, 1, OPNA_ADPCM_FLAG_REG,
					OPNA_ADPCM_FLAG_BIT_IRQRESET );

	// set to memory write mode
	ym2608_write(chip, 1, OPNA_ADPCM_CTRL1_REG,
					OPNA_ADPCM_CTRL1_BIT_REC |
					OPNA_ADPCM_CTRL1_BIT_MEMDATA );

	// set memory type to DRAM
	ym2608_write(chip, 1, OPNA_ADPCM_CTRL2_REG,
					OPNA_ADPCM_CTRL2_BIT_RAM_BUS8 );
	
	// set limit address
	low = limit&0xff;
	high = (limit>>8)&0xff;
	ym2608_write(chip, 1, OPNA_ADPCM_LIMITL_REG, low );
	ym2608_write(chip, 1, OPNA_ADPCM_LIMITH_REG, high );

	// set start address
	low = addr&0xff;
	high = (addr>>8)&0xff;
	ym2608_write(chip, 1, OPNA_ADPCM_STARTL_REG, low );
	ym2608_write(chip, 1, OPNA_ADPCM_STARTH_REG, high );

	// set stop address
	low = stop&0xff;
	high = (stop>>8)&0xff;
	ym2608_write(chip, 1, OPNA_ADPCM_STOPL_REG, low );
	ym2608_write(chip, 1, OPNA_ADPCM_STOPH_REG, high );
	
	
	for( i=0; i<num; i++ ){
		// data write
		ym2608_write(chip, 1, OPNA_ADPCM_DATA_REG, *data++ );
		
		// BRDY待ちはym2608_write内で行っているので不要。
	}

	// finalize
	ym2608_write(chip, 1, OPNA_ADPCM_CTRL1_REG, 0 );
	ym2608_write(chip, 1, OPNA_ADPCM_FLAG_REG, OPNA_ADPCM_FLAG_BIT_IRQRESET );
}

// addr, size は32byte単位なので注意。
void ym2608_read_adpcm(CHIP_INFO *chip, uint16_t addr, uint8_t *data, uint16_t size)
{
	uint32_t i, num;
	uint16_t high, low, stop, limit, retry;
	volatile uint8_t status=0;

	num = (size<<5) + 2; // dummy read *2
	stop = addr + size - 1;
	limit = stop;
//	stop = 0xffff;
//	limit = 0xffff;
	
	ym2608_write(chip, 1, OPNA_ADPCM_CTRL1_REG,
					OPNA_ADPCM_CTRL1_BIT_RESET );

	// Enable BRDY and EOS flag
	ym2608_write(chip, 1, OPNA_ADPCM_FLAG_REG,
					OPNA_ADPCM_FLAG_BIT_MASK_ALL &
					~( OPNA_ADPCM_FLAG_BIT_MASK_BRDY |
					   OPNA_ADPCM_FLAG_BIT_MASK_EOS ) );

	// IRQ flag reset.
	ym2608_write(chip, 1, OPNA_ADPCM_FLAG_REG,
					OPNA_ADPCM_FLAG_BIT_IRQRESET );

	// set to memory read mode
	ym2608_write(chip, 1, OPNA_ADPCM_CTRL1_REG,
					OPNA_ADPCM_CTRL1_BIT_MEMDATA );

	// set memory type to DRAM
	ym2608_write(chip, 1, OPNA_ADPCM_CTRL2_REG,
					OPNA_ADPCM_CTRL2_BIT_RAM_BUS8 );
	
	// set limit address
	low = limit&0xff;
	high = (limit>>8)&0xff;
	ym2608_write(chip, 1, OPNA_ADPCM_LIMITL_REG, low );
	ym2608_write(chip, 1, OPNA_ADPCM_LIMITH_REG, high );

	// set start address
	low = addr&0xff;
	high = (addr>>8)&0xff;
	ym2608_write(chip, 1, OPNA_ADPCM_STARTL_REG, low );
	ym2608_write(chip, 1, OPNA_ADPCM_STARTH_REG, high );

	// set stop address
	low = stop&0xff;
	high = (stop>>8)&0xff;
	ym2608_write(chip, 1, OPNA_ADPCM_STOPL_REG, low );
	ym2608_write(chip, 1, OPNA_ADPCM_STOPH_REG, high );
	
	for( i=0; i<num; i++ ){
		// data read
		if( i<2 ){
			// 最初の２回はダミーリード（バグだよねぇ、これ。）
			volatile uint8_t dummy __attribute__((unused)) = ym2608_read( chip, 1, OPNA_ADPCM_DATA_REG );
		}else{
			*data++ = ym2608_read( chip, 1, OPNA_ADPCM_DATA_REG );
		}

		// reset BRDY flag
		ym2608_write(chip, 1, OPNA_ADPCM_FLAG_REG,
						OPNA_ADPCM_FLAG_BIT_MASK_ALL & ~OPNA_ADPCM_FLAG_BIT_MASK_EOS );
		
		// enable BRDY and EOS flag.
		ym2608_write(chip, 1, OPNA_ADPCM_FLAG_REG,
						OPNA_ADPCM_FLAG_BIT_MASK_ALL &
						~( OPNA_ADPCM_FLAG_BIT_MASK_BRDY |
						   OPNA_ADPCM_FLAG_BIT_MASK_EOS ) );

		// status check.
		// 最後の２回はフラグが立たない。
		if( i < num-2 ){
			for( retry = 0; retry<100; retry++ ){
				status = ym2608_read_status( chip, 1 );
				if( status & OPNA_STATUS_BIT_BRDY )
					break;
            }
        }

		if( status & OPNA_STATUS_BIT_EOS )
			break;        
	}

	// finalize
	ym2608_write(chip, 1, OPNA_ADPCM_CTRL1_REG, 0 );
	ym2608_write(chip, 1, OPNA_ADPCM_FLAG_REG, OPNA_ADPCM_FLAG_BIT_IRQRESET );
}

// ADPCM-RAM 有り: 1
// ADPCM-RAM 無し: 0
uint8_t ym2608_check_adpcm(CHIP_INFO *chip)
{
	uint8_t pattern[32]={
		0x00, 0xa5, 0x5a, 0x55, 0x00, 0xa5, 0x5a, 0x55, 0x00, 0xa5, 0x5a, 0x55, 0x00, 0xa5, 0x5a, 0x55,
		0x00, 0xa5, 0x5a, 0x55, 0x00, 0xa5, 0x5a, 0x55, 0x00, 0xa5, 0x5a, 0x55, 0x00, 0xa5, 0x5a, 0x55
		};
	uint8_t tmp[32];
	uint32_t n = 32, i;
	uint8_t flag = 1;

	ym2608_write_adpcm( chip, 0, pattern, n>>5 );
	ym2608_read_adpcm( chip, 0, tmp, n>>5 );
	
	ym2608_write_adpcm( chip, 0, pattern, n>>5 );
	ym2608_read_adpcm( chip, 0, tmp, n>>5 );
    
	for( i=0; i<n; i++ ){
		if( tmp[i] != pattern[i] ){
			flag = 0;
			break;
		}
	}
	return flag;
}
