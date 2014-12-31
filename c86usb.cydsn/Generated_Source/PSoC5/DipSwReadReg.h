/*******************************************************************************
* File Name: DipSwReadReg.h  
* Version 1.80
*
* Description:
*  This file containts Status Register function prototypes and register defines
*
* Note:
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_STATUS_REG_DipSwReadReg_H) /* CY_STATUS_REG_DipSwReadReg_H */
#define CY_STATUS_REG_DipSwReadReg_H

#include "cytypes.h"
#include "CyLib.h"


/***************************************
*        Function Prototypes
***************************************/

uint8 DipSwReadReg_Read(void) ;
void DipSwReadReg_InterruptEnable(void) ;
void DipSwReadReg_InterruptDisable(void) ;
void DipSwReadReg_WriteMask(uint8 mask) ;
uint8 DipSwReadReg_ReadMask(void) ;


/***************************************
*           API Constants
***************************************/

#define DipSwReadReg_STATUS_INTR_ENBL    0x10u


/***************************************
*         Parameter Constants
***************************************/

/* Status Register Inputs */
#define DipSwReadReg_INPUTS              4


/***************************************
*             Registers
***************************************/

/* Status Register */
#define DipSwReadReg_Status             (* (reg8 *) DipSwReadReg_sts_sts_reg__STATUS_REG )
#define DipSwReadReg_Status_PTR         (  (reg8 *) DipSwReadReg_sts_sts_reg__STATUS_REG )
#define DipSwReadReg_Status_Mask        (* (reg8 *) DipSwReadReg_sts_sts_reg__MASK_REG )
#define DipSwReadReg_Status_Aux_Ctrl    (* (reg8 *) DipSwReadReg_sts_sts_reg__STATUS_AUX_CTL_REG )

#endif /* End CY_STATUS_REG_DipSwReadReg_H */


/* [] END OF FILE */
