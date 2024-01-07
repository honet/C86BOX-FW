#ifndef CBUS_BOARD_H__
#define CBUS_BOARD_H__

#include <stdint.h>
#include "cbus_boardtype.h"
#include "sound_chip.h"
    
#define NMAXCHIPS  4
#define NMAXBOARDS 2

typedef struct tag_CBUS_BOARD_INFO{
	uint8_t slot;
	CBUS_BOARD_TYPE boardtype;
	uint8_t nchips;
	CHIP_INFO chip[NMAXCHIPS];
	void (*control_write)(struct tag_CBUS_BOARD_INFO *board, uint8_t idx, uint8_t data);
	uint16_t (*control_read)(struct tag_CBUS_BOARD_INFO *board, uint8_t idx);
    uint32_t mpu_data_port;
    uint32_t mpu_stat_port;
} CBUS_BOARD_INFO;


uint32_t cbus_get_board_type(uint8_t slot);
uint32_t cbus_get_nchips(uint8_t slot);
uint32_t cbus_get_chip_type(uint8_t slot, uint8_t chipidx);
void cbus_board_control_write(uint8_t slot, uint16_t idx, uint16_t data);
uint16_t cbus_board_control_read(uint8_t slot, uint16_t idx);

void cbus_board_init(uint8_t slot);
void cbus_board_setup(void);
void write_chip(uint8_t exaddr, uint8_t addr, uint8_t data);

uint8_t cbus_has_midi(uint8_t slot);
void cbus_midi_write(uint8_t slot, uint8_t msg);
uint8_t cbus_midi_read(uint8_t slot, uint8_t *msg);

#endif

