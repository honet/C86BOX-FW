/*******************************************************************************
* File Name: TickTimerISR.h
* Version 1.70
*
*  Description:
*   Provides the function definitions for the Interrupt Controller.
*
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/
#if !defined(CY_ISR_TickTimerISR_H)
#define CY_ISR_TickTimerISR_H


#include <cytypes.h>
#include <cyfitter.h>

/* Interrupt Controller API. */
void TickTimerISR_Start(void);
void TickTimerISR_StartEx(cyisraddress address);
void TickTimerISR_Stop(void);

CY_ISR_PROTO(TickTimerISR_Interrupt);

void TickTimerISR_SetVector(cyisraddress address);
cyisraddress TickTimerISR_GetVector(void);

void TickTimerISR_SetPriority(uint8 priority);
uint8 TickTimerISR_GetPriority(void);

void TickTimerISR_Enable(void);
uint8 TickTimerISR_GetState(void);
void TickTimerISR_Disable(void);

void TickTimerISR_SetPending(void);
void TickTimerISR_ClearPending(void);


/* Interrupt Controller Constants */

/* Address of the INTC.VECT[x] register that contains the Address of the TickTimerISR ISR. */
#define TickTimerISR_INTC_VECTOR            ((reg32 *) TickTimerISR__INTC_VECT)

/* Address of the TickTimerISR ISR priority. */
#define TickTimerISR_INTC_PRIOR             ((reg8 *) TickTimerISR__INTC_PRIOR_REG)

/* Priority of the TickTimerISR interrupt. */
#define TickTimerISR_INTC_PRIOR_NUMBER      TickTimerISR__INTC_PRIOR_NUM

/* Address of the INTC.SET_EN[x] byte to bit enable TickTimerISR interrupt. */
#define TickTimerISR_INTC_SET_EN            ((reg32 *) TickTimerISR__INTC_SET_EN_REG)

/* Address of the INTC.CLR_EN[x] register to bit clear the TickTimerISR interrupt. */
#define TickTimerISR_INTC_CLR_EN            ((reg32 *) TickTimerISR__INTC_CLR_EN_REG)

/* Address of the INTC.SET_PD[x] register to set the TickTimerISR interrupt state to pending. */
#define TickTimerISR_INTC_SET_PD            ((reg32 *) TickTimerISR__INTC_SET_PD_REG)

/* Address of the INTC.CLR_PD[x] register to clear the TickTimerISR interrupt. */
#define TickTimerISR_INTC_CLR_PD            ((reg32 *) TickTimerISR__INTC_CLR_PD_REG)


#endif /* CY_ISR_TickTimerISR_H */


/* [] END OF FILE */
