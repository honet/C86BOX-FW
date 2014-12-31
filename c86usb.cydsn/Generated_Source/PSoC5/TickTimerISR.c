/*******************************************************************************
* File Name: TickTimerISR.c  
* Version 1.70
*
*  Description:
*   API for controlling the state of an interrupt.
*
*
*  Note:
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/


#include <cydevice_trm.h>
#include <CyLib.h>
#include <TickTimerISR.h>

#if !defined(TickTimerISR__REMOVED) /* Check for removal by optimization */

/*******************************************************************************
*  Place your includes, defines and code here 
********************************************************************************/
/* `#START TickTimerISR_intc` */

/* `#END` */

#ifndef CYINT_IRQ_BASE
#define CYINT_IRQ_BASE      16
#endif /* CYINT_IRQ_BASE */
#ifndef CYINT_VECT_TABLE
#define CYINT_VECT_TABLE    ((cyisraddress **) CYREG_NVIC_VECT_OFFSET)
#endif /* CYINT_VECT_TABLE */

/* Declared in startup, used to set unused interrupts to. */
CY_ISR_PROTO(IntDefaultHandler);


/*******************************************************************************
* Function Name: TickTimerISR_Start
********************************************************************************
*
* Summary:
*  Set up the interrupt and enable it.
*
* Parameters:  
*   None
*
* Return:
*   None
*
*******************************************************************************/
void TickTimerISR_Start(void)
{
    /* For all we know the interrupt is active. */
    TickTimerISR_Disable();

    /* Set the ISR to point to the TickTimerISR Interrupt. */
    TickTimerISR_SetVector(&TickTimerISR_Interrupt);

    /* Set the priority. */
    TickTimerISR_SetPriority((uint8)TickTimerISR_INTC_PRIOR_NUMBER);

    /* Enable it. */
    TickTimerISR_Enable();
}


/*******************************************************************************
* Function Name: TickTimerISR_StartEx
********************************************************************************
*
* Summary:
*  Set up the interrupt and enable it.
*
* Parameters:  
*   address: Address of the ISR to set in the interrupt vector table.
*
* Return:
*   None
*
*******************************************************************************/
void TickTimerISR_StartEx(cyisraddress address)
{
    /* For all we know the interrupt is active. */
    TickTimerISR_Disable();

    /* Set the ISR to point to the TickTimerISR Interrupt. */
    TickTimerISR_SetVector(address);

    /* Set the priority. */
    TickTimerISR_SetPriority((uint8)TickTimerISR_INTC_PRIOR_NUMBER);

    /* Enable it. */
    TickTimerISR_Enable();
}


/*******************************************************************************
* Function Name: TickTimerISR_Stop
********************************************************************************
*
* Summary:
*   Disables and removes the interrupt.
*
* Parameters:  
*
* Return:
*   None
*
*******************************************************************************/
void TickTimerISR_Stop(void)
{
    /* Disable this interrupt. */
    TickTimerISR_Disable();

    /* Set the ISR to point to the passive one. */
    TickTimerISR_SetVector(&IntDefaultHandler);
}


/*******************************************************************************
* Function Name: TickTimerISR_Interrupt
********************************************************************************
*
* Summary:
*   The default Interrupt Service Routine for TickTimerISR.
*
*   Add custom code between the coments to keep the next version of this file
*   from over writting your code.
*
* Parameters:  
*
* Return:
*   None
*
*******************************************************************************/
CY_ISR(TickTimerISR_Interrupt)
{
    /*  Place your Interrupt code here. */
    /* `#START TickTimerISR_Interrupt` */

    /* `#END` */
}


/*******************************************************************************
* Function Name: TickTimerISR_SetVector
********************************************************************************
*
* Summary:
*   Change the ISR vector for the Interrupt. Note calling TickTimerISR_Start
*   will override any effect this method would have had. To set the vector 
*   before the component has been started use TickTimerISR_StartEx instead.
*
* Parameters:
*   address: Address of the ISR to set in the interrupt vector table.
*
* Return:
*   None
*
*******************************************************************************/
void TickTimerISR_SetVector(cyisraddress address)
{
    cyisraddress * ramVectorTable;

    ramVectorTable = (cyisraddress *) *CYINT_VECT_TABLE;

    ramVectorTable[CYINT_IRQ_BASE + (uint32)TickTimerISR__INTC_NUMBER] = address;
}


/*******************************************************************************
* Function Name: TickTimerISR_GetVector
********************************************************************************
*
* Summary:
*   Gets the "address" of the current ISR vector for the Interrupt.
*
* Parameters:
*   None
*
* Return:
*   Address of the ISR in the interrupt vector table.
*
*******************************************************************************/
cyisraddress TickTimerISR_GetVector(void)
{
    cyisraddress * ramVectorTable;

    ramVectorTable = (cyisraddress *) *CYINT_VECT_TABLE;

    return ramVectorTable[CYINT_IRQ_BASE + (uint32)TickTimerISR__INTC_NUMBER];
}


/*******************************************************************************
* Function Name: TickTimerISR_SetPriority
********************************************************************************
*
* Summary:
*   Sets the Priority of the Interrupt. Note calling TickTimerISR_Start
*   or TickTimerISR_StartEx will override any effect this method 
*   would have had. This method should only be called after 
*   TickTimerISR_Start or TickTimerISR_StartEx has been called. To set 
*   the initial priority for the component use the cydwr file in the tool.
*
* Parameters:
*   priority: Priority of the interrupt. 0 - 7, 0 being the highest.
*
* Return:
*   None
*
*******************************************************************************/
void TickTimerISR_SetPriority(uint8 priority)
{
    *TickTimerISR_INTC_PRIOR = priority << 5;
}


/*******************************************************************************
* Function Name: TickTimerISR_GetPriority
********************************************************************************
*
* Summary:
*   Gets the Priority of the Interrupt.
*
* Parameters:
*   None
*
* Return:
*   Priority of the interrupt. 0 - 7, 0 being the highest.
*
*******************************************************************************/
uint8 TickTimerISR_GetPriority(void)
{
    uint8 priority;


    priority = *TickTimerISR_INTC_PRIOR >> 5;

    return priority;
}


/*******************************************************************************
* Function Name: TickTimerISR_Enable
********************************************************************************
*
* Summary:
*   Enables the interrupt.
*
* Parameters:
*   None
*
* Return:
*   None
*
*******************************************************************************/
void TickTimerISR_Enable(void)
{
    /* Enable the general interrupt. */
    *TickTimerISR_INTC_SET_EN = TickTimerISR__INTC_MASK;
}


/*******************************************************************************
* Function Name: TickTimerISR_GetState
********************************************************************************
*
* Summary:
*   Gets the state (enabled, disabled) of the Interrupt.
*
* Parameters:
*   None
*
* Return:
*   1 if enabled, 0 if disabled.
*
*******************************************************************************/
uint8 TickTimerISR_GetState(void)
{
    /* Get the state of the general interrupt. */
    return ((*TickTimerISR_INTC_SET_EN & (uint32)TickTimerISR__INTC_MASK) != 0u) ? 1u:0u;
}


/*******************************************************************************
* Function Name: TickTimerISR_Disable
********************************************************************************
*
* Summary:
*   Disables the Interrupt.
*
* Parameters:
*   None
*
* Return:
*   None
*
*******************************************************************************/
void TickTimerISR_Disable(void)
{
    /* Disable the general interrupt. */
    *TickTimerISR_INTC_CLR_EN = TickTimerISR__INTC_MASK;
}


/*******************************************************************************
* Function Name: TickTimerISR_SetPending
********************************************************************************
*
* Summary:
*   Causes the Interrupt to enter the pending state, a software method of
*   generating the interrupt.
*
* Parameters:
*   None
*
* Return:
*   None
*
*******************************************************************************/
void TickTimerISR_SetPending(void)
{
    *TickTimerISR_INTC_SET_PD = TickTimerISR__INTC_MASK;
}


/*******************************************************************************
* Function Name: TickTimerISR_ClearPending
********************************************************************************
*
* Summary:
*   Clears a pending interrupt.
*
* Parameters:
*   None
*
* Return:
*   None
*
*******************************************************************************/
void TickTimerISR_ClearPending(void)
{
    *TickTimerISR_INTC_CLR_PD = TickTimerISR__INTC_MASK;
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
