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
#include "`$INSTANCE_NAME`_doutapi.h"

void `$INSTANCE_NAME`_Start(void)
{
	// 特にすること無し
}

void `$INSTANCE_NAME`_Stop(void)
{
	// 特にすること無し
}

void `$INSTANCE_NAME`_Write8(uint8_t data)
{
    uint8_t intr = CyEnterCriticalSection();
	// バスアクセス待ち
	while((`$INSTANCE_NAME`_STATUS_REG & `$INSTANCE_NAME`_STATUSBIT_BUS_BUSY) != 0u) {
	}
	// FIFOにデータ詰める
	CY_SET_REG16(`$INSTANCE_NAME`_DATA_FIFO_PTR, data);
	// ライト指示送る
	`$INSTANCE_NAME`_CMD_FIFO_REG = `$INSTANCE_NAME`_COMMAND_BYTE;
	
    CyExitCriticalSection(intr);
}

// 注意： unalignedアクセスした場合の結果は不定。
void `$INSTANCE_NAME`_Write16(uint16_t data)
{
    uint8_t intr = CyEnterCriticalSection();
	// バスアクセス待ち
	while((`$INSTANCE_NAME`_STATUS_REG & `$INSTANCE_NAME`_STATUSBIT_BUS_BUSY) != 0u) {
	}
	// FIFOにデータ詰める
	CY_SET_REG16(`$INSTANCE_NAME`_DATA_FIFO_PTR, data);
	// ライト指示送る
	`$INSTANCE_NAME`_CMD_FIFO_REG = `$INSTANCE_NAME`_COMMAND_WORD;

    CyExitCriticalSection(intr);
}

void `$INSTANCE_NAME`_BusyWait()
{
	// バスアクセス待ち
    uint8_t s = `$INSTANCE_NAME`_STATUS_REG;
    uint32_t count = 0;
	while((s& `$INSTANCE_NAME`_STATUSBIT_BUS_BUSY) != 0u) {
        s = `$INSTANCE_NAME`_STATUS_REG;
        count++;
        if(count>65535){
            if( (s&0x47)==0x47){
            	CY_SET_REG16(`$INSTANCE_NAME`_DATA_FIFO_PTR, 0);
            }
        }
	}
}
/* [] END OF FILE */
