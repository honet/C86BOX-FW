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
#if !defined(CBUSDIN_`$INSTANCE_NAME`_H)
#define CBUSDIN_`$INSTANCE_NAME`_H

#include "cytypes.h"
#include "cyfitter.h"
#include "CyLib.h"

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#ifndef CY_PSOC5LP
    #error Component `$CY_COMPONENT_NAME` requires cy_boot v3.0 or later
#endif /* CY_PSOCLP */


/***************************************
*        Function Prototypes            
***************************************/

void    `$INSTANCE_NAME`_Start(void);
void    `$INSTANCE_NAME`_Stop(void);
uint8_t `$INSTANCE_NAME`_Read8();
uint16_t `$INSTANCE_NAME`_Read16();
void     `$INSTANCE_NAME`_BusyWait();


/***************************************
*           API Constants               
***************************************/

// STATUS_REG のフラグ
#define `$INSTANCE_NAME`_STATUSBIT_BUS_BUSY          (0x01u)
#define `$INSTANCE_NAME`_STATUSBIT_DATA_READY        (0x02u)

// CMD_FIFO_REG のフラグ
#define `$INSTANCE_NAME`_COMMAND_BYTE             (0x02u)
#define `$INSTANCE_NAME`_COMMAND_WORD             (0x00u)

#define `$INSTANCE_NAME`_COMMAND_ADDR_ODD         (0x01u)
#define `$INSTANCE_NAME`_COMMAND_ADDR_EVEN        (0x00u)


/***************************************
*             Registers                 
***************************************/

#define `$INSTANCE_NAME`_STATUS_REG            (* (reg8 *)  `$INSTANCE_NAME`_StsReg__STATUS_REG)
#define `$INSTANCE_NAME`_STATUS_PTR            (  (reg8 *)  `$INSTANCE_NAME`_StsReg__STATUS_REG)

// コマンドレジスタ
#define `$INSTANCE_NAME`_CMD_FIFO_REG          (* (reg8 *)  `$INSTANCE_NAME`_Lsb__F0_REG)
#define `$INSTANCE_NAME`_CMD_FIFO_PTR          (  (reg8 *)  `$INSTANCE_NAME`_Lsb__F0_REG)

// データレジスタ(16bit幅)
#define `$INSTANCE_NAME`_DATA_FIFO_PTR         (  (reg16 *) `$INSTANCE_NAME`_Lsb__16BIT_F1_REG)

// パルス幅設定レジスタ
#define `$INSTANCE_NAME`_LO_PULSE_REG          (* (reg8 *)  `$INSTANCE_NAME`_Lsb__D0_REG)
#define `$INSTANCE_NAME`_LO_PULSE_PTR          (  (reg8 *)  `$INSTANCE_NAME`_Lsb__D0_REG)
#define `$INSTANCE_NAME`_HI_PULSE_REG          (* (reg8 *)  `$INSTANCE_NAME`_Lsb__D1_REG)
#define `$INSTANCE_NAME`_HI_PULSE_PTR          (  (reg8 *)  `$INSTANCE_NAME`_Lsb__D1_REG)


#endif /* CBUSCTRL_`$INSTANCE_NAME`_H */


/* [] END OF FILE */
