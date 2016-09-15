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
#if !defined(CBUSCTRL_CBUSControl_H)
#define CBUSCTRL_CBUSControl_H

#include "cytypes.h"
#include "cyfitter.h"
#include "CyLib.h"

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#ifndef CY_PSOC5LP
    #error Component CBUS_CTRL requires cy_boot v3.0 or later
#endif /* CY_PSOCLP */


/***************************************
*        Function Prototypes            
***************************************/

void    CBUSControl_Start(void);
void    CBUSControl_Stop(void);
void     CBUSControl_Write8(uint8_t isodd, uint16_t data);
void     CBUSControl_Write16(uint16_t data);
uint8_t CBUSControl_Read8(uint8_t isodd);
uint16_t CBUSControl_Read16();
void     CBUSControl_BusyWait();


/***************************************
*           API Constants               
***************************************/

// STATUS_REG のフラグ
#define CBUSControl_STATUSBIT_BUS_BUSY          (0x01u)
#define CBUSControl_STATUSBIT_DATA_VALID        (0x02u)
#define CBUSControl_STATUSBIT_CMD_FIFO_FULL     (0x04u)


// CMD_FIFO_REG のフラグ
#define CBUSControl_COMMAND_READ             (0x02u)
#define CBUSControl_COMMAND_WRITE            (0x00u)

#define CBUSControl_COMMAND_BYTE             (0x04u)
#define CBUSControl_COMMAND_WORD             (0x00u)

#define CBUSControl_COMMAND_ADDR_ODD         (0x01u)
#define CBUSControl_COMMAND_ADDR_EVEN        (0x00u)


/***************************************
*             Registers                 
***************************************/

#define CBUSControl_STATUS_REG            (* (reg8 *)  CBUSControl_StsReg__STATUS_REG)
#define CBUSControl_STATUS_PTR            (  (reg8 *)  CBUSControl_StsReg__STATUS_REG)


// コマンドレジスタ
#define CBUSControl_CMD_FIFO_REG          (* (reg8 *)  CBUSControl_Lsb__F0_REG)
#define CBUSControl_CMD_FIFO_PTR          (  (reg8 *)  CBUSControl_Lsb__F0_REG)

// データレジスタ(16bit幅)
#define CBUSControl_DATA_FIFO_PTR         (  (reg16 *) CBUSControl_Lsb__F1_REG)

// リード動作のデータ保持レジスタ
#define CBUSControl_DIN_LSB_DATA_REG      (* (reg8 *)  CBUSControl_LsbReg__STATUS_REG)
#define CBUSControl_DIN_LSB_DATA_PTR      (  (reg8 *)  CBUSControl_LsbReg__STATUS_REG)
#define CBUSControl_DIN_MSB_DATA_REG      (* (reg8 *)  CBUSControl_MsbReg__STATUS_REG)
#define CBUSControl_DIN_MSB_DATA_PTR      (  (reg8 *)  CBUSControl_MsbReg__STATUS_REG)

// パルス幅設定レジスタ
#define CBUSControl_LO_PULSE_REG          (* (reg8 *)  CBUSControl_Lsb__D0_REG)
#define CBUSControl_LO_PULSE_PTR          (  (reg8 *)  CBUSControl_Lsb__D0_REG)
#define CBUSControl_HI_PULSE_REG          (* (reg8 *)  CBUSControl_Lsb__D1_REG)
#define CBUSControl_HI_PULSE_PTR          (  (reg8 *)  CBUSControl_Lsb__D1_REG)


#endif /* CBUSCTRL_CBUSControl_H */

/* [] END OF FILE */
