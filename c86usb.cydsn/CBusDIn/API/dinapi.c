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


#include "`$INSTANCE_NAME`_dinapi.h"

void `$INSTANCE_NAME`_Start(void)
{
	// 特にすること無し
}

void `$INSTANCE_NAME`_Stop(void)
{
	// 特にすること無し
}

uint8_t `$INSTANCE_NAME`_Read8()
{
    //uint8_t intr = CyEnterCriticalSection();
	// バスアクセス待ち
	while ((`$INSTANCE_NAME`_STATUS_REG & `$INSTANCE_NAME`_STATUSBIT_BUS_BUSY) != 0u) {}
    
	// リード指示送る
	`$INSTANCE_NAME`_CMD_FIFO_REG = `$INSTANCE_NAME`_COMMAND_BYTE;
#if 0
    uint8_t s = `$INSTANCE_NAME`_STATUS_REG;
    uint32_t count = 0;
	while (( s & `$INSTANCE_NAME`_STATUSBIT_DATA_READY) == 0) {
		/* wait until input data are valid */
        s = `$INSTANCE_NAME`_STATUS_REG;
        if(++count>65535){
            // BUG: 本来ココにはたどり着かないハズなんだけど、どーにもバグが取れないので暫定措置。
            return 0;
        }
	}
#else
	while ((`$INSTANCE_NAME`_STATUS_REG & `$INSTANCE_NAME`_STATUSBIT_DATA_READY) == 0) {
		/* wait until input data are valid */
	}
#endif

    //CyExitCriticalSection(intr);
    return CY_GET_REG16(`$INSTANCE_NAME`_DATA_FIFO_PTR);
}

// 注意： unalignedアクセスした場合の結果は不定。
uint16_t `$INSTANCE_NAME`_Read16()
{
    //uint8_t intr = CyEnterCriticalSection();
	// バスアクセス待ち
	while ((`$INSTANCE_NAME`_STATUS_REG & `$INSTANCE_NAME`_STATUSBIT_BUS_BUSY) != 0u) {
	}
	// リード指示送る
	`$INSTANCE_NAME`_CMD_FIFO_REG = `$INSTANCE_NAME`_COMMAND_WORD;

	while ((`$INSTANCE_NAME`_STATUS_REG & `$INSTANCE_NAME`_STATUSBIT_DATA_READY) == 0) {
		/* wait until input data are valid */
	}

    //CyExitCriticalSection(intr);
    return CY_GET_REG16(`$INSTANCE_NAME`_DATA_FIFO_PTR);
}

void `$INSTANCE_NAME`_BusyWait()
{
	// バスアクセス待ち
	while((`$INSTANCE_NAME`_STATUS_REG & `$INSTANCE_NAME`_STATUSBIT_BUS_BUSY) != 0u) {
	}
}
/* [] END OF FILE */
