/*******************************************************************************
* File Name: LEDControl_PM.c
* Version 1.80
*
* Description:
*  This file contains the setup, control, and status commands to support 
*  the component operation in the low power mode. 
*
* Note:
*
********************************************************************************
* Copyright 2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "LEDControl.h"

/* Check for removal by optimization */
#if !defined(LEDControl_Sync_ctrl_reg__REMOVED)

static LEDControl_BACKUP_STRUCT  LEDControl_backup = {0u};

    
/*******************************************************************************
* Function Name: LEDControl_SaveConfig
********************************************************************************
*
* Summary:
*  Saves the control register value.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void LEDControl_SaveConfig(void) 
{
    LEDControl_backup.controlState = LEDControl_Control;
}


/*******************************************************************************
* Function Name: LEDControl_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the control register value.
*
* Parameters:
*  None
*
* Return:
*  None
*
*
*******************************************************************************/
void LEDControl_RestoreConfig(void) 
{
     LEDControl_Control = LEDControl_backup.controlState;
}


/*******************************************************************************
* Function Name: LEDControl_Sleep
********************************************************************************
*
* Summary:
*  Prepares the component for entering the low power mode.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void LEDControl_Sleep(void) 
{
    LEDControl_SaveConfig();
}


/*******************************************************************************
* Function Name: LEDControl_Wakeup
********************************************************************************
*
* Summary:
*  Restores the component after waking up from the low power mode.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void LEDControl_Wakeup(void)  
{
    LEDControl_RestoreConfig();
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
