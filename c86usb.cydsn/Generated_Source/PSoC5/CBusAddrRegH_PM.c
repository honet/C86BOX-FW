/*******************************************************************************
* File Name: CBusAddrRegH_PM.c
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

#include "CBusAddrRegH.h"

/* Check for removal by optimization */
#if !defined(CBusAddrRegH_Sync_ctrl_reg__REMOVED)

static CBusAddrRegH_BACKUP_STRUCT  CBusAddrRegH_backup = {0u};

    
/*******************************************************************************
* Function Name: CBusAddrRegH_SaveConfig
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
void CBusAddrRegH_SaveConfig(void) 
{
    CBusAddrRegH_backup.controlState = CBusAddrRegH_Control;
}


/*******************************************************************************
* Function Name: CBusAddrRegH_RestoreConfig
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
void CBusAddrRegH_RestoreConfig(void) 
{
     CBusAddrRegH_Control = CBusAddrRegH_backup.controlState;
}


/*******************************************************************************
* Function Name: CBusAddrRegH_Sleep
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
void CBusAddrRegH_Sleep(void) 
{
    CBusAddrRegH_SaveConfig();
}


/*******************************************************************************
* Function Name: CBusAddrRegH_Wakeup
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
void CBusAddrRegH_Wakeup(void)  
{
    CBusAddrRegH_RestoreConfig();
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
