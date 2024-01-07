#include <stdint.h>
#include <project.h>

#include "cbus.h"
#include "cbus_board.h"
#include "cbus_mpu.h"
#include "tick.h"

#define DSR 0x80  // DATA SET READY
#define DRR 0x40  // DATA READ READY

#define TIMEOUT 8000000 //1sec (1tick = 8MHz = 125ns)

/* Checks if there is a byte waiting to be read from the MPU. 
   Returns 0 if so. Returns non-zero if not. */
uint8_t mpu_dsr(CBUS_BOARD_INFO *board)
{
    /* Return state of DATA SET READY. MPU clears this line when it
       has a byte waiting to be read from its DATA port. */
    return cbus_read8(board->slot, board->mpu_stat_port) & DSR;

}

/* Checks if it's OK to write a byte to the MPU's COMMAND or DATA ports.
   Returns 0 if so. Returns non-zero if not. */
static uint8_t mpu_drr(CBUS_BOARD_INFO *board)
{
    /* Return state of DATA READ READY. MPU clears this line when it's
       OK for us to write to the MPU's ports. */
    return cbus_read8(board->slot, board->mpu_stat_port) & DRR;
}


uint32_t tickSpan(uint32_t t1, uint32_t t2){
    if (t1<=t2) return t2-t1;
    else return t1 + (UINT_LEAST32_MAX - t2);
}

// MPUのデータ準備が完了するまで待つ
// 0: 待機完了, 1: タイムアウト
uint8_t mpu_wait_dsr(CBUS_BOARD_INFO *board)
{
	uint32_t st = TickCounter_ReadCounter();
	while (mpu_dsr(board)) {
		uint32_t t = TickCounter_ReadCounter();
        if (TIMEOUT<tickSpan(st, t)) return 1;
	}
	return 0;
}

// MPUのデータ読込準備が完了するまで待つ
// 0: 待機完了, 1: タイムアウト
uint8_t mpu_wait_drr(CBUS_BOARD_INFO *board)
{
	uint32_t st = TickCounter_ReadCounter();
	while (mpu_drr(board)) {
		uint32_t t = TickCounter_ReadCounter();
        if (TIMEOUT<tickSpan(st, t)) return 1;
	}
	return 0;
}


// MPUデータポート読取り
uint8_t mpu_read_data(CBUS_BOARD_INFO *board)
{
    return cbus_read8(board->slot, board->mpu_data_port);
}


// MPUデータポート書き込み
void mpu_write_data(CBUS_BOARD_INFO *board, uint8_t data)
{
	mpu_wait_drr(board);

	cbus_write8(board->slot, board->mpu_data_port, data);
}

// MPUコマンドポート書き込み
void mpu_write_command(CBUS_BOARD_INFO *board, uint8_t cmd)
{
	mpu_wait_drr(board);

	cbus_write8(board->slot, board->mpu_stat_port, cmd);
}

// MPUをUARTモードに設定する
void mpu_set_uart_mode(CBUS_BOARD_INFO *board)
{
	// Send FF(reset) command to the MPU.
	mpu_write_command(board, 0xff);

	do {
		if (mpu_wait_dsr(board))
			return; // timeout

	} while (mpu_read_data(board) != 0xFE); // ACK待ち

	// Send 3F(UART mode) command to the MPU.
	mpu_write_command(board, 0x3f);
}


//Here's an example of reading input:
//unsigned char data;
//
//     /* Is there another byte waiting to be read? */
//     while(!is_input())
//     {
//       /* Yes there is. Read it. */
//       data = get_mpu_in();
//
//       /* OK. We got the byte. Normally, we'd store it somewhere. But,
//       I'm just going to throw it away because this is a do-nothing
//       tutorial. */
//     }
//Here's an example of writing a byte:
//    /* Is it OK to write the MPU ports? */
//    while(is_output());
//
//    /* Write the byte. Just for illustration, I'll output a MIDI Clock */
//    put_mpu_out(0xF8);
 

/* [] END OF FILE */
