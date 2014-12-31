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
#if !defined(ChipWaitTimer3_api_H)
#define ChipWaitTimer3_api_H

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

void ChipWaitTimer3_Start(void);
void ChipWaitTimer3_Stop(void);

void ChipWaitTimer3_Set(uint16_t count);
uint8_t ChipWaitTimer3_Query();
void ChipWaitTimer3_Wait();


/***************************************
*           API Constants               
***************************************/

// STATUS_REG のフラグ
#define ChipWaitTimer3_QUEUE_FULL           (0x01u)
#define ChipWaitTimer3_COUNTING             (0x02u)

/***************************************
*             Registers                 
***************************************/

#define ChipWaitTimer3_STATUS_REG            (* (reg8 *)  ChipWaitTimer3_StsReg__STATUS_REG)
#define ChipWaitTimer3_STATUS_PTR            (  (reg8 *)  ChipWaitTimer3_StsReg__STATUS_REG)

// データレジスタ(16bit幅)
#define ChipWaitTimer3_FIFO_PTR              (  (reg16 *) ChipWaitTimer3_datapath_u0__F0_REG)

#endif /* ChipWaitTimer3_api_H */

/* [] END OF FILE */
