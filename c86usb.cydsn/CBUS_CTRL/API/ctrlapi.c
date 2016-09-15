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

#include "`$INSTANCE_NAME`_ctrlapi.h"

void `$INSTANCE_NAME`_Start(void)
{
	// 特にすること無し
}

void `$INSTANCE_NAME`_Stop(void)
{
	// 特にすること無し
}

void `$INSTANCE_NAME`_Write8(uint8_t isodd, uint16_t data)
{
    uint8_t intr = CyEnterCriticalSection();
	// バスアクセス待ち
	while((`$INSTANCE_NAME`_STATUS_REG & `$INSTANCE_NAME`_STATUSBIT_CMD_FIFO_FULL) != 0u) {
	}
	// ライト指示送る
	`$INSTANCE_NAME`_CMD_FIFO_REG = `$INSTANCE_NAME`_COMMAND_BYTE | `$INSTANCE_NAME`_COMMAND_WRITE | /*ODD,EVEN*/(isodd&0x01);
	
	// FIFOにデータ詰める
	CY_SET_REG16(`$INSTANCE_NAME`_DATA_FIFO_PTR, data);
    CyExitCriticalSection(intr);
}

// 注意： unalignedアクセスした場合の結果は不定。
void `$INSTANCE_NAME`_Write16(uint16_t data)
{
    uint8_t intr = CyEnterCriticalSection();
	// バスアクセス待ち
	while((`$INSTANCE_NAME`_STATUS_REG & `$INSTANCE_NAME`_STATUSBIT_CMD_FIFO_FULL) != 0u) {
	}
	// ライト指示送る
	`$INSTANCE_NAME`_CMD_FIFO_REG = `$INSTANCE_NAME`_COMMAND_WORD | `$INSTANCE_NAME`_COMMAND_WRITE;
	
	// FIFOにデータ詰める
	CY_SET_REG16(`$INSTANCE_NAME`_DATA_FIFO_PTR, data);
    CyExitCriticalSection(intr);
}


uint8_t `$INSTANCE_NAME`_Read8(uint8_t isodd)
{
    uint8_t intr = CyEnterCriticalSection();
	// バスアクセス待ち
#if 1
	volatile uint8_t lasts = `$INSTANCE_NAME`_STATUS_REG;
	while ((lasts & `$INSTANCE_NAME`_STATUSBIT_CMD_FIFO_FULL) != 0u) {
	}
#else
	while ((`$INSTANCE_NAME`_STATUS_REG & `$INSTANCE_NAME`_STATUSBIT_CMD_FIFO_FULL) != 0u) {
	}
#endif
    
	// リード指示送る
	`$INSTANCE_NAME`_CMD_FIFO_REG = `$INSTANCE_NAME`_COMMAND_BYTE | `$INSTANCE_NAME`_COMMAND_READ | /*ODD,EVEN*/(isodd&0x01);

#if 1
	volatile uint8_t s = `$INSTANCE_NAME`_STATUS_REG;
	while ((s & `$INSTANCE_NAME`_STATUSBIT_DATA_VALID) == 0) {
		/* wait until input data are valid */
		s = s>>3;
		s = `$INSTANCE_NAME`_STATUS_REG;
	}
#else
	while ((`$INSTANCE_NAME`_STATUS_REG & `$INSTANCE_NAME`_STATUSBIT_DATA_VALID) == 0) {
		/* wait until input data are valid */
	}
#endif

	return (`$INSTANCE_NAME`_DIN_LSB_DATA_REG);
    CyExitCriticalSection(intr);
}

// 注意： unalignedアクセスした場合の結果は不定。
uint16_t `$INSTANCE_NAME`_Read16()
{
    uint8_t intr = CyEnterCriticalSection();
	// バスアクセス待ち
	while ((`$INSTANCE_NAME`_STATUS_REG & `$INSTANCE_NAME`_STATUSBIT_CMD_FIFO_FULL) != 0u) {
	}
	// リード指示送る
	`$INSTANCE_NAME`_CMD_FIFO_REG = `$INSTANCE_NAME`_COMMAND_WORD | `$INSTANCE_NAME`_COMMAND_READ;

	while ((`$INSTANCE_NAME`_STATUS_REG & `$INSTANCE_NAME`_STATUSBIT_DATA_VALID) == 0) {
		/* wait until input data are valid */
	}

	return ((((uint16) `$INSTANCE_NAME`_DIN_MSB_DATA_REG) << 8u) | `$INSTANCE_NAME`_DIN_LSB_DATA_REG);
    CyExitCriticalSection(intr);
}

void `$INSTANCE_NAME`_BusyWait()
{
	// バスアクセス待ち
	while((`$INSTANCE_NAME`_STATUS_REG & `$INSTANCE_NAME`_STATUSBIT_BUS_BUSY) != 0u) {
	}
}

/* [] END OF FILE */
