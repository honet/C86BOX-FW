/*******************************************************************************
* File Name: CBusAddrRegL_PM.c
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

#include "CBusAddrRegL.h"

/* Check for removal by optimization */
#if !defined(CBusAddrRegL_Sync_ctrl_reg__REMOVED)

static CBusAddrRegL_BACKUP_STRUCT  CBusAddrRegL_backup = {0u};

    
/*******************************************************************************
* Function Name: CBusAddrRegL_SaveConfig
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
void CBusAddrRegL_SaveConfig(void) 
{
    CBusAddrRegL_backup.controlState = CBusAddrRegL_Control;
}


/*******************************************************************************
* Function Name: CBusAddrRegL_RestoreConfig
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
void CBusAddrRegL_RestoreConfig(void) 
{
     CBusAddrRegL_Control = CBusAddrRegL_backup.controlState;
}


/*******************************************************************************
* Function Name: CBusAddrRegL_Sleep
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
void CBusAddrRegL_Sleep(void) 
{
    CBusAddrRegL_SaveConfig();
}


/*******************************************************************************
* Function Name: CBusAddrRegL_Wakeup
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
void CBusAddrRegL_Wakeup(void)  
{
    CBusAddrRegL_RestoreConfig();
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
