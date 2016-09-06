/*******************************************************************************
* File Name: CBUS_SCLK.h  
* Version 2.20
*
* Description:
*  This file contains Pin function prototypes and register defines
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_CBUS_SCLK_H) /* Pins CBUS_SCLK_H */
#define CY_PINS_CBUS_SCLK_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "CBUS_SCLK_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 CBUS_SCLK__PORT == 15 && ((CBUS_SCLK__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    CBUS_SCLK_Write(uint8 value);
void    CBUS_SCLK_SetDriveMode(uint8 mode);
uint8   CBUS_SCLK_ReadDataReg(void);
uint8   CBUS_SCLK_Read(void);
void    CBUS_SCLK_SetInterruptMode(uint16 position, uint16 mode);
uint8   CBUS_SCLK_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the CBUS_SCLK_SetDriveMode() function.
     *  @{
     */
        #define CBUS_SCLK_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define CBUS_SCLK_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define CBUS_SCLK_DM_RES_UP          PIN_DM_RES_UP
        #define CBUS_SCLK_DM_RES_DWN         PIN_DM_RES_DWN
        #define CBUS_SCLK_DM_OD_LO           PIN_DM_OD_LO
        #define CBUS_SCLK_DM_OD_HI           PIN_DM_OD_HI
        #define CBUS_SCLK_DM_STRONG          PIN_DM_STRONG
        #define CBUS_SCLK_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define CBUS_SCLK_MASK               CBUS_SCLK__MASK
#define CBUS_SCLK_SHIFT              CBUS_SCLK__SHIFT
#define CBUS_SCLK_WIDTH              1u

/* Interrupt constants */
#if defined(CBUS_SCLK__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in CBUS_SCLK_SetInterruptMode() function.
     *  @{
     */
        #define CBUS_SCLK_INTR_NONE      (uint16)(0x0000u)
        #define CBUS_SCLK_INTR_RISING    (uint16)(0x0001u)
        #define CBUS_SCLK_INTR_FALLING   (uint16)(0x0002u)
        #define CBUS_SCLK_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define CBUS_SCLK_INTR_MASK      (0x01u) 
#endif /* (CBUS_SCLK__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define CBUS_SCLK_PS                     (* (reg8 *) CBUS_SCLK__PS)
/* Data Register */
#define CBUS_SCLK_DR                     (* (reg8 *) CBUS_SCLK__DR)
/* Port Number */
#define CBUS_SCLK_PRT_NUM                (* (reg8 *) CBUS_SCLK__PRT) 
/* Connect to Analog Globals */                                                  
#define CBUS_SCLK_AG                     (* (reg8 *) CBUS_SCLK__AG)                       
/* Analog MUX bux enable */
#define CBUS_SCLK_AMUX                   (* (reg8 *) CBUS_SCLK__AMUX) 
/* Bidirectional Enable */                                                        
#define CBUS_SCLK_BIE                    (* (reg8 *) CBUS_SCLK__BIE)
/* Bit-mask for Aliased Register Access */
#define CBUS_SCLK_BIT_MASK               (* (reg8 *) CBUS_SCLK__BIT_MASK)
/* Bypass Enable */
#define CBUS_SCLK_BYP                    (* (reg8 *) CBUS_SCLK__BYP)
/* Port wide control signals */                                                   
#define CBUS_SCLK_CTL                    (* (reg8 *) CBUS_SCLK__CTL)
/* Drive Modes */
#define CBUS_SCLK_DM0                    (* (reg8 *) CBUS_SCLK__DM0) 
#define CBUS_SCLK_DM1                    (* (reg8 *) CBUS_SCLK__DM1)
#define CBUS_SCLK_DM2                    (* (reg8 *) CBUS_SCLK__DM2) 
/* Input Buffer Disable Override */
#define CBUS_SCLK_INP_DIS                (* (reg8 *) CBUS_SCLK__INP_DIS)
/* LCD Common or Segment Drive */
#define CBUS_SCLK_LCD_COM_SEG            (* (reg8 *) CBUS_SCLK__LCD_COM_SEG)
/* Enable Segment LCD */
#define CBUS_SCLK_LCD_EN                 (* (reg8 *) CBUS_SCLK__LCD_EN)
/* Slew Rate Control */
#define CBUS_SCLK_SLW                    (* (reg8 *) CBUS_SCLK__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define CBUS_SCLK_PRTDSI__CAPS_SEL       (* (reg8 *) CBUS_SCLK__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define CBUS_SCLK_PRTDSI__DBL_SYNC_IN    (* (reg8 *) CBUS_SCLK__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define CBUS_SCLK_PRTDSI__OE_SEL0        (* (reg8 *) CBUS_SCLK__PRTDSI__OE_SEL0) 
#define CBUS_SCLK_PRTDSI__OE_SEL1        (* (reg8 *) CBUS_SCLK__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define CBUS_SCLK_PRTDSI__OUT_SEL0       (* (reg8 *) CBUS_SCLK__PRTDSI__OUT_SEL0) 
#define CBUS_SCLK_PRTDSI__OUT_SEL1       (* (reg8 *) CBUS_SCLK__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define CBUS_SCLK_PRTDSI__SYNC_OUT       (* (reg8 *) CBUS_SCLK__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(CBUS_SCLK__SIO_CFG)
    #define CBUS_SCLK_SIO_HYST_EN        (* (reg8 *) CBUS_SCLK__SIO_HYST_EN)
    #define CBUS_SCLK_SIO_REG_HIFREQ     (* (reg8 *) CBUS_SCLK__SIO_REG_HIFREQ)
    #define CBUS_SCLK_SIO_CFG            (* (reg8 *) CBUS_SCLK__SIO_CFG)
    #define CBUS_SCLK_SIO_DIFF           (* (reg8 *) CBUS_SCLK__SIO_DIFF)
#endif /* (CBUS_SCLK__SIO_CFG) */

/* Interrupt Registers */
#if defined(CBUS_SCLK__INTSTAT)
    #define CBUS_SCLK_INTSTAT            (* (reg8 *) CBUS_SCLK__INTSTAT)
    #define CBUS_SCLK_SNAP               (* (reg8 *) CBUS_SCLK__SNAP)
    
	#define CBUS_SCLK_0_INTTYPE_REG 		(* (reg8 *) CBUS_SCLK__0__INTTYPE)
#endif /* (CBUS_SCLK__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_CBUS_SCLK_H */


/* [] END OF FILE */
