#include <project.h>
#include <stdint.h>
#include "cbus.h"
#include "sound_chip.h"
#include "tick.h"

uint8_t tms3631regs[8] = {0};
#define TMS3631_CE 0x80
#define TMS3631_WCLK 0x40

// --------------------------------------------------------------------------
// exaddr = [7:4]slot,[3:1]chip,[0:0]ex
void board14_write(CHIP_INFO *chip, uint8_t exaddr __unused, uint8_t addr, uint8_t data)
{
	uint8_t slot = chip->slot;
    
    switch(addr){
        case 0x00: case 0x01: case 0x02: case 0x03:
        case 0x04: case 0x05: case 0x06:
            tms3631regs[addr] = data & 0x3f;
            break;
        
         case 0x07:
            tms3631regs[7] = data & 0x3f;
            // enable CE
    		cbus_write8(slot, 0x008c, TMS3631_CE);
            chip->next_tick = GetTick() + 24; // about 3.0us
            TickWait(chip->next_tick);
            
            for(int i=0; i<8; i++){
                uint8_t val = tms3631regs[i];
        	    // data-set
    			cbus_write8(slot, 0x008c, TMS3631_CE|val);
                chip->next_tick = GetTick() + 32; // about 4.0us
                TickWait(chip->next_tick);
                
                // clk HI
    			cbus_write8(slot, 0x008c, TMS3631_CE|TMS3631_WCLK|val);
                chip->next_tick = GetTick() + 48; // about 6.0us
                TickWait(chip->next_tick); // wait 6us

                // clk LO
    			cbus_write8(slot, 0x008c, TMS3631_CE|val);
                chip->next_tick = GetTick() + 48; // about 6.0us
                TickWait(chip->next_tick);
            }
            
            // wait 8us
            chip->next_tick = GetTick() + 64; // about 8.0us
            TickWait(chip->next_tick);
            // disable CE
    		cbus_write8(slot, 0x008c, 0);
            break;
            
        case 0x10: // 0088: Envelope R
            cbus_write8(slot, 0x0088, data);
            break;
        case 0x11: // 008A: Envelope L
            cbus_write8(slot, 0x008a, data);
            break;
        case 0x12: // 0188: CH Mask
            cbus_write8(slot, 0x0188, data);
            break;
            
        case 0x20: // 018C: TIM
            cbus_write8(slot, 0x018c, data);
            break;
        case 0x21: // 018E: TIM #2
            cbus_write8(slot, 0x018e, data);
            break;
    }
}

void board14_init(CHIP_INFO *chip)
{
    // PIO Control Reg
	uint8_t slot = chip->slot;
    cbus_write8(slot, 0x008e, 0x80);
}

/* [] END OF FILE */
