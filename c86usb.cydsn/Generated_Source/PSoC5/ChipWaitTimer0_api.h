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
#if !defined(ChipWaitTimer0_api_H)
#define ChipWaitTimer0_api_H

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

void ChipWaitTimer0_Start(void);
void ChipWaitTimer0_Stop(void);

void ChipWaitTimer0_Set(uint16_t count);
uint8_t ChipWaitTimer0_Query();
void ChipWaitTimer0_Wait();


/***************************************
*           API Constants               
***************************************/

// STATUS_REG のフラグ
#define ChipWaitTimer0_QUEUE_FULL           (0x01u)
#define ChipWaitTimer0_COUNTING             (0x02u)

/***************************************
*             Registers                 
***************************************/

#define ChipWaitTimer0_STATUS_REG            (* (reg8 *)  ChipWaitTimer0_StsReg__STATUS_REG)
#define ChipWaitTimer0_STATUS_PTR            (  (reg8 *)  ChipWaitTimer0_StsReg__STATUS_REG)

// データレジスタ(16bit幅)
#define ChipWaitTimer0_FIFO_PTR              (  (reg16 *) ChipWaitTimer0_datapath_u0__F0_REG)

#endif /* ChipWaitTimer0_api_H */

/* [] END OF FILE */
