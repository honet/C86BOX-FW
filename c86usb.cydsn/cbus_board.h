#ifndef CBUS_BOARD_H__
#define CBUS_BOARD_H__

#include <stdint.h>
#include "cbus_boardtype.h"

#define NMAXCHIPS  4
#define NMAXBOARDS 2


uint32_t cbus_get_board_type(uint8_t slot);
uint32_t cbus_get_nchips(uint8_t slot);
uint32_t cbus_get_chip_type(uint8_t slot, uint8_t chipidx);
void cbus_board_control_write(uint8_t slot, uint16_t idx, uint16_t data);
uint16_t cbus_board_control_read(uint8_t slot, uint16_t idx);

void cbus_board_init(uint8_t slot);
void cbus_board_setup(void);
void write_chip(uint8_t exaddr, uint8_t addr, uint8_t data);


#endif

