/*******************************************************************************
* File Name: WAITCLK.h
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

#if !defined(CY_CLOCK_WAITCLK_H)
#define CY_CLOCK_WAITCLK_H

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

void WAITCLK_Start(void) ;
void WAITCLK_Stop(void) ;

#if(CY_PSOC3 || CY_PSOC5LP)
void WAITCLK_StopBlock(void) ;
#endif /* (CY_PSOC3 || CY_PSOC5LP) */

void WAITCLK_StandbyPower(uint8 state) ;
void WAITCLK_SetDividerRegister(uint16 clkDivider, uint8 restart) 
                                ;
uint16 WAITCLK_GetDividerRegister(void) ;
void WAITCLK_SetModeRegister(uint8 modeBitMask) ;
void WAITCLK_ClearModeRegister(uint8 modeBitMask) ;
uint8 WAITCLK_GetModeRegister(void) ;
void WAITCLK_SetSourceRegister(uint8 clkSource) ;
uint8 WAITCLK_GetSourceRegister(void) ;
#if defined(WAITCLK__CFG3)
void WAITCLK_SetPhaseRegister(uint8 clkPhase) ;
uint8 WAITCLK_GetPhaseRegister(void) ;
#endif /* defined(WAITCLK__CFG3) */

#define WAITCLK_Enable()                       WAITCLK_Start()
#define WAITCLK_Disable()                      WAITCLK_Stop()
#define WAITCLK_SetDivider(clkDivider)         WAITCLK_SetDividerRegister(clkDivider, 1u)
#define WAITCLK_SetDividerValue(clkDivider)    WAITCLK_SetDividerRegister((clkDivider) - 1u, 1u)
#define WAITCLK_SetMode(clkMode)               WAITCLK_SetModeRegister(clkMode)
#define WAITCLK_SetSource(clkSource)           WAITCLK_SetSourceRegister(clkSource)
#if defined(WAITCLK__CFG3)
#define WAITCLK_SetPhase(clkPhase)             WAITCLK_SetPhaseRegister(clkPhase)
#define WAITCLK_SetPhaseValue(clkPhase)        WAITCLK_SetPhaseRegister((clkPhase) + 1u)
#endif /* defined(WAITCLK__CFG3) */


/***************************************
*             Registers
***************************************/

/* Register to enable or disable the clock */
#define WAITCLK_CLKEN              (* (reg8 *) WAITCLK__PM_ACT_CFG)
#define WAITCLK_CLKEN_PTR          ((reg8 *) WAITCLK__PM_ACT_CFG)

/* Register to enable or disable the clock */
#define WAITCLK_CLKSTBY            (* (reg8 *) WAITCLK__PM_STBY_CFG)
#define WAITCLK_CLKSTBY_PTR        ((reg8 *) WAITCLK__PM_STBY_CFG)

/* Clock LSB divider configuration register. */
#define WAITCLK_DIV_LSB            (* (reg8 *) WAITCLK__CFG0)
#define WAITCLK_DIV_LSB_PTR        ((reg8 *) WAITCLK__CFG0)
#define WAITCLK_DIV_PTR            ((reg16 *) WAITCLK__CFG0)

/* Clock MSB divider configuration register. */
#define WAITCLK_DIV_MSB            (* (reg8 *) WAITCLK__CFG1)
#define WAITCLK_DIV_MSB_PTR        ((reg8 *) WAITCLK__CFG1)

/* Mode and source configuration register */
#define WAITCLK_MOD_SRC            (* (reg8 *) WAITCLK__CFG2)
#define WAITCLK_MOD_SRC_PTR        ((reg8 *) WAITCLK__CFG2)

#if defined(WAITCLK__CFG3)
/* Analog clock phase configuration register */
#define WAITCLK_PHASE              (* (reg8 *) WAITCLK__CFG3)
#define WAITCLK_PHASE_PTR          ((reg8 *) WAITCLK__CFG3)
#endif /* defined(WAITCLK__CFG3) */


/**************************************
*       Register Constants
**************************************/

/* Power manager register masks */
#define WAITCLK_CLKEN_MASK         WAITCLK__PM_ACT_MSK
#define WAITCLK_CLKSTBY_MASK       WAITCLK__PM_STBY_MSK

/* CFG2 field masks */
#define WAITCLK_SRC_SEL_MSK        WAITCLK__CFG2_SRC_SEL_MASK
#define WAITCLK_MODE_MASK          (~(WAITCLK_SRC_SEL_MSK))

#if defined(WAITCLK__CFG3)
/* CFG3 phase mask */
#define WAITCLK_PHASE_MASK         WAITCLK__CFG3_PHASE_DLY_MASK
#endif /* defined(WAITCLK__CFG3) */

#endif /* CY_CLOCK_WAITCLK_H */


/* [] END OF FILE */
