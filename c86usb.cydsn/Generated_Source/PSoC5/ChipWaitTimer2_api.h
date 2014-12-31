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
#if !defined(ChipWaitTimer2_api_H)
#define ChipWaitTimer2_api_H

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

void ChipWaitTimer2_Start(void);
void ChipWaitTimer2_Stop(void);

void ChipWaitTimer2_Set(uint16_t count);
uint8_t ChipWaitTimer2_Query();
void ChipWaitTimer2_Wait();


/***************************************
*           API Constants               
***************************************/

// STATUS_REG のフラグ
#define ChipWaitTimer2_QUEUE_FULL           (0x01u)
#define ChipWaitTimer2_COUNTING             (0x02u)

/***************************************
*             Registers                 
***************************************/

#define ChipWaitTimer2_STATUS_REG            (* (reg8 *)  ChipWaitTimer2_StsReg__STATUS_REG)
#define ChipWaitTimer2_STATUS_PTR            (  (reg8 *)  ChipWaitTimer2_StsReg__STATUS_REG)

// データレジスタ(16bit幅)
#define ChipWaitTimer2_FIFO_PTR              (  (reg16 *) ChipWaitTimer2_datapath_u0__F0_REG)

#endif /* ChipWaitTimer2_api_H */

/* [] END OF FILE */
