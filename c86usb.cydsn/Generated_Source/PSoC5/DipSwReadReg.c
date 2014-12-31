/*******************************************************************************
* File Name: DipSwReadReg.c  
* Version 1.80
*
* Description:
*  This file contains API to enable firmware to read the value of a Status 
*  Register.
*
* Note:
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "DipSwReadReg.h"

#if !defined(DipSwReadReg_sts_sts_reg__REMOVED) /* Check for removal by optimization */


/*******************************************************************************
* Function Name: DipSwReadReg_Read
********************************************************************************
*
* Summary:
*  Reads the current value assigned to the Status Register.
*
* Parameters:
*  None.
*
* Return:
*  The current value in the Status Register.
*
*******************************************************************************/
uint8 DipSwReadReg_Read(void) 
{ 
    return DipSwReadReg_Status;
}


/*******************************************************************************
* Function Name: DipSwReadReg_InterruptEnable
********************************************************************************
*
* Summary:
*  Enables the Status Register interrupt.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void DipSwReadReg_InterruptEnable(void) 
{
    uint8 interruptState;
    interruptState = CyEnterCriticalSection();
    DipSwReadReg_Status_Aux_Ctrl |= DipSwReadReg_STATUS_INTR_ENBL;
    CyExitCriticalSection(interruptState);
}


/*******************************************************************************
* Function Name: DipSwReadReg_InterruptDisable
********************************************************************************
*
* Summary:
*  Disables the Status Register interrupt.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void DipSwReadReg_InterruptDisable(void) 
{
    uint8 interruptState;
    interruptState = CyEnterCriticalSection();
    DipSwReadReg_Status_Aux_Ctrl &= (uint8)(~DipSwReadReg_STATUS_INTR_ENBL);
    CyExitCriticalSection(interruptState);
}


/*******************************************************************************
* Function Name: DipSwReadReg_WriteMask
********************************************************************************
*
* Summary:
*  Writes the current mask value assigned to the Status Register.
*
* Parameters:
*  mask:  Value to write into the mask register.
*
* Return:
*  None.
*
*******************************************************************************/
void DipSwReadReg_WriteMask(uint8 mask) 
{
    #if(DipSwReadReg_INPUTS < 8u)
    	mask &= (uint8)((((uint8)1u) << DipSwReadReg_INPUTS) - 1u);
	#endif /* End DipSwReadReg_INPUTS < 8u */
    DipSwReadReg_Status_Mask = mask;
}


/*******************************************************************************
* Function Name: DipSwReadReg_ReadMask
********************************************************************************
*
* Summary:
*  Reads the current interrupt mask assigned to the Status Register.
*
* Parameters:
*  None.
*
* Return:
*  The value of the interrupt mask of the Status Register.
*
*******************************************************************************/
uint8 DipSwReadReg_ReadMask(void) 
{
    return DipSwReadReg_Status_Mask;
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
