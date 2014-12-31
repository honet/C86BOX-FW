#include <project.h>
#include <string.h>
#include "cbus.h"
#include "cbus_board.h"
#include "sound_chip.h"

#define MAGIC 0x0826860c //0x0c862608

// offset size desc
//      0    4 magic(0x0c862608)
//      4    4 version(0x01)
//      8    8 blank
//
//  16 +0    4 board-type[0]
//  16+ 4    4 board-type[1]
//  16+ 8    4 board-type[2]
//  16+12    4 board-type[3]

// little endian
static uint16_t read16(volatile uint8_t* p){
	return (p[1]<<8) | p[0];
}
static uint32_t read32(volatile uint8_t* p){
	return (p[3]<<24) | (p[2]<<16) | (p[1]<<8) | p[0];
}
static void write16(volatile uint8_t* dst, uint16_t v){
	dst[0] = v&0xff;
	dst[1] = (v>>8)&0xff;
}
static void write32(volatile uint8_t* dst, uint32_t v){
	dst[0] = v&0xff;
	dst[1] = (v>>8)&0xff;
	dst[2] = (v>>16)&0xff;
	dst[3] = (v>>24)&0xff;
}

static void conf_format()
{
	uint8_t buff[16];

	memset(buff,0,16);
	write32(buff, MAGIC);
	write32(buff+4, 0x01);
	EEPROM_Write(&buff[0], 0);
	
	memset(buff,0,16);
	write32(buff, CBUS_BOARD_AUTO);
	write32(buff+4, CBUS_BOARD_AUTO);
	write32(buff+8, CBUS_BOARD_AUTO);
	write32(buff+12, CBUS_BOARD_AUTO);
	EEPROM_Write(&buff[0], 1);
}

static void conf_check()
{
	volatile uint8_t *base = (volatile uint8_t*)CYDEV_EE_BASE;
	
	if ( read32(base) != MAGIC ){
		conf_format();
	}
}

void conf_set_board_type(uint8_t slot, uint32_t type)
{
	volatile uint8_t *base = (volatile uint8_t*)CYDEV_EE_BASE;
	uint8_t buff[16];
	
	if (slot>3) return;
	
	conf_check();

	memcpy(buff, base+16, 16);
	write32(buff+(slot<<2), type);
	EEPROM_Write(buff, 1);
}

uint32_t conf_get_board_type(uint8_t slot)
{
	volatile uint8_t *base = (volatile uint8_t*)CYDEV_EE_BASE;
	conf_check();

	return read32(base+16+(slot<<2));
}

