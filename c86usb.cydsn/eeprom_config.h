#ifndef EEPROM_CONFIG_H__
#define EEPROM_CONFIG_H__

#include <stdint.h>

void conf_set_board_type(uint8_t slot, uint32_t type);
uint32_t conf_get_board_type(uint8_t slot);

#endif

