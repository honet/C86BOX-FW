/*******************************************************************************
* File Name: LEDControl.h  
* Version 1.80
*
* Description:
*  This file containts Control Register function prototypes and register defines
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_CONTROL_REG_LEDControl_H) /* CY_CONTROL_REG_LEDControl_H */
#define CY_CONTROL_REG_LEDControl_H

#include "cytypes.h"

    
/***************************************
*     Data Struct Definitions
***************************************/

/* Sleep Mode API Support */
typedef struct
{
    uint8 controlState;

} LEDControl_BACKUP_STRUCT;


/***************************************
*         Function Prototypes 
***************************************/

void    LEDControl_Write(uint8 control) ;
uint8   LEDControl_Read(void) ;

void LEDControl_SaveConfig(void) ;
void LEDControl_RestoreConfig(void) ;
void LEDControl_Sleep(void) ; 
void LEDControl_Wakeup(void) ;


/***************************************
*            Registers        
***************************************/

/* Control Register */
#define LEDControl_Control        (* (reg8 *) LEDControl_Sync_ctrl_reg__CONTROL_REG )
#define LEDControl_Control_PTR    (  (reg8 *) LEDControl_Sync_ctrl_reg__CONTROL_REG )

#endif /* End CY_CONTROL_REG_LEDControl_H */


/* [] END OF FILE */
