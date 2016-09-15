
#ifndef CBUS_H__
#define CBUS_H__

#include <stdint.h>

void cbus_reset(void);
void cbus_write8(uint8_t slot, uint32_t addr, uint8_t data);
void cbus_write16(uint8_t slot, uint32_t addr, uint16_t data);
uint8_t cbus_read8(uint8_t slot, uint32_t addr);
uint16_t cbus_read16(uint8_t slot, uint32_t addr);


#endif

