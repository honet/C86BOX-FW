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
#if !defined(ChipWaitTimer5_api_H)
#define ChipWaitTimer5_api_H

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

void ChipWaitTimer5_Start(void);
void ChipWaitTimer5_Stop(void);

void ChipWaitTimer5_Set(uint16_t count);
uint8_t ChipWaitTimer5_Query();
void ChipWaitTimer5_Wait();


/***************************************
*           API Constants               
***************************************/

// STATUS_REG のフラグ
#define ChipWaitTimer5_QUEUE_FULL           (0x01u)
#define ChipWaitTimer5_COUNTING             (0x02u)

/***************************************
*             Registers                 
***************************************/

#define ChipWaitTimer5_STATUS_REG            (* (reg8 *)  ChipWaitTimer5_StsReg__STATUS_REG)
#define ChipWaitTimer5_STATUS_PTR            (  (reg8 *)  ChipWaitTimer5_StsReg__STATUS_REG)

// データレジスタ(16bit幅)
#define ChipWaitTimer5_FIFO_PTR              (  (reg16 *) ChipWaitTimer5_datapath_u0__F0_REG)

#endif /* ChipWaitTimer5_api_H */

/* [] END OF FILE */
