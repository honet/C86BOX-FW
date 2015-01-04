#include <project.h>
#include "timer_if.h"
#include "cbus.h"
#include "cbus_board.h"
#include "sound_chip.h"
#include "eeprom_config.h"

struct tag_CBUS_BOARD_INFO;

typedef struct tag_CBUS_BOARD_INFO{
	uint8_t slot;
	CBUS_BOARD_TYPE boardtype;
	uint8_t nchips;
	CHIP_INFO chip[NMAXCHIPS];
	void (*control_write)(struct tag_CBUS_BOARD_INFO *board, uint8_t idx, uint16_t data);
	uint16_t (*control_read)(struct tag_CBUS_BOARD_INFO *board, uint16_t idx);
} CBUS_BOARD_INFO;

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

// ------------------------------------
// function declaration
void board86_control(CBUS_BOARD_INFO *board, uint8_t idx, uint16_t data);


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
	uint16_t d=0;
	
	if (slot>=NMAXBOARDS)
		return;

	CBUS_BOARD_INFO *board = &boards[slot];

	switch(board->boardtype){
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
		// OPNAマスク解除
		// bit 1: YM2608(OPNA)マスク設定   : 0=non-mask / 1=OPNA masked
		// bit 0: YM2608(OPNA)拡張部分機能 : 0=OPN / 1=OPNA
		d = cbus_read(slot, 0xa460);
		cbus_write(slot, 0xa460, (d&0xfc)|0x1);

		// volume control
		board86_control(board, 0, 0);
		board86_control(board, 1, 0);
		board86_control(board, 2, 0);
		board86_control(board, 3, 0);
		board86_control(board, 4, 0);

		ym2608_init(&board->chip[0]);
		break;
		
	case CBUS_BOARD_118:
	default:
		break;
	}
}

// ---------------------------------------
// NOTE: ウエイト値確定前のチェック時用
static void chip_write(int bidx, uint16_t aport, uint16_t dport, uint8_t addr, uint8_t data)
{
	cbus_write(bidx, aport, addr); // I/O
    CyDelayUs(250);
	cbus_write(bidx, dport, data); // I/O=IN, SSG DISABLE
    CyDelayUs(250);
}
static uint8_t chip_read(int bidx, uint16_t aport, uint16_t dport, uint8_t addr)
{
	uint16_t x;
	cbus_write(bidx, aport, addr);
    CyDelayUs(250);
	x = cbus_read(bidx, dport);
    CyDelayUs(250);
	return x&0xff;
}

// ボード自動検出
static uint32_t auto_detect(int idx)
{
	uint32_t type = CBUS_BOARD_UNKNOWN;

	// is 26K ?? ---------------------
	// SSG FineTune にテストパタンを読み書きしてみて、レジスタが存在してたら26Kと判定。
	chip_write(idx, 0x188, 0x18a, 0x7, 0x7f); // I/O=IN, SSG DISABLE

	chip_write(idx, 0x188, 0x18a, 0x00, 0xa5 );  // CH.A FineTune, pattern=0xa5
	chip_write(idx, 0x188, 0x18a, 0x02, 0x5a );  // CH.B FineTune, pattern=0x5a
	chip_write(idx, 0x188, 0x18a, 0x04, 0x26 );  // CH.C FineTune, pattern=0x26

	if (0xa5 != chip_read(idx, 0x0188, 0x018a, 0x00)) // CH.A FineTune
		return type;
	if (0x5a != chip_read(idx, 0x188, 0x18a, 0x02)) // CH.B FineTune
		return type;
	if (0x26 != chip_read(idx, 0x188, 0x18a, 0x04)) // CH.C FineTune
		return type;

	type = CBUS_BOARD_26;

	// is YM2608 / YMF288?? -----------------------
	// SpeakBoardとか0xa460非サポートな奴用
    // NOTE: SpeakBoardはアドレスを12bitしかデコードしていないようだ。

	// 0xff OPNA device-id
	uint16_t chiptype = chip_read(idx, 0x188, 0x18a, 0xff);
	
	if (0x01==chiptype) // YM2608
		return CBUS_BOARD_ASB01;
	else if (0x02==chiptype) // YMF288
		return CBUS_BOARD_SXM_F;
	
	// ここでADPCM-RAM判定したいが、
	// ボード情報設定前なので boards が使えない。
	
	return CBUS_BOARD_26;
}

// ------------------------------------------
// 初期化
void cbus_board_setup(void)
{
	int timeridx = 0;
	uint16_t d=0;

	// Cバスボード種別判定
	for (int i=0; i<NMAXBOARDS; i++){
		CBUS_BOARD_INFO *board = &boards[i];
		memset(board, 0, sizeof(CBUS_BOARD_INFO));
		board->slot = i;
		
		uint32_t preferred_type = conf_get_board_type(i);
		uint32_t type = preferred_type;
		
		// 自動判定
		if (preferred_type == CBUS_BOARD_AUTO){
			// read sound-id
			uint16_t sid = cbus_read(i, 0xA460);
			switch((sid&0xff)>>4){
			case SOUND_86_018X:
				type = CBUS_BOARD_86_0188H;
				break;
			case SOUND_86_028X:
				type = CBUS_BOARD_86_0288H;
				break;
			case SOUND_73_018X:
				type = CBUS_BOARD_73_0288H;
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
			board->chip[0].writefunc = ym2608_write;
			board->chip[0].wait_timerif = &timerRes[timeridx++];
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
			board->chip[0].writefunc = ym2608_write;
			board->chip[0].wait_timerif = &timerRes[timeridx++];
			
			if(timeridx>=NTIMERS)
				break;
			
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
			board->chip[1].writefunc = ym2608_write;
			board->chip[1].wait_timerif = &timerRes[timeridx++];
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
			board->chip[0].writefunc = ym2608_write;
			board->chip[0].wait_timerif = &timerRes[timeridx++];
			
			if(timeridx>=NTIMERS)
				break;
			
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
			board->chip[1].writefunc = ym2608_write;
			board->chip[1].wait_timerif = &timerRes[timeridx++];
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
			board->chip[0].wait_timerif = &timerRes[timeridx++];
			board->control_write = 0;

			// OPNAマスク解除
			d = cbus_read(i, 0xa460);
			cbus_write(i, 0xa460, (d&0xfc)|0x1);
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
			board->chip[0].wait_timerif = &timerRes[timeridx++];
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
			board->chip[0].wait_timerif = &timerRes[timeridx++];
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
			board->chip[0].wait_timerif = &timerRes[timeridx++];

			// OPNA初期化
			ym2608_init(&board->chip[0]);

			if(timeridx>=NTIMERS)
				break;
			
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
			board->chip[1].writefunc = ym2608_write;
			board->chip[1].wait_timerif = &timerRes[timeridx++];
			
			break;
			

		// ---------------------------------
		// YM2608 0xA460有り
		case CBUS_BOARD_86_0188H:
		case CBUS_BOARD_86_0288H:
		case CBUS_BOARD_WAVEMASTER_0188H:
		case CBUS_BOARD_WAVEMASTER_0288H:
		case CBUS_BOARD_WAVESMIT_0188H:
		case CBUS_BOARD_WAVESMIT_0288H:
		case CBUS_BOARD_WAVESTAR_0188H:
		case CBUS_BOARD_WAVESTAR_0288H:
			board->nchips = 1;
			board->control_write = board86_control;
			board->chip[0].slot = i;
			board->chip[0].chiptype = CHIP_YM2608NOADPCM;
			
			if (type==CBUS_BOARD_86_0288H || type==CBUS_BOARD_WAVEMASTER_0288H ||
				type==CBUS_BOARD_WAVESMIT_0288H || type==CBUS_BOARD_WAVESTAR_0288H){
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
			board->chip[0].wait_timerif = &timerRes[timeridx++];

			// OPNAマスク解除
			// bit 1: YM2608(OPNA)マスク設定   : 0=non-mask / 1=OPNA masked
			// bit 0: YM2608(OPNA)拡張部分機能 : 0=OPN / 1=OPNA
			d = cbus_read(i, 0xa460);
			cbus_write(i, 0xa460, (d&0xfc)|0x1);
			// OPNA初期化
			ym2608_init(&board->chip[0]);
			// ADPCM-RAM搭載判定
			if( ym2608_check_adpcm(&board->chip[0]) ){
				board->chip[0].chiptype = CHIP_YM2608;
			}
			break;

		// ---------------------------------
		case CBUS_BOARD_118:
			board->nchips = 1;
			board->chip[0].slot = i;
			// TODO: とりあえずOPNA側のみ対応。 OPL側どうすればいいかわからん。
			board->chip[0].chiptype = CHIP_YMF297;
			board->chip[0].areg_addr[0] = 0x188;
			board->chip[0].areg_addr[1] = 0x18c;
			board->chip[0].dreg_addr[0] = 0x18a;
			board->chip[0].dreg_addr[1] = 0x18e;
			// FIXME: たぶんウエイト違うけど、資料が無い。
			board->chip[0].waitidx = &waitidx_2608[0];
			board->chip[0].waitdef = &waitdef_2608[0];
			board->chip[0].writefunc = ym2608_write;
			// FIXME:
			board->chip[0].wait_timerif = &timerRes[timeridx++];
			board->control_write = 0;
			break;

		case CBUS_BOARD_UNKNOWN:
		default:
			board->boardtype = CBUS_BOARD_UNKNOWN;
			board->nchips = 0;
			break;
		}

		cbus_board_init(i);

		// ウェイトタイマーが不足したらそこで止める
		if(timeridx>=NTIMERS)
			break;
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

void board86_control(CBUS_BOARD_INFO *board, uint8_t idx, uint16_t data)
{
	//000b= VOL0(FM音源直接出力レベル)
	//001b= VOL1(FM音源間接出力レベル)
	//010b= VOL2(CD-DA(MULTiのみ),LINE直接出力レベル)
	//011b= VOL3(CD-DA(MULTiのみ),LINE間接出力レベル)
	//101b= VOL4(PCM直接出力レベル)
	uint8_t vol_idx[5] = { 0, 1, 2, 3, 5 };
	
	if (4<idx) return;
	if (15<data) data = 15;
	
	cbus_write(board->slot, 0xa466, vol_idx[idx]<<4 | data);
}

