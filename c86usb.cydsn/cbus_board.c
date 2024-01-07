#include <stdint.h>
#include <project.h>
#include "cbus.h"
#include "cbus_board.h"
#include "cbus_board14.h"
#include "eeprom_config.h"
#include "cbus_init118.h"
#include "cbus_mpu.h"
#include "tick.h"

struct tag_CBUS_BOARD_INFO;


typedef enum {
	SOUND_98DOPLUS = 0x00, // RESERVE
	SOUND_98GS     = 0x01, // RESERVE
	SOUND_73_018X  = 0x02,
	SOUND_73_028X  = 0x03,
	SOUND_86_018X  = 0x04,
	SOUND_86_028X  = 0x05,
	SOUND_9821NF   = 0x06, // RESERVE
	SOUND_9821X    = 0x07, // RESERVE
	SOUND_118      = 0x08,
	SOUND_9821C    = 0x08,
	SOUND_NONE     = 0xff
} SOUND_ID;

CBUS_BOARD_INFO boards[NMAXBOARDS];
static uint8_t has_lowbit_decoder_board = 0;
static uint8_t locked_base_addr[4] = {0};

// ------------------------------------
// function declaration
void board86_control(CBUS_BOARD_INFO *board, uint8_t idx, uint8_t data);


uint32_t cbus_get_board_type(uint8_t slot)
{
	if (slot<NMAXBOARDS)
		return boards[slot].boardtype;
	else
		return CBUS_BOARD_UNKNOWN;
}

uint32_t cbus_get_nchips(uint8_t slot)
{
	if (slot<NMAXBOARDS)
		return boards[slot].nchips;
	else
		return CHIP_UNKNOWN;
}

uint32_t cbus_get_chip_type(uint8_t slot, uint8_t chipidx)
{
	if (slot<NMAXBOARDS && chipidx<NMAXCHIPS)
		return boards[slot].chip[chipidx].chiptype;
	else
		return CHIP_UNKNOWN;
}

void cbus_board_control_write(uint8_t slot, uint16_t idx, uint16_t data)
{
	if (slot>=NMAXBOARDS)
		return;

	CBUS_BOARD_INFO *board = &boards[slot];

	if (board->control_write){
		board->control_write(board, idx, data);
	}
}
uint16_t cbus_board_control_read(uint8_t slot, uint16_t idx)
{
	if (slot>=NMAXBOARDS)
		return 0;

	CBUS_BOARD_INFO *board = &boards[slot];

	if (board->control_read){
		return board->control_read(board, idx);
	}else{
		return 0;
	}
}


void cbus_board_init(uint8_t slot)
{
	uint8_t d=0;
	
	if (slot>=NMAXBOARDS)
		return;

	CBUS_BOARD_INFO *board = &boards[slot];

	switch(board->boardtype){
	// ---------------------------------
	case CBUS_BOARD_14:
        board14_init(&board->chip[0]);
        break;
        
	// ---------------------------------
	// YM2203 only
	case CBUS_BOARD_26:
	case CBUS_BOARD_MULTIMEDIA_ORCHESTRA:
	case CBUS_BOARD_LITTLE_ORCHESTRA:
	case CBUS_BOARD_LITTLE_ORCHESTRA_L:
	case CBUS_BOARD_LITTLE_ORCHESTRA_RS:
	case CBUS_BOARD_LITTLE_ORCHESTRA_LS:
	case CBUS_BOARD_LITTLE_ORCHESTRA_SS:
	case CBUS_BOARD_LITTLE_ORCHESTRA_MATE:
	case CBUS_BOARD_LITTLE_ORCHESTRA_FELLOW:
	case CBUS_BOARD_JOY2:
	case CBUS_BOARD_SOUND_GRANPRI:
	case CBUS_BOARD_TN_F3FM:
		break;
		
	// ---------------------------------
	// YM2203 + YM3812
	case CBUS_BOARD_SOUND_ORCHESTRA:
	case CBUS_BOARD_SOUND_ORCHESTRA_L:
		break;
	// ---------------------------------
	// YM2203 + Y8950
	case CBUS_BOARD_SOUND_ORCHESTRA_V:
	case CBUS_BOARD_SOUND_ORCHESTRA_VS:
	case CBUS_BOARD_SOUND_ORCHESTRA_LS:
	case CBUS_BOARD_SOUND_ORCHESTRA_MATE:
		break;
		
	// ---------------------------------
	// YM2608 0xA460無し
	case CBUS_BOARD_ASB01:
	case CBUS_BOARD_SOUNDPLAYER98:
	case CBUS_BOARD_SPEAKBOARD:
	case CBUS_BOARD_OTOMI:
		ym2608_init(&board->chip[0]);
		break;
		
	// ---------------------------------
	// YM2608 0xA460有り
	case CBUS_BOARD_73:
	case CBUS_BOARD_86:
	case CBUS_BOARD_WAVEMASTER:
	case CBUS_BOARD_WAVESTAR:
    case CBUS_BOARD_WAVESMIT:
		// OPNAマスク解除
		// bit 1: YM2608(OPNA)マスク設定   : 0=non-mask / 1=OPNA masked
		// bit 0: YM2608(OPNA)拡張部分機能 : 0=OPN / 1=OPNA
		d = cbus_read8(slot, 0xa460);
		cbus_write8(slot, 0xa460, (d&0xfc)|0x1);

		// volume control
		board86_control(board, 0, 0);
		board86_control(board, 1, 0);
		board86_control(board, 2, 0);
		board86_control(board, 3, 0);
		board86_control(board, 4, 0);

		ym2608_init(&board->chip[0]);
		break;
		
	case CBUS_BOARD_118:
   		//opl3_init(&board->chip[1]);
		break;

	case CBUS_BOARD_SID98:
		mos6581_init(&board->chip[0]);
		mos6581_init(&board->chip[1]);
		mos6581_init(&board->chip[2]);
		mos6581_init(&board->chip[3]);
		break;

	default:
		break;
	}
}

// ---------------------------------------
// NOTE: ウエイト値確定前のチェック時用: ウエイトたっぷり
static void chip_write(int bidx, uint16_t aport, uint16_t dport, uint8_t addr, uint8_t data)
{
	cbus_write8(bidx, aport, addr); // I/O
    CyDelayUs(250);
	cbus_write8(bidx, dport, data); // I/O=IN, SSG DISABLE
    CyDelayUs(250);
}
static uint8_t chip_read(int bidx, uint16_t aport, uint16_t dport, uint8_t addr)
{
	uint8_t x;
	cbus_write8(bidx, aport, addr);
    CyDelayUs(250);
	x = cbus_read8(bidx, dport);
    CyDelayUs(250);
	return x;
}

static uint8_t check_have26func(int idx, uint16_t base)
{
	// is 26K ?? ---------------------
	// SSG FineTune にテストパタンを読み書きしてみて、レジスタが存在してたら26Kと判定。
	chip_write(idx, base+0, base+2, 0x7, 0x7f); // I/O=IN, SSG DISABLE

	chip_write(idx, base+0, base+2, 0x00, 0xa5 );  // CH.A FineTune, pattern=0xa5
	chip_write(idx, base+0, base+2, 0x02, 0x5a );  // CH.B FineTune, pattern=0x5a
	chip_write(idx, base+0, base+2, 0x04, 0x26 );  // CH.C FineTune, pattern=0x26

	if (0xa5 != chip_read(idx, base+0, base+2, 0x00)) // CH.A FineTune
		return 0;
	if (0x5a != chip_read(idx, base+0, base+2, 0x02)) // CH.B FineTune
		return 0;
	if (0x26 != chip_read(idx, base+0, base+2, 0x04)) // CH.C FineTune
		return 0;

	return 1;
}


// ボード自動検出
static uint32_t auto_detect(int idx)
{
	uint32_t type = CBUS_BOARD_UNKNOWN;
	uint16_t base=0x88;

	for( int i=0; i<4; i++, base+=0x100){
		if(has_lowbit_decoder_board && locked_base_addr[i])
			continue;
		
		// is 26K ?? ---------------------
		if(!check_have26func(idx, base))
			continue;

		// is 12bit decoder?
        // bit[15:12]を変えながら（下位12bitは同じ）SSGレジスタを読み書きしてみて、
        // レジスタが存在してたらアドレスデコーダが12bitしか無いと判定する
		uint8_t lowbit_decoder_board = 1;
		for(int i=0; i<16; i++){
			if(!check_have26func(idx, base+(i<<12))){
				lowbit_decoder_board = 0;
				break;
			}
		}
		if (lowbit_decoder_board){
			locked_base_addr[i] = 1;
			has_lowbit_decoder_board = 1;
		}

		// is YM2608 / YMF288?? -----------------------
		// SpeakBoardとか0xa460非サポートな奴用
		// NOTE: SpeakBoardはアドレスを12bitしかデコードしていないようだ。

		// YMF288 mode select
		chip_write(idx, base+0, base+2, 0x20, 0x2);  // bit0:STANDBY, bit1:YMF288 mode

		// 0xff OPNA device-id
		uint16_t chiptype = chip_read(idx, base+0, base+2, 0xff);

		if (0x01==chiptype){ // YM2608
			switch(base){
			case 0x088:  type = CBUS_BOARD_ASB01_0088H; break;
			case 0x188:  type = CBUS_BOARD_ASB01_0188H; break;
			case 0x288:  type = CBUS_BOARD_ASB01_0288H; break;
			case 0x388:  type = CBUS_BOARD_ASB01_0388H; break;
			}
		}else if (0x02==chiptype){ // YMF288
			switch(base){
			case 0x088:  type = CBUS_BOARD_UNKNOWN; break;// そんなの無いはず
			case 0x188:  type = CBUS_BOARD_SXM_F; break;
			case 0x288:  type = CBUS_BOARD_WAVESTAR_0288H; break;// そんなの無いはず. A460判定で取られるはずだけど一応。
			case 0x388:  type = CBUS_BOARD_UNKNOWN; break;// そんなの無いはず
			}
		}else{ // 2203
			switch(base){
			case 0x088:  type = CBUS_BOARD_LITTLE_ORCHESTRA_0088H; break;
			case 0x188:  type = CBUS_BOARD_26; break;
			case 0x288:  type = CBUS_BOARD_UNKNOWN; break;// そんなの無いはず
			case 0x388:  type = CBUS_BOARD_UNKNOWN; break;// そんなの無いはず
			}
		}
		
		// ここでADPCM-RAM判定したいが、
		// ボード情報設定前なので boards が使えない。

		// なんか見つかったらスキャン終了
		if(type!=CBUS_BOARD_UNKNOWN)
			break;
	}
	return type;
}

// ------------------------------------------
// 初期化
void cbus_board_setup(void)
{
	uint8_t d=0;

	has_lowbit_decoder_board = 0;
	for (int i=0; i<4; i++){
		locked_base_addr[i] = 0;
	}

	// Cバスボード種別判定
	for (int i=0; i<NMAXBOARDS; i++){
		CBUS_BOARD_INFO *board = &boards[i];
		memset(board, 0, sizeof(CBUS_BOARD_INFO));
		board->slot = i;
		
		uint32_t preferred_type = conf_get_board_type(i);
		uint32_t type = preferred_type;

        // for DEBUG!!!!!!!!!!!!!!
//        if(i==0){
//            preferred_type = CBUS_BOARD_SB16;
//            type = preferred_type;
//        }

		
		// 自動判定
		if (preferred_type == CBUS_BOARD_AUTO){
			// read sound-id
			uint8_t sid = cbus_read8(i, 0xA460);
			switch(sid>>4){
			case SOUND_86_018X:
				type = CBUS_BOARD_86_0188H;
				break;
			case SOUND_86_028X:
				type = CBUS_BOARD_86_0288H;
				break;
			case SOUND_73_018X:
				type = CBUS_BOARD_73_0188H;
				break;
			case SOUND_73_028X:
				type = CBUS_BOARD_73_0288H;
				break;
			case SOUND_118:
		        type = CBUS_BOARD_118;
				break;
			default:
				type = auto_detect(i);
				break;
			}
		}

		board->boardtype = type;

		switch(type){
		// ---------------------------------
		// YM2203 only
		case CBUS_BOARD_26:
		case CBUS_BOARD_MULTIMEDIA_ORCHESTRA:
		case CBUS_BOARD_LITTLE_ORCHESTRA_0188H:
		case CBUS_BOARD_LITTLE_ORCHESTRA_0088H:
		case CBUS_BOARD_LITTLE_ORCHESTRA_L_0188H:
		case CBUS_BOARD_LITTLE_ORCHESTRA_L_0088H:
		case CBUS_BOARD_LITTLE_ORCHESTRA_RS_0188H:
		case CBUS_BOARD_LITTLE_ORCHESTRA_RS_0088H:
		case CBUS_BOARD_LITTLE_ORCHESTRA_LS_0188H:
		case CBUS_BOARD_LITTLE_ORCHESTRA_LS_0088H:
		case CBUS_BOARD_LITTLE_ORCHESTRA_SS:
		case CBUS_BOARD_LITTLE_ORCHESTRA_MATE:
		case CBUS_BOARD_LITTLE_ORCHESTRA_FELLOW:
		case CBUS_BOARD_JOY2:
		case CBUS_BOARD_SOUND_GRANPRI:
		case CBUS_BOARD_TN_F3FM:
			board->nchips = 1;
			board->chip[0].slot = i;
			board->chip[0].chiptype = CHIP_YM2203;
			if(type==CBUS_BOARD_LITTLE_ORCHESTRA_0088H || type==CBUS_BOARD_LITTLE_ORCHESTRA_L_0088H ||
			   type==CBUS_BOARD_LITTLE_ORCHESTRA_RS_0088H || type==CBUS_BOARD_LITTLE_ORCHESTRA_LS_0088H ){
				board->chip[0].areg_addr[0] = 0x088;
				board->chip[0].areg_addr[1] = 0x088;
				board->chip[0].dreg_addr[0] = 0x08a;
				board->chip[0].dreg_addr[1] = 0x08a;
			}else{
				board->chip[0].areg_addr[0] = 0x188;
				board->chip[0].areg_addr[1] = 0x188;
				board->chip[0].dreg_addr[0] = 0x18a;
				board->chip[0].dreg_addr[1] = 0x18a;
			}
			// FIXME: たぶんウエイト違うけど、資料が無い。
			board->chip[0].waitidx = &waitidx_2608[0];
			board->chip[0].waitdef = &waitdef_2203_MC4[0];
			board->chip[0].writefunc = ym2203_write;
			board->control_write = 0;
			
			// FIXME: 初期化
			//ym2608_init(&board->chip[0]);
			break;

		// ---------------------------------
		// YM2203 + YM3812
		case CBUS_BOARD_SOUND_ORCHESTRA_0188H:
		case CBUS_BOARD_SOUND_ORCHESTRA_0088H:
		case CBUS_BOARD_SOUND_ORCHESTRA_L_0188H:
		case CBUS_BOARD_SOUND_ORCHESTRA_L_0088H:
			board->nchips = 1;
			board->control_write = 0;
			board->chip[0].slot = i;
			board->chip[0].chiptype = CHIP_YM2203;
			if (type==CBUS_BOARD_SOUND_ORCHESTRA_0088H||type==CBUS_BOARD_SOUND_ORCHESTRA_L_0088H){
				board->chip[0].areg_addr[0] = 0x088;
				board->chip[0].dreg_addr[0] = 0x08a;
			}else{
				board->chip[0].areg_addr[0] = 0x188;
				board->chip[0].dreg_addr[0] = 0x18a;
			}
			// FIXME: たぶんウエイト違うけど、資料が無い。
			board->chip[0].waitidx = &waitidx_2608[0];
			board->chip[0].waitdef = &waitdef_2203_MC4[0];
			board->chip[0].writefunc = ym2203_write;
			
			board->nchips = 2;
			board->chip[1].slot = i;
			board->chip[1].chiptype = CHIP_YM3812;
			if (type==CBUS_BOARD_SOUND_ORCHESTRA_0088H||type==CBUS_BOARD_SOUND_ORCHESTRA_L_0088H){
				board->chip[1].areg_addr[0] = 0x08c;
				board->chip[1].dreg_addr[0] = 0x08e;
			}else{
				board->chip[1].areg_addr[0] = 0x18c;
				board->chip[1].dreg_addr[0] = 0x18e;
			}
			// FIXME: ウエイト違う.
			board->chip[1].waitidx = &waitidx_2608[0];
			board->chip[1].waitdef = &waitdef_2203_MC4[0];
			board->chip[1].writefunc = ym2203_write;
			break;
			
		// ---------------------------------
		// YM2203 + Y8950
		case CBUS_BOARD_SOUND_ORCHESTRA_V_0188H:
		case CBUS_BOARD_SOUND_ORCHESTRA_V_0088H:
		case CBUS_BOARD_SOUND_ORCHESTRA_VS_0188H:
		case CBUS_BOARD_SOUND_ORCHESTRA_VS_0088H:
		case CBUS_BOARD_SOUND_ORCHESTRA_LS_0188H:
		case CBUS_BOARD_SOUND_ORCHESTRA_LS_0088H:
		case CBUS_BOARD_SOUND_ORCHESTRA_MATE:
			board->nchips = 1;
			board->control_write = 0;
			board->chip[0].slot = i;
			board->chip[0].chiptype = CHIP_YM2203;
			if( type==CBUS_BOARD_SOUND_ORCHESTRA_V_0088H ||
				type==CBUS_BOARD_SOUND_ORCHESTRA_VS_0088H ||
				type==CBUS_BOARD_SOUND_ORCHESTRA_LS_0088H ){
				board->chip[0].areg_addr[0] = 0x088;
				board->chip[0].dreg_addr[0] = 0x08a;
			}else{
				board->chip[0].areg_addr[0] = 0x188;
				board->chip[0].dreg_addr[0] = 0x18a;
			}
			// FIXME: たぶんウエイト違うけど、資料が無い。
			board->chip[0].waitidx = &waitidx_2608[0];
			board->chip[0].waitdef = &waitdef_2203_MC4[0];
			board->chip[0].writefunc = ym2203_write;
			
			board->nchips = 2;
			board->chip[1].slot = i;
			board->chip[1].chiptype = CHIP_Y8950ADPCM;
			if( type==CBUS_BOARD_SOUND_ORCHESTRA_VS_0088H || type==CBUS_BOARD_SOUND_ORCHESTRA_LS_0088H ){
				board->chip[1].areg_addr[0] = 0x08c;
				board->chip[1].dreg_addr[0] = 0x08e;
			}else{
				board->chip[1].areg_addr[0] = 0x18c;
				board->chip[1].dreg_addr[0] = 0x18e;
			}
			// FIXME: ウエイト違う.
			board->chip[1].waitidx = &waitidx_2608[0];
			board->chip[1].waitdef = &waitdef_2203_MC4[0];
			board->chip[1].writefunc = ym2203_write;
			break;
			
		// ---------------------------------
		// YM2608 0xA460有り
		case CBUS_BOARD_73_0188H:
		case CBUS_BOARD_73_0288H:
			board->nchips = 1;
			board->chip[0].slot = i;
			board->chip[0].chiptype = CHIP_YM2608NOADPCM;
			if (type == CBUS_BOARD_73_0288H){
				board->chip[0].areg_addr[0] = 0x288;
				board->chip[0].areg_addr[1] = 0x28c;
				board->chip[0].dreg_addr[0] = 0x28a;
				board->chip[0].dreg_addr[1] = 0x28e;
			}else{
				board->chip[0].areg_addr[0] = 0x188;
				board->chip[0].areg_addr[1] = 0x18c;
				board->chip[0].dreg_addr[0] = 0x18a;
				board->chip[0].dreg_addr[1] = 0x18e;
			}
			board->chip[0].waitidx = &waitidx_2608[0];
			board->chip[0].waitdef = &waitdef_2608[0];
			board->chip[0].writefunc = ym2608_write;
			board->control_write = 0;

			// OPNAマスク解除
			d = cbus_read8(i, 0xa460);
			cbus_write8(i, 0xa460, (d&0xfc)|0x1);
			// 初期化
			ym2608_init(&board->chip[0]);
			// ADPCM-RAM搭載判定
			if( ym2608_check_adpcm(&board->chip[0]) ){
				board->chip[0].chiptype = CHIP_YM2608;
			}
			break;

		// ---------------------------------
		// YM2608 0xA460無し
		case CBUS_BOARD_ASB01_0188H:
		case CBUS_BOARD_ASB01_0088H:
		case CBUS_BOARD_ASB01_0288H:
		case CBUS_BOARD_ASB01_0388H:
		case CBUS_BOARD_SOUNDPLAYER98:
			board->nchips = 1;
			board->chip[0].slot = i;
			board->chip[0].chiptype = CHIP_YM2608NOADPCM;

			switch(type){
			case CBUS_BOARD_ASB01_0188H:
			case CBUS_BOARD_SOUNDPLAYER98:
				board->chip[0].areg_addr[0] = 0x188;
				board->chip[0].areg_addr[1] = 0x18c;
				board->chip[0].dreg_addr[0] = 0x18a;
				board->chip[0].dreg_addr[1] = 0x18e;
				break;
			case CBUS_BOARD_ASB01_0088H:
				board->chip[0].areg_addr[0] = 0x088;
				board->chip[0].areg_addr[1] = 0x08c;
				board->chip[0].dreg_addr[0] = 0x08a;
				board->chip[0].dreg_addr[1] = 0x08e;
				break;
			case CBUS_BOARD_ASB01_0288H:
				board->chip[0].areg_addr[0] = 0x288;
				board->chip[0].areg_addr[1] = 0x28c;
				board->chip[0].dreg_addr[0] = 0x28a;
				board->chip[0].dreg_addr[1] = 0x28e;
				break;
			case CBUS_BOARD_ASB01_0388H:
				board->chip[0].areg_addr[0] = 0x388;
				board->chip[0].areg_addr[1] = 0x38c;
				board->chip[0].dreg_addr[0] = 0x38a;
				board->chip[0].dreg_addr[1] = 0x38e;
				break;
			}
			
			board->chip[0].waitidx = &waitidx_2608[0];
			board->chip[0].waitdef = &waitdef_2608[0];
			board->chip[0].writefunc = ym2608_write;
			board->control_write = board86_control;

			// OPNA初期化
			ym2608_init(&board->chip[0]);
			// ADPCM-RAM搭載判定
			if( ym2608_check_adpcm(&board->chip[0]) ){
				if (preferred_type == CBUS_BOARD_AUTO){
					board->boardtype = CBUS_BOARD_SPEAKBOARD;
				}
				board->chip[0].chiptype = CHIP_YM2608;
			}
			break;
			
		// ---------------------------------
		// YM2608 0xA460無し ADPCM-RAM有り
		case CBUS_BOARD_SPEAKBOARD:
			board->nchips = 1;
			board->chip[0].slot = i;
			board->chip[0].chiptype = CHIP_YM2608;
			board->chip[0].areg_addr[0] = 0x188;
			board->chip[0].areg_addr[1] = 0x18c;
			board->chip[0].dreg_addr[0] = 0x18a;
			board->chip[0].dreg_addr[1] = 0x18e;
			board->chip[0].waitidx = &waitidx_2608[0];
			board->chip[0].waitdef = &waitdef_2608[0];
			board->chip[0].writefunc = ym2608_write;
			board->control_write = board86_control;

			// OPNA初期化
			ym2608_init(&board->chip[0]);
			break;
		
		case CBUS_BOARD_OTOMI:
			board->nchips = 1;
			board->control_write = 0;
			board->chip[0].slot = i;
			board->chip[0].chiptype = CHIP_YM2608;
			board->chip[0].areg_addr[0] = 0x188;
			board->chip[0].areg_addr[1] = 0x18c;
			board->chip[0].dreg_addr[0] = 0x18a;
			board->chip[0].dreg_addr[1] = 0x18e;
			board->chip[0].waitidx = &waitidx_2608[0];
			board->chip[0].waitdef = &waitdef_2608[0];
			board->chip[0].writefunc = ym2608_write;

			// OPNA初期化
			ym2608_init(&board->chip[0]);

			board->nchips = 2;
			board->chip[1].slot = i;
			board->chip[1].chiptype = CHIP_YM3438;
			board->chip[1].areg_addr[0] = 0x588;
			board->chip[1].areg_addr[1] = 0x58c;
			board->chip[1].dreg_addr[0] = 0x58a;
			board->chip[1].dreg_addr[1] = 0x58e;
			// FIXME: ウエイト違う.
			board->chip[1].waitidx = &waitidx_2608[0];
			board->chip[1].waitdef = &waitdef_2203_MC4[0];
			board->chip[1].writefunc = ym2203_write;
			break;
			

		// ---------------------------------
		// YM2608 0xA460有り
		case CBUS_BOARD_86_0188H:
		case CBUS_BOARD_86_0288H:
		case CBUS_BOARD_WAVEMASTER_0188H:
		case CBUS_BOARD_WAVEMASTER_0288H:
			board->nchips = 1;
			board->control_write = board86_control;
			board->chip[0].slot = i;
			board->chip[0].chiptype = CHIP_YM2608NOADPCM;
			
			if (type==CBUS_BOARD_86_0288H || type==CBUS_BOARD_WAVEMASTER_0288H){
				board->chip[0].areg_addr[0] = 0x288;
				board->chip[0].areg_addr[1] = 0x28c;
				board->chip[0].dreg_addr[0] = 0x28a;
				board->chip[0].dreg_addr[1] = 0x28e;
			}else{
				board->chip[0].areg_addr[0] = 0x188;
				board->chip[0].areg_addr[1] = 0x18c;
				board->chip[0].dreg_addr[0] = 0x18a;
				board->chip[0].dreg_addr[1] = 0x18e;
			}
			board->chip[0].waitidx = &waitidx_2608[0];
			board->chip[0].waitdef = &waitdef_2608[0];
			board->chip[0].writefunc = ym2608_write;

			// OPNAマスク解除
			// bit 1: YM2608(OPNA)マスク設定   : 0=non-mask / 1=OPNA masked
			// bit 0: YM2608(OPNA)拡張部分機能 : 0=OPN / 1=OPNA
			d = cbus_read8(i, 0xa460);
			cbus_write8(i, 0xa460, (d&0xfc)|0x1);
			// OPNA初期化
			ym2608_init(&board->chip[0]);
			// ADPCM-RAM搭載判定
			if( ym2608_check_adpcm(&board->chip[0]) ){
				board->chip[0].chiptype = CHIP_YM2608;
			}
			break;

		// YMF288 0xA460有り
		case CBUS_BOARD_WAVESMIT_0188H:
		case CBUS_BOARD_WAVESMIT_0288H:
		case CBUS_BOARD_WAVESTAR_0188H:
		case CBUS_BOARD_WAVESTAR_0288H:
			board->nchips = 1;
			board->control_write = board86_control;
			board->chip[0].slot = i;
			board->chip[0].chiptype = CHIP_YMF288;
			
			if (type==CBUS_BOARD_WAVESMIT_0288H || type==CBUS_BOARD_WAVESTAR_0288H){
				board->chip[0].areg_addr[0] = 0x288;
				board->chip[0].areg_addr[1] = 0x28c;
				board->chip[0].dreg_addr[0] = 0x28a;
				board->chip[0].dreg_addr[1] = 0x28e;
			}else{
				board->chip[0].areg_addr[0] = 0x188;
				board->chip[0].areg_addr[1] = 0x18c;
				board->chip[0].dreg_addr[0] = 0x18a;
				board->chip[0].dreg_addr[1] = 0x18e;
			}
			board->chip[0].waitidx = &waitidx_2608[0];
			board->chip[0].waitdef = &waitdef_2608[0];
			board->chip[0].writefunc = ym2608_write;

			// OPNAマスク解除
			// bit 1: YM2608(OPNA)マスク設定   : 0=non-mask / 1=OPNA masked
			// bit 0: YM2608(OPNA)拡張部分機能 : 0=OPN / 1=OPNA
			d = cbus_read8(i, 0xa460);
			cbus_write8(i, 0xa460, (d&0xfc)|0x1);

			// FIXME:OPNAとして初期化
			ym2608_init(&board->chip[0]);
			break;

		// YMF288 0xA460無し
		case CBUS_BOARD_WSN_A4F:
		case CBUS_BOARD_SXM_F:
		case CBUS_BOARD_SRN_F:
			board->nchips = 1;
			board->control_write = 0; // FIXME!
			board->chip[0].slot = i;
			board->chip[0].chiptype = CHIP_YMF288;
			
			board->chip[0].areg_addr[0] = 0x188;
			board->chip[0].areg_addr[1] = 0x18c;
			board->chip[0].dreg_addr[0] = 0x18a;
			board->chip[0].dreg_addr[1] = 0x18e;
			board->chip[0].waitidx = &waitidx_2608[0];
			board->chip[0].waitdef = &waitdef_2608[0];
			board->chip[0].writefunc = ym2608_write;

			// FIXME:OPNAとして初期化
			ym2608_init(&board->chip[0]);
			break;

		// ---------------------------------
		case CBUS_BOARD_118:
			// 初期化(init118のぱくり)
			board118_init(i);

			// NOTE: OPN3モードにする方法がわからん・・・。
			// change to OPNA-mode
			d = cbus_read8(i, 0xa460); // SoundID
			//cbus_write8(i, 0xa460, (d&0xfc)|0x1); // OPNAマスク解除
			// OPL-modeの場合:
			cbus_write8(i, 0xa460, (d&0xfc)|0x3); // OPNAマスク
#if 0
			for(int y=0; y<100; y++){ // MIDIチェック
				cbus_write8(i, 0x148E, 0x00);
				uint8_t status118 = cbus_read8(i, 0x148E);
				if ((status118&0x08) == 0x00)
					type = CBUS_BOARD_118;
			}
#endif
#if 0
			board->nchips = 2;
			board->chip[0].slot = i;
			// OPNA-mode --------------
			board->chip[0].chiptype = CHIP_YMF297_OPNA;
			board->chip[0].areg_addr[0] = 0x188;
			board->chip[0].areg_addr[1] = 0x18c;
			board->chip[0].dreg_addr[0] = 0x18a;
			board->chip[0].dreg_addr[1] = 0x18e;
			// FIXME: たぶんウエイト違うけど、資料が無い。
			board->chip[0].waitidx = &waitidx_2608[0];
			board->chip[0].waitdef = &waitdef_2608[0];
			board->chip[0].writefunc = ym2608_write;
			// FIXME:
			board->control_write = 0;

			// OPL3-mode -------------
			board->chip[1].slot = i;
			board->chip[1].chiptype = CHIP_YMF297_OPL3;
			board->chip[1].areg_addr[0] = 0x1488;
			board->chip[1].dreg_addr[0] = 0x1489;
			board->chip[1].areg_addr[1] = 0x148a;
			board->chip[1].dreg_addr[1] = 0x148b;
			// ウエイト値固定・テーブル無し（タイマは必要）
			board->chip[1].waitidx = 0;
			board->chip[1].waitdef = 0;
			board->chip[1].writefunc = ymf297_opl3_write;
			// FIXME:
			board->control_write = 0;
       		opl3_init(&board->chip[1]);
#else
			board->nchips = 1;
			board->chip[0].slot = i;
			// OPL3-mode -------------
			board->chip[0].slot = i;
			board->chip[0].chiptype = CHIP_YMF297_OPL3;
			board->chip[0].areg_addr[0] = 0x1488;
			board->chip[0].dreg_addr[0] = 0x1489;
			board->chip[0].areg_addr[1] = 0x148a;
			board->chip[0].dreg_addr[1] = 0x148b;
			// ウエイト値固定・テーブル無し（タイマは必要）
			board->chip[0].waitidx = 0;
			board->chip[0].waitdef = 0;
			board->chip[0].writefunc = ymf297_opl3_write;
			// FIXME:
			board->control_write = 0;
       		opl3_init(&board->chip[0]);
#endif
            
            //CyDelay(2000);
			break;

		// YMF262 --------------------------------------
		// いろいろ未検証
		case CBUS_BOARD_POWERWINDOW_T64S:
		case CBUS_BOARD_PCSB2:
		case CBUS_BOARD_WGS98S:
		case CBUS_BOARD_SRB_G:
		case CBUS_BOARD_MIDI_ORCHESTRA_MIDI3:
			board->nchips = 1;
			board->chip[1].slot = i;
			board->chip[1].chiptype = CHIP_YMF262;
			{
				//FIXME: IOポート未検証 ->SB16と一緒かなぁ？？？？
				uint32_t base = 0xc8d2;//0x20d2;
				board->chip[1].areg_addr[0] = base + 0;
				board->chip[1].dreg_addr[0] = base + 1;
				board->chip[1].areg_addr[1] = base + 2;
				board->chip[1].dreg_addr[1] = base + 3;
			}
			// FIXME: ウエイト違う.
			board->chip[1].waitidx = &waitidx_2608[0];
			board->chip[1].waitdef = &waitdef_2608[0];
			board->chip[1].writefunc = ym2608_write;
			break;

		// YMF262 + YM2203 --------------------------------------
		case CBUS_BOARD_SB16:
		case CBUS_BOARD_SB16VALUE:
		case CBUS_BOARD_SB16_2203_0188H:
		case CBUS_BOARD_SB16_2203_0088H:
			board->nchips = 1;
			board->chip[0].slot = i;
			board->chip[0].chiptype = CHIP_YMF262;
            // 要検証:
            //[2016/09/19 21:39:26] RuRuRu: SB16は設定しだいですよねー。hootでは0x24d2~2cd2
			board->chip[0].areg_addr[0] = 0xC8D2;
			board->chip[0].dreg_addr[0] = 0xC9D2;
			board->chip[0].areg_addr[1] = 0xCAD2;
			board->chip[0].dreg_addr[1] = 0xCBD2;
//			board->chip[0].areg_addr[0] = 0x24D2;
//			board->chip[0].dreg_addr[0] = 0x25D2;
//			board->chip[0].areg_addr[1] = 0x26D2;
//			board->chip[0].dreg_addr[1] = 0x27D2;
			// FIXME: ウエイト違う.
			board->chip[0].waitidx = 0;//&waitidx_2608[0];
			board->chip[0].waitdef = 0;//&waitdef_2203_MC4[0];
			board->chip[0].writefunc = ymf262_write;
			
    		// have 2203? ---------------------
            if(type!=CBUS_BOARD_SB16VALUE){
                uint16_t base = 0;
    			if (type==CBUS_BOARD_SB16_2203_0088H) base = 0x88;
    			else if (type==CBUS_BOARD_SB16_2203_0188H) base = 0x188;
    	    	else if (check_have26func(i, 0x088)) base = 0x88;
    	    	else if (check_have26func(i, 0x188)) base = 0x188;
                
    	    	if (base){
        			board->nchips = 2;
        			board->chip[1].slot = i;
        			board->chip[1].chiptype = CHIP_YM2203;
       				board->chip[1].areg_addr[0] = base;
       				board->chip[1].dreg_addr[0] = base + 4;
       				board->chip[1].areg_addr[1] = base + 2;
       				board->chip[1].dreg_addr[1] = base + 6;
        			// FIXME: ウエイト違う.
        			board->chip[1].waitidx = &waitidx_2608[0];
    	    		board->chip[1].waitdef = &waitdef_2203_MC4[0];
    		    	board->chip[1].writefunc = ym2203_write;

                    locked_base_addr[base==0x088?0:1] = 1;
        			has_lowbit_decoder_board = 1;
                }
            }
       		opl3_init(&board->chip[0]);
            break;

        // 14board(TMS3631-RI104) -----------------------------------
        case CBUS_BOARD_14:
            board->nchips = 1;
            board->boardtype = CBUS_BOARD_14;
			board->control_write = 0;
			board->control_read = 0;
            board->chip[0].chiptype = CHIP_TMS3631RI104;
			board->chip[0].areg_addr[0] = 0;
			board->chip[0].dreg_addr[0] = 0;
			board->chip[0].areg_addr[1] = 0;
			board->chip[0].dreg_addr[1] = 0;
			board->chip[0].waitidx = 0;
			board->chip[0].waitdef = 0;
			board->chip[0].writefunc = board14_write;
			board->control_write = 0;
            
            board14_init(&board->chip[0]);
            break;
            
		// SID98 ----------------------------------------------------
		// 俺ボード
		case CBUS_BOARD_SID98:
			board->nchips = 4;
			board->boardtype = CBUS_BOARD_SID98;
			board->control_write = 0;
			board->control_read = 0;
			for (int k=0; k<4; k++) {
				uint32_t base = 0x788;
				board->chip[k].writefunc = mos6581_write;
				board->chip[k].slot = i;
				board->chip[k].chiptype = CHIP_MOS6581;
				board->chip[k].dreg_addr[0] = base + (k*2);
				board->chip[k].areg_addr[0] = base + (k*2) + 1;
				// ウエイト不要
				board->chip[k].waitidx = 0;
				board->chip[k].waitdef = 0;
			}
			
			break;

        case CBUS_BOARD_MPU98:
        case CBUS_BOARD_MPU98II:
			board->nchips = 1;
			//board->boardtype = type;
			board->mpu_data_port = 0xe0d0;
			board->mpu_stat_port = 0xe0d2;

			mpu_set_uart_mode(board);
			break;
            
		case CBUS_BOARD_UNKNOWN:
		default:
			board->boardtype = CBUS_BOARD_UNKNOWN;
			board->nchips = 0;
			break;
		}

		cbus_board_init(i);
	}
}

void write_chip(uint8_t exaddr, uint8_t addr, uint8_t data)
{
	uint8_t slot = exaddr>>4;
	uint8_t chipidx = (exaddr>>1)&0x07;
	uint8_t ex = exaddr&0x01;

	if (slot>=NMAXBOARDS)
		return;
	if (chipidx>=boards[slot].nchips)
		return;

	CHIP_INFO *chip = &(boards[slot].chip[chipidx]);
	chip->writefunc(chip, ex, addr, data);
}

void board86_control(CBUS_BOARD_INFO *board, uint8_t idx, uint8_t data)
{
	//000b= VOL0(FM音源直接出力レベル)
	//001b= VOL1(FM音源間接出力レベル)
	//010b= VOL2(CD-DA(MULTiのみ),LINE直接出力レベル)
	//011b= VOL3(CD-DA(MULTiのみ),LINE間接出力レベル)
	//101b= VOL4(PCM直接出力レベル)
	uint8_t vol_idx[5] = { 0, 1, 2, 3, 5 };
	
	if (4<idx) return;
	if (15<data) data = 15;
	
	cbus_write8(board->slot, 0xa466, vol_idx[idx]<<5 | data);
}

uint8_t cbus_has_midi(uint8_t slot)
{
	if (slot>=NMAXBOARDS)
		return 0;
	
	CBUS_BOARD_INFO *board = &boards[slot];
	return board->mpu_data_port != 0;
}

void cbus_midi_write(uint8_t slot, uint8_t msg)
{
	if (slot>=NMAXBOARDS)
		return;

    CBUS_BOARD_INFO *board = &boards[slot];	
    if (board->mpu_data_port == 0)
        return;
	mpu_write_data(board, msg);
}


uint8_t cbus_midi_read(uint8_t slot, uint8_t *msg)
{
	if (slot>=NMAXBOARDS)
		return 0;
	
	CBUS_BOARD_INFO *board = &boards[slot];
    if (board->mpu_data_port == 0)
        return 0;
	if (!mpu_dsr(board))
		return 0;
	
	*msg = mpu_read_data(board);
	return 1;
}
