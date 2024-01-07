#ifndef CBUS_MPU_H_
#define CBUS_MPU_H_

#include "cbus_board.h"

uint8_t mpu_dsr(CBUS_BOARD_INFO *board);
uint8_t mpu_wait_dsr(CBUS_BOARD_INFO *board);
uint8_t mpu_wait_drr(CBUS_BOARD_INFO *board);

uint8_t mpu_read_data(CBUS_BOARD_INFO *board);
void mpu_write_data(CBUS_BOARD_INFO *board, uint8_t data);
void mpu_write_command(CBUS_BOARD_INFO *board, uint8_t cmd);
void mpu_set_uart_mode(CBUS_BOARD_INFO *board);


#endif
/* [] END OF FILE */
