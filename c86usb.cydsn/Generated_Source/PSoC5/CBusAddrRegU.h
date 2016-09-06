/*******************************************************************************
* File Name: CBusAddrRegU.h  
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

#if !defined(CY_CONTROL_REG_CBusAddrRegU_H) /* CY_CONTROL_REG_CBusAddrRegU_H */
#define CY_CONTROL_REG_CBusAddrRegU_H

#include "cytypes.h"

    
/***************************************
*     Data Struct Definitions
***************************************/

/* Sleep Mode API Support */
typedef struct
{
    uint8 controlState;

} CBusAddrRegU_BACKUP_STRUCT;


/***************************************
*         Function Prototypes 
***************************************/

void    CBusAddrRegU_Write(uint8 control) ;
uint8   CBusAddrRegU_Read(void) ;

void CBusAddrRegU_SaveConfig(void) ;
void CBusAddrRegU_RestoreConfig(void) ;
void CBusAddrRegU_Sleep(void) ; 
void CBusAddrRegU_Wakeup(void) ;


/***************************************
*            Registers        
***************************************/

/* Control Register */
#define CBusAddrRegU_Control        (* (reg8 *) CBusAddrRegU_Sync_ctrl_reg__CONTROL_REG )
#define CBusAddrRegU_Control_PTR    (  (reg8 *) CBusAddrRegU_Sync_ctrl_reg__CONTROL_REG )

#endif /* End CY_CONTROL_REG_CBusAddrRegU_H */


/* [] END OF FILE */
