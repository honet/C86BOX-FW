/*******************************************************************************
* File Name: CBusAddrRegU_PM.c
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

#include "CBusAddrRegU.h"

/* Check for removal by optimization */
#if !defined(CBusAddrRegU_Sync_ctrl_reg__REMOVED)

static CBusAddrRegU_BACKUP_STRUCT  CBusAddrRegU_backup = {0u};

    
/*******************************************************************************
* Function Name: CBusAddrRegU_SaveConfig
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
void CBusAddrRegU_SaveConfig(void) 
{
    CBusAddrRegU_backup.controlState = CBusAddrRegU_Control;
}


/*******************************************************************************
* Function Name: CBusAddrRegU_RestoreConfig
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
void CBusAddrRegU_RestoreConfig(void) 
{
     CBusAddrRegU_Control = CBusAddrRegU_backup.controlState;
}


/*******************************************************************************
* Function Name: CBusAddrRegU_Sleep
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
void CBusAddrRegU_Sleep(void) 
{
    CBusAddrRegU_SaveConfig();
}


/*******************************************************************************
* Function Name: CBusAddrRegU_Wakeup
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
void CBusAddrRegU_Wakeup(void)  
{
    CBusAddrRegU_RestoreConfig();
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
