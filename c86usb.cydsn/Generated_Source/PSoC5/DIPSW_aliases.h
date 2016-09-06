/*******************************************************************************
* File Name: DIPSW.h  
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

#if !defined(CY_PINS_DIPSW_ALIASES_H) /* Pins DIPSW_ALIASES_H */
#define CY_PINS_DIPSW_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"


/***************************************
*              Constants        
***************************************/
#define DIPSW_0			(DIPSW__0__PC)
#define DIPSW_0_INTR	((uint16)((uint16)0x0001u << DIPSW__0__SHIFT))

#define DIPSW_1			(DIPSW__1__PC)
#define DIPSW_1_INTR	((uint16)((uint16)0x0001u << DIPSW__1__SHIFT))

#define DIPSW_2			(DIPSW__2__PC)
#define DIPSW_2_INTR	((uint16)((uint16)0x0001u << DIPSW__2__SHIFT))

#define DIPSW_3			(DIPSW__3__PC)
#define DIPSW_3_INTR	((uint16)((uint16)0x0001u << DIPSW__3__SHIFT))

#define DIPSW_INTR_ALL	 ((uint16)(DIPSW_0_INTR| DIPSW_1_INTR| DIPSW_2_INTR| DIPSW_3_INTR))

#endif /* End Pins DIPSW_ALIASES_H */


/* [] END OF FILE */
