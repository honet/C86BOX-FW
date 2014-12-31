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
#if !defined(ChipWaitTimer7_api_H)
#define ChipWaitTimer7_api_H

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

void ChipWaitTimer7_Start(void);
void ChipWaitTimer7_Stop(void);

void ChipWaitTimer7_Set(uint16_t count);
uint8_t ChipWaitTimer7_Query();
void ChipWaitTimer7_Wait();


/***************************************
*           API Constants               
***************************************/

// STATUS_REG のフラグ
#define ChipWaitTimer7_QUEUE_FULL           (0x01u)
#define ChipWaitTimer7_COUNTING             (0x02u)

/***************************************
*             Registers                 
***************************************/

#define ChipWaitTimer7_STATUS_REG            (* (reg8 *)  ChipWaitTimer7_StsReg__STATUS_REG)
#define ChipWaitTimer7_STATUS_PTR            (  (reg8 *)  ChipWaitTimer7_StsReg__STATUS_REG)

// データレジスタ(16bit幅)
#define ChipWaitTimer7_FIFO_PTR              (  (reg16 *) ChipWaitTimer7_datapath_u0__F0_REG)

#endif /* ChipWaitTimer7_api_H */

/* [] END OF FILE */
