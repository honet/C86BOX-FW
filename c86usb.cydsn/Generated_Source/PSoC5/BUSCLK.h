/*******************************************************************************
* File Name: BUSCLK.h
* Version 2.20
*
*  Description:
*   Provides the function and constant definitions for the clock component.
*
*  Note:
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_CLOCK_BUSCLK_H)
#define CY_CLOCK_BUSCLK_H

#include <cytypes.h>
#include <cyfitter.h>


/***************************************
* Conditional Compilation Parameters
***************************************/

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component cy_clock_v2_20 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5LP) */


/***************************************
*        Function Prototypes
***************************************/

void BUSCLK_Start(void) ;
void BUSCLK_Stop(void) ;

#if(CY_PSOC3 || CY_PSOC5LP)
void BUSCLK_StopBlock(void) ;
#endif /* (CY_PSOC3 || CY_PSOC5LP) */

void BUSCLK_StandbyPower(uint8 state) ;
void BUSCLK_SetDividerRegister(uint16 clkDivider, uint8 restart) 
                                ;
uint16 BUSCLK_GetDividerRegister(void) ;
void BUSCLK_SetModeRegister(uint8 modeBitMask) ;
void BUSCLK_ClearModeRegister(uint8 modeBitMask) ;
uint8 BUSCLK_GetModeRegister(void) ;
void BUSCLK_SetSourceRegister(uint8 clkSource) ;
uint8 BUSCLK_GetSourceRegister(void) ;
#if defined(BUSCLK__CFG3)
void BUSCLK_SetPhaseRegister(uint8 clkPhase) ;
uint8 BUSCLK_GetPhaseRegister(void) ;
#endif /* defined(BUSCLK__CFG3) */

#define BUSCLK_Enable()                       BUSCLK_Start()
#define BUSCLK_Disable()                      BUSCLK_Stop()
#define BUSCLK_SetDivider(clkDivider)         BUSCLK_SetDividerRegister(clkDivider, 1u)
#define BUSCLK_SetDividerValue(clkDivider)    BUSCLK_SetDividerRegister((clkDivider) - 1u, 1u)
#define BUSCLK_SetMode(clkMode)               BUSCLK_SetModeRegister(clkMode)
#define BUSCLK_SetSource(clkSource)           BUSCLK_SetSourceRegister(clkSource)
#if defined(BUSCLK__CFG3)
#define BUSCLK_SetPhase(clkPhase)             BUSCLK_SetPhaseRegister(clkPhase)
#define BUSCLK_SetPhaseValue(clkPhase)        BUSCLK_SetPhaseRegister((clkPhase) + 1u)
#endif /* defined(BUSCLK__CFG3) */


/***************************************
*             Registers
***************************************/

/* Register to enable or disable the clock */
#define BUSCLK_CLKEN              (* (reg8 *) BUSCLK__PM_ACT_CFG)
#define BUSCLK_CLKEN_PTR          ((reg8 *) BUSCLK__PM_ACT_CFG)

/* Register to enable or disable the clock */
#define BUSCLK_CLKSTBY            (* (reg8 *) BUSCLK__PM_STBY_CFG)
#define BUSCLK_CLKSTBY_PTR        ((reg8 *) BUSCLK__PM_STBY_CFG)

/* Clock LSB divider configuration register. */
#define BUSCLK_DIV_LSB            (* (reg8 *) BUSCLK__CFG0)
#define BUSCLK_DIV_LSB_PTR        ((reg8 *) BUSCLK__CFG0)
#define BUSCLK_DIV_PTR            ((reg16 *) BUSCLK__CFG0)

/* Clock MSB divider configuration register. */
#define BUSCLK_DIV_MSB            (* (reg8 *) BUSCLK__CFG1)
#define BUSCLK_DIV_MSB_PTR        ((reg8 *) BUSCLK__CFG1)

/* Mode and source configuration register */
#define BUSCLK_MOD_SRC            (* (reg8 *) BUSCLK__CFG2)
#define BUSCLK_MOD_SRC_PTR        ((reg8 *) BUSCLK__CFG2)

#if defined(BUSCLK__CFG3)
/* Analog clock phase configuration register */
#define BUSCLK_PHASE              (* (reg8 *) BUSCLK__CFG3)
#define BUSCLK_PHASE_PTR          ((reg8 *) BUSCLK__CFG3)
#endif /* defined(BUSCLK__CFG3) */


/**************************************
*       Register Constants
**************************************/

/* Power manager register masks */
#define BUSCLK_CLKEN_MASK         BUSCLK__PM_ACT_MSK
#define BUSCLK_CLKSTBY_MASK       BUSCLK__PM_STBY_MSK

/* CFG2 field masks */
#define BUSCLK_SRC_SEL_MSK        BUSCLK__CFG2_SRC_SEL_MASK
#define BUSCLK_MODE_MASK          (~(BUSCLK_SRC_SEL_MSK))

#if defined(BUSCLK__CFG3)
/* CFG3 phase mask */
#define BUSCLK_PHASE_MASK         BUSCLK__CFG3_PHASE_DLY_MASK
#endif /* defined(BUSCLK__CFG3) */

#endif /* CY_CLOCK_BUSCLK_H */


/* [] END OF FILE */
