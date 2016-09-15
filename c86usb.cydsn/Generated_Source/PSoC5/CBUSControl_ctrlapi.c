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

#include "CBUSControl_ctrlapi.h"

void CBUSControl_Start(void)
{
	// 特にすること無し
}

void CBUSControl_Stop(void)
{
	// 特にすること無し
}

void CBUSControl_Write8(uint8_t isodd, uint16_t data)
{
    uint8_t intr = CyEnterCriticalSection();
	// バスアクセス待ち
	while((CBUSControl_STATUS_REG & CBUSControl_STATUSBIT_CMD_FIFO_FULL) != 0u) {
	}
	// ライト指示送る
	CBUSControl_CMD_FIFO_REG = CBUSControl_COMMAND_BYTE | CBUSControl_COMMAND_WRITE | /*ODD,EVEN*/(isodd&0x01);
	
	// FIFOにデータ詰める
	CY_SET_REG16(CBUSControl_DATA_FIFO_PTR, data);
    CyExitCriticalSection(intr);
}

// 注意： unalignedアクセスした場合の結果は不定。
void CBUSControl_Write16(uint16_t data)
{
    uint8_t intr = CyEnterCriticalSection();
	// バスアクセス待ち
	while((CBUSControl_STATUS_REG & CBUSControl_STATUSBIT_CMD_FIFO_FULL) != 0u) {
	}
	// ライト指示送る
	CBUSControl_CMD_FIFO_REG = CBUSControl_COMMAND_WORD | CBUSControl_COMMAND_WRITE;
	
	// FIFOにデータ詰める
	CY_SET_REG16(CBUSControl_DATA_FIFO_PTR, data);
    CyExitCriticalSection(intr);
}


uint8_t CBUSControl_Read8(uint8_t isodd)
{
    uint8_t intr = CyEnterCriticalSection();
	// バスアクセス待ち
#if 1
	volatile uint8_t lasts = CBUSControl_STATUS_REG;
	while ((lasts & CBUSControl_STATUSBIT_CMD_FIFO_FULL) != 0u) {
	}
#else
	while ((CBUSControl_STATUS_REG & CBUSControl_STATUSBIT_CMD_FIFO_FULL) != 0u) {
	}
#endif
    
	// リード指示送る
	CBUSControl_CMD_FIFO_REG = CBUSControl_COMMAND_BYTE | CBUSControl_COMMAND_READ | /*ODD,EVEN*/(isodd&0x01);

#if 1
	volatile uint8_t s = CBUSControl_STATUS_REG;
	while ((s & CBUSControl_STATUSBIT_DATA_VALID) == 0) {
		/* wait until input data are valid */
		s = s>>3;
		s = CBUSControl_STATUS_REG;
	}
#else
	while ((CBUSControl_STATUS_REG & CBUSControl_STATUSBIT_DATA_VALID) == 0) {
		/* wait until input data are valid */
	}
#endif

	return (CBUSControl_DIN_LSB_DATA_REG);
    CyExitCriticalSection(intr);
}

// 注意： unalignedアクセスした場合の結果は不定。
uint16_t CBUSControl_Read16()
{
    uint8_t intr = CyEnterCriticalSection();
	// バスアクセス待ち
	while ((CBUSControl_STATUS_REG & CBUSControl_STATUSBIT_CMD_FIFO_FULL) != 0u) {
	}
	// リード指示送る
	CBUSControl_CMD_FIFO_REG = CBUSControl_COMMAND_WORD | CBUSControl_COMMAND_READ;

	while ((CBUSControl_STATUS_REG & CBUSControl_STATUSBIT_DATA_VALID) == 0) {
		/* wait until input data are valid */
	}

	return ((((uint16) CBUSControl_DIN_MSB_DATA_REG) << 8u) | CBUSControl_DIN_LSB_DATA_REG);
    CyExitCriticalSection(intr);
}

void CBUSControl_BusyWait()
{
	// バスアクセス待ち
	while((CBUSControl_STATUS_REG & CBUSControl_STATUSBIT_BUS_BUSY) != 0u) {
	}
}

/* [] END OF FILE */
