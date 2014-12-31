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
#if !defined(ChipWaitTimer1_api_H)
#define ChipWaitTimer1_api_H

#include "cytypes.h"
#include "cyfitter.h"
#include "CyLib.h"

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#ifndef CY_PSOC5LP
    #error Component ChipWaitTimer requires cy_boot v3.0 or later
#endif /* CY_PSOCLP */

/***************************************
*        Function Prototypes            
***************************************/

void ChipWaitTimer1_Start(void);
void ChipWaitTimer1_Stop(void);

void ChipWaitTimer1_Set(uint16_t count);
uint8_t ChipWaitTimer1_Query();
void ChipWaitTimer1_Wait();


/***************************************
*           API Constants               
***************************************/

// STATUS_REG のフラグ
#define ChipWaitTimer1_QUEUE_FULL           (0x01u)
#define ChipWaitTimer1_COUNTING             (0x02u)

/***************************************
*             Registers                 
***************************************/

#define ChipWaitTimer1_STATUS_REG            (* (reg8 *)  ChipWaitTimer1_StsReg__STATUS_REG)
#define ChipWaitTimer1_STATUS_PTR            (  (reg8 *)  ChipWaitTimer1_StsReg__STATUS_REG)

// データレジスタ(16bit幅)
#define ChipWaitTimer1_FIFO_PTR              (  (reg16 *) ChipWaitTimer1_datapath_u0__F0_REG)

#endif /* ChipWaitTimer1_api_H */

/* [] END OF FILE */
