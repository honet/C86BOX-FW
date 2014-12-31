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
#if !defined(`$INSTANCE_NAME`_api_H)
#define `$INSTANCE_NAME`_api_H

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

void `$INSTANCE_NAME`_Start(void);
void `$INSTANCE_NAME`_Stop(void);

void `$INSTANCE_NAME`_Set(uint16_t count);
uint8_t `$INSTANCE_NAME`_Query();
void `$INSTANCE_NAME`_Wait();


/***************************************
*           API Constants               
***************************************/

// STATUS_REG のフラグ
#define `$INSTANCE_NAME`_QUEUE_FULL           (0x01u)
#define `$INSTANCE_NAME`_COUNTING             (0x02u)

/***************************************
*             Registers                 
***************************************/

#define `$INSTANCE_NAME`_STATUS_REG            (* (reg8 *)  `$INSTANCE_NAME`_StsReg__STATUS_REG)
#define `$INSTANCE_NAME`_STATUS_PTR            (  (reg8 *)  `$INSTANCE_NAME`_StsReg__STATUS_REG)

// データレジスタ(16bit幅)
#define `$INSTANCE_NAME`_FIFO_PTR              (  (reg16 *) `$INSTANCE_NAME`_datapath_u0__F0_REG)

#endif /* `$INSTANCE_NAME`_api_H */

/* [] END OF FILE */
