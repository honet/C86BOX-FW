#ifndef CBUS_BOARD_H__
#define CBUS_BOARD_H__

#include <stdint.h>

#define NMAXCHIPS  4
#define NMAXBOARDS 2

typedef enum {
	CBUS_BOARD_UNKNOWN					= 0x0,
	CBUS_BOARD_14						= 0x00001,	// (未対応) NEC PC-9801-14
	CBUS_BOARD_26						= 0x00002,	// NEC PC-9801-26, 26K
	CBUS_BOARD_SOUND_ORCHESTRA			= 0x00012,	// (未テスト) SNE Sound Orchestra : YM2203, YM3812
	CBUS_BOARD_SOUND_ORCHESTRA_L		= 0x00022,	// (未テスト) SNE Sound Orchestra L : YM2203, YM3812
	CBUS_BOARD_SOUND_ORCHESTRA_V		= 0x00032,	// (未テスト) SNE Sound Orchestra V : YM2203, Y8950(w/ADPCM-RAM)
	CBUS_BOARD_SOUND_ORCHESTRA_VS		= 0x00042,	// (未テスト) SNE Sound Orchestra VS : YM2203, Y8950(w/ADPCM-RAM)
	CBUS_BOARD_SOUND_ORCHESTRA_LS		= 0x00052,	// (未テスト) SNE Sound Orchestra LS : YM2203, Y8950
	CBUS_BOARD_SOUND_ORCHESTRA_MATE		= 0x00062,	// (未テスト) SNE Sound Orchestra MATE : YM2203, Y8950
	CBUS_BOARD_MULTIMEDIA_ORCHESTRA		= 0x00072,	// (未テスト) SNE Multimedia Orchestra : YM2203, YM262M
	CBUS_BOARD_LITTLE_ORCHESTRA			= 0x00082,	// (未テスト) SNE Littele Orchestra : YM2203
	CBUS_BOARD_LITTLE_ORCHESTRA_L		= 0x00092,	// (未テスト) SNE Littele Orchestra L : YM2203
	CBUS_BOARD_LITTLE_ORCHESTRA_RS		= 0x000a2,	// (未テスト) SNE Littele Orchestra RS : YM2203
	CBUS_BOARD_LITTLE_ORCHESTRA_LS		= 0x000b2,	// (未テスト) SNE Littele Orchestra LS : YM2203
	CBUS_BOARD_LITTLE_ORCHESTRA_SS		= 0x000c2,	// (未テスト) SNE Littele Orchestra SS : YM2203
	CBUS_BOARD_LITTLE_ORCHESTRA_MATE	= 0x000d2,	// (未テスト) SNE Littele Orchestra MATE : YM2203
	CBUS_BOARD_LITTLE_ORCHESTRA_FELLOW	= 0x000e2,	// (未テスト) SNE Littele Orchestra FELLOW : YM2203
	CBUS_BOARD_JOY2						= 0x000f2,	// (未テスト) SNE JOY-2 : YM2203
	CBUS_BOARD_SOUND_GRANPRI			= 0x00102,	// (未テスト) SNE SOUND GRANPRI : YM2203
	CBUS_BOARD_TN_F3FM					= 0x00112,	// (未テスト) 東京ニーズ TN-F3FM : YM2203C
	
	CBUS_BOARD_73						= 0x00003,	// NEC PC-9801-73
	CBUS_BOARD_86						= 0x00023,	// NEC PC-9801-86
	
	CBUS_BOARD_ASB01					= 0x00043,	// (未テスト) SIS アミューズメントサウンドボードASB-01 : YM2608
	CBUS_BOARD_SPEAKBOARD				= 0x00053,	// (未テスト) アイドルジャパン SpeakBoard : YM2608(w/ADPCM-RAM)
	CBUS_BOARD_SOUNDPLAYER98			= 0x00063,	// (未テスト) コンピュータテクニカ SPB-98 : YM2608, YMF278
	
	CBUS_BOARD_SECONDBUS86				= 0x00073,	// (未対応) second-bus86 : YM2608, YMF278B-S
	CBUS_BOARD_SOUNDEDGE				= 0x00083,	// (未対応) sound-edge : YAMAHA Sound Edge SW20-98 : YM2608B, YMF278B
	CBUS_BOARD_WINDUO					= 0x00093,	// (未対応) win-duo : YM2608
	CBUS_BOARD_OTOMI					= 0x000a3,	// (未テスト) MAD FACTORY 音美 : YM2608(w/ADPCM-RAM), YM3438

	CBUS_BOARD_WAVEMASTER				= 0x000b3,	// Q-Vision WaveMaster(86互換) : YM2608
	CBUS_BOARD_WAVESMIT					= 0x000c3,	// Q-Vision WaveSMIT(86互換) : YMF288-S
	CBUS_BOARD_WAVESTAR					= 0x000d3,	// Q-Vision WaveStar(86互換) : YMF288-S
	CBUS_BOARD_WSN_A4F					= 0x000e3,	// (未対応) Buffalo WSN-A4F : YMF288-S

	CBUS_BOARD_SB16						= 0x00004,	// (未対応) sound-blaster 16 (CT2720) : YMF262-F
	CBUS_BOARD_SB16_2203				= 0x00014,	// (未対応) sound-blaster 16 with YM2203 (CT2720) : YMF262-F, YM2203
	CBUS_BOARD_SB16VALUE				= 0x00024,	// (未対応) sound-blaster 16Value (CT3720) : YMF262-F
	CBUS_BOARD_POWERWINDOW_T64S			= 0x00034,	// (未対応) canopus PowerWindow T64S : YMF262-M
	CBUS_BOARD_PCSB2					= 0x00044,	// (未対応) EPSON PCSB2 : YMF262-M
	CBUS_BOARD_WGS98S					= 0x00054,	// (未対応) コンピュータテクニカ : YMF262-M
	CBUS_BOARD_SRB_G					= 0x00064,	// (未対応) buffalo SRB-G : YMF264-F, YMZ263B-F
	CBUS_BOARD_MIDI_ORCHESTRA_MIDI3		= 0x00074,	// (未対応) SNE MIDI ORCHESTRA MIDI-3 : YM264F
		
	CBUS_BOARD_SB_AWE32					= 0x00005,	// (未対応) SoundBlaster AWE32 (CT3610) : YMF289B-S
	CBUS_BOARD_118						= 0x00006,	// NEC PC-9801-118 : YMF297-F
	
	CBUS_BOARD_AUTO = 0x40000000
} CBUS_BOARD_TYPE;


uint32_t cbus_get_board_type(uint8_t slot);
uint32_t cbus_get_nchips(uint8_t slot);
uint32_t cbus_get_chip_type(uint8_t slot, uint8_t chipidx);
void cbus_board_control_write(uint8_t slot, uint16_t idx, uint16_t data);
uint16_t cbus_board_control_read(uint8_t slot, uint16_t idx);

void cbus_board_init(uint8_t slot);
void cbus_board_setup(void);
void write_chip(uint8_t exaddr, uint8_t addr, uint8_t data);


#endif

