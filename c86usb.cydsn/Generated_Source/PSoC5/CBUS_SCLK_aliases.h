/*******************************************************************************
* File Name: CBUS_SCLK.h  
* Version 2.20
*
* Description:
*  This file contains the Alias definitions for Per-Pin APIs in cypins.h. 
*  Information on using these APIs can be found in the System Reference Guide.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_CBUS_SCLK_ALIASES_H) /* Pins CBUS_SCLK_ALIASES_H */
#define CY_PINS_CBUS_SCLK_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"


/***************************************
*              Constants        
***************************************/
#define CBUS_SCLK_0			(CBUS_SCLK__0__PC)
#define CBUS_SCLK_0_INTR	((uint16)((uint16)0x0001u << CBUS_SCLK__0__SHIFT))

#define CBUS_SCLK_INTR_ALL	 ((uint16)(CBUS_SCLK_0_INTR))

#endif /* End Pins CBUS_SCLK_ALIASES_H */


/* [] END OF FILE */
