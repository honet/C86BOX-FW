/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#ifndef CBUS_BOARD14_H__
#define CBUS_BOARD14_H__
    
#include <stdint.h>
#include "sound_chip.h"
    
void board14_write(CHIP_INFO *chip, uint8_t exaddr, uint8_t addr, uint8_t data);
void board14_init(CHIP_INFO *chip);

#endif
/* [] END OF FILE */
