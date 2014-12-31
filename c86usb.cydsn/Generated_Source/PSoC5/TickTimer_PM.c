/*******************************************************************************
* File Name: TickTimer_PM.c
* Version 2.60
*
*  Description:
*     This file provides the power management source code to API for the
*     Timer.
*
*   Note:
*     None
*
*******************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/

#include "TickTimer.h"

static TickTimer_backupStruct TickTimer_backup;


/*******************************************************************************
* Function Name: TickTimer_SaveConfig
********************************************************************************
*
* Summary:
*     Save the current user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  TickTimer_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void TickTimer_SaveConfig(void) 
{
    #if (!TickTimer_UsingFixedFunction)
        TickTimer_backup.TimerUdb = TickTimer_ReadCounter();
        TickTimer_backup.InterruptMaskValue = TickTimer_STATUS_MASK;
        #if (TickTimer_UsingHWCaptureCounter)
            TickTimer_backup.TimerCaptureCounter = TickTimer_ReadCaptureCount();
        #endif /* Back Up capture counter register  */

        #if(!TickTimer_UDB_CONTROL_REG_REMOVED)
            TickTimer_backup.TimerControlRegister = TickTimer_ReadControlRegister();
        #endif /* Backup the enable state of the Timer component */
    #endif /* Backup non retention registers in UDB implementation. All fixed function registers are retention */
}


/*******************************************************************************
* Function Name: TickTimer_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the current user configuration.
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  TickTimer_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void TickTimer_RestoreConfig(void) 
{   
    #if (!TickTimer_UsingFixedFunction)

        TickTimer_WriteCounter(TickTimer_backup.TimerUdb);
        TickTimer_STATUS_MASK =TickTimer_backup.InterruptMaskValue;
        #if (TickTimer_UsingHWCaptureCounter)
            TickTimer_SetCaptureCount(TickTimer_backup.TimerCaptureCounter);
        #endif /* Restore Capture counter register*/

        #if(!TickTimer_UDB_CONTROL_REG_REMOVED)
            TickTimer_WriteControlRegister(TickTimer_backup.TimerControlRegister);
        #endif /* Restore the enable state of the Timer component */
    #endif /* Restore non retention registers in the UDB implementation only */
}


/*******************************************************************************
* Function Name: TickTimer_Sleep
********************************************************************************
*
* Summary:
*     Stop and Save the user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  TickTimer_backup.TimerEnableState:  Is modified depending on the
*  enable state of the block before entering sleep mode.
*
*******************************************************************************/
void TickTimer_Sleep(void) 
{
    #if(!TickTimer_UDB_CONTROL_REG_REMOVED)
        /* Save Counter's enable state */
        if(TickTimer_CTRL_ENABLE == (TickTimer_CONTROL & TickTimer_CTRL_ENABLE))
        {
            /* Timer is enabled */
            TickTimer_backup.TimerEnableState = 1u;
        }
        else
        {
            /* Timer is disabled */
            TickTimer_backup.TimerEnableState = 0u;
        }
    #endif /* Back up enable state from the Timer control register */
    TickTimer_Stop();
    TickTimer_SaveConfig();
}


/*******************************************************************************
* Function Name: TickTimer_Wakeup
********************************************************************************
*
* Summary:
*  Restores and enables the user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  TickTimer_backup.enableState:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void TickTimer_Wakeup(void) 
{
    TickTimer_RestoreConfig();
    #if(!TickTimer_UDB_CONTROL_REG_REMOVED)
        if(TickTimer_backup.TimerEnableState == 1u)
        {     /* Enable Timer's operation */
                TickTimer_Enable();
        } /* Do nothing if Timer was disabled before */
    #endif /* Remove this code section if Control register is removed */
}


/* [] END OF FILE */
