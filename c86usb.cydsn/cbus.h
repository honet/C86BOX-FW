
#ifndef CBUS_H__
#define CBUS_H__

void cbus_reset(void);
void cbus_write(uint8_t slot, uint16_t addr, uint16_t data);
uint16_t cbus_read(uint8_t slot, uint16_t addr);


#endif

