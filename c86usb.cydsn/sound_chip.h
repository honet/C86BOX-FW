#ifndef SOUND_CHIP_H__
#define SOUND_CHIP_H__
#include <stdint.h>
#include "timer_if.h"

// ------------------------------------------------------------------
// OPNA ADPCM registers
#define OPNA_ADPCM_CTRL1_REG		0x00
#define OPNA_ADPCM_CTRL2_REG		0x01
#define OPNA_ADPCM_STARTL_REG		0x02
#define OPNA_ADPCM_STARTH_REG		0x03
#define OPNA_ADPCM_STOPL_REG		0x04
#define OPNA_ADPCM_STOPH_REG		0x05
#define OPNA_ADPCM_PRESCALEL_REG	0x06
#define OPNA_ADPCM_PRESCALEH_REG	0x07
#define OPNA_ADPCM_DATA_REG			0x08
#define OPNA_ADPCM_DELTA_NL_REG		0x09
#define OPNA_ADPCM_DELTA_NH_REG		0x0a
#define OPNA_ADPCM_LEVEL_REG		0x0b
#define OPNA_ADPCM_LIMITL_REG		0x0c
#define OPNA_ADPCM_LIMITH_REG		0x0d
#define OPNA_ADPCM_DACDATA_REG		0x0e
#define OPNA_ADPCM_PCMDATA_REG		0x0f
#define OPNA_ADPCM_FLAG_REG			0x10

// OPNA ADPCM control register 1 : $00
#define OPNA_ADPCM_CTRL1_BIT_START			0x80
#define OPNA_ADPCM_CTRL1_BIT_REC			0x40
#define OPNA_ADPCM_CTRL1_BIT_MEMDATA		0x20
#define OPNA_ADPCM_CTRL1_BIT_REPEAT			0x10
#define OPNA_ADPCM_CTRL1_BIT_SPOFF			0x08
#define OPNA_ADPCM_CTRL1_BIT_RESET			0x01

// OPNA ADPCM control register 2 : $01
#define OPNA_ADPCM_CTRL2_BIT_LEFT			0x80
#define OPNA_ADPCM_CTRL2_BIT_RIGHT			0x40
#define OPNA_ADPCM_CTRL2_BIT_SAMPLE			0x08
#define OPNA_ADPCM_CTRL2_BIT_DAAD			0x04
#define OPNA_ADPCM_CTRL2_BIT_RAM_BUS8		0x02
#define OPNA_ADPCM_CTRL2_BIT_ROM			0x01

// OPNA ADPCM FLAG control register : $10
#define OPNA_ADPCM_FLAG_BIT_IRQRESET		0x80
#define OPNA_ADPCM_FLAG_BIT_MASK_ZERO		0x10
#define OPNA_ADPCM_FLAG_BIT_MASK_BRDY		0x08
#define OPNA_ADPCM_FLAG_BIT_MASK_EOS		0x04
#define OPNA_ADPCM_FLAG_BIT_MASK_TIMERB		0x02
#define OPNA_ADPCM_FLAG_BIT_MASK_TIMERA		0x01
#define OPNA_ADPCM_FLAG_BIT_MASK_ALL		0x1f

// OPNA status register 0/1.
#define OPNA_STATUS_BIT_BUSY		0x80
#define OPNA_STATUS_BIT_PCMBUSY		0x20
#define OPNA_STATUS_BIT_ZERO		0x10
#define OPNA_STATUS_BIT_BRDY		0x08
#define OPNA_STATUS_BIT_EOS			0x04
#define OPNA_STATUS_BIT_FLAGB		0x02
#define OPNA_STATUS_BIT_FLAGA		0x01


// ------------------------------------------------------------------
typedef enum {
	CHIP_UNKNOWN		= 0x0000,
	
	CHIP_OPNA			= 0x00001,
	CHIP_YM2608			= 0x00001,
	CHIP_YM2608NOADPCM	= 0x10001,
	CHIP_OPM			= 0x00002,
	CHIP_YM2151			= 0x00002,
	CHIP_OPN3L			= 0x00003,
	CHIP_YMF288			= 0x00003,
	CHIP_OPL3			= 0x00004,
	CHIP_YMF262			= 0x00004,
	CHIP_OPLL			= 0x00005,
	CHIP_YM2413			= 0x00005,
	
	CHIP_SN76489		= 0x00006,
	CHIP_SN76496		= 0x10006,
	CHIP_AY38910		= 0x00007,
	CHIP_YM2149			= 0x10007,
	CHIP_YM2203			= 0x00008,
	CHIP_YM2612			= 0x00009,
	CHIP_YM3526			= 0x0000a,
	CHIP_YM3812			= 0x0000b,
	CHIP_YMF271			= 0x0000c,
	CHIP_YMF278B		= 0x0000d,
	CHIP_YMZ280B		= 0x0000e,
	CHIP_YMF297			= 0x0000f,
	CHIP_YM2610B		= 0x00010,
	CHIP_Y8950			= 0x00020,
	CHIP_Y8950ADPCM 	= 0x10020,
	CHIP_YM3438			= 0x00021,
	
	CHIP_MOS6581		= 0x00022,
} CHIP_TYPE;

struct tag_CHIP_INFO;

typedef struct tag_CHIP_INFO{
	uint8_t slot;
	uint32_t chiptype;
	uint16_t areg_addr[2];
	uint16_t dreg_addr[2];
	const uint8_t *waitidx;
	const uint16_t *waitdef;
	const TIMER_INTERFACE *wait_timerif;
	void (*writefunc)(struct tag_CHIP_INFO *chip, uint8_t exaddr, uint8_t addr, uint8_t data);
} CHIP_INFO;

extern const uint16_t waitdef_2203_MC4[5];
extern const uint16_t waitdef_2608[5];
extern const uint8_t waitidx_2608[512];

void ym2203_write(CHIP_INFO *chip, uint8_t exaddr, uint8_t addr, uint8_t data);

void ym2608_write(CHIP_INFO *chip, uint8_t exaddr, uint8_t addr, uint8_t data);
uint8_t ym2608_read_status(CHIP_INFO *chip, uint8_t ex);
void ym2608_write_adpcm(CHIP_INFO *chip, uint16_t addr, uint8_t *data, uint16_t size);
void ym2608_read_adpcm(CHIP_INFO *chip, uint16_t addr, uint8_t *data, uint16_t size);
uint8_t ym2608_check_adpcm(CHIP_INFO *chip);
void ym2608_init(CHIP_INFO *chip);

void mos6581_write(CHIP_INFO *chip, uint8_t exaddr, uint8_t addr, uint8_t data);
void mos6581_init(CHIP_INFO *chip);

#endif


