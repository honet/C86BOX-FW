/*******************************************************************************
* File Name: DigitalReset_PM.c
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

#include "DigitalReset.h"

/* Check for removal by optimization */
#if !defined(DigitalReset_Sync_ctrl_reg__REMOVED)

static DigitalReset_BACKUP_STRUCT  DigitalReset_backup = {0u};

    
/*******************************************************************************
* Function Name: DigitalReset_SaveConfig
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
void DigitalReset_SaveConfig(void) 
{
    DigitalReset_backup.controlState = DigitalReset_Control;
}


/*******************************************************************************
* Function Name: DigitalReset_RestoreConfig
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
void DigitalReset_RestoreConfig(void) 
{
     DigitalReset_Control = DigitalReset_backup.controlState;
}


/*******************************************************************************
* Function Name: DigitalReset_Sleep
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
void DigitalReset_Sleep(void) 
{
    DigitalReset_SaveConfig();
}


/*******************************************************************************
* Function Name: DigitalReset_Wakeup
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
void DigitalReset_Wakeup(void)  
{
    DigitalReset_RestoreConfig();
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
