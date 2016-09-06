/*******************************************************************************
* File Name: TickTimer.h
* Version 2.70
*
*  Description:
*     Contains the function prototypes and constants available to the timer
*     user module.
*
*   Note:
*     None
*
********************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/

#if !defined(CY_Timer_v2_60_TickTimer_H)
#define CY_Timer_v2_60_TickTimer_H

#include "cytypes.h"
#include "cyfitter.h"
#include "CyLib.h" /* For CyEnterCriticalSection() and CyExitCriticalSection() functions */

extern uint8 TickTimer_initVar;

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component Timer_v2_70 requires cy_boot v3.0 or later
#endif /* (CY_ PSOC5LP) */


/**************************************
*           Parameter Defaults
**************************************/

#define TickTimer_Resolution                 16u
#define TickTimer_UsingFixedFunction         1u
#define TickTimer_UsingHWCaptureCounter      0u
#define TickTimer_SoftwareCaptureMode        0u
#define TickTimer_SoftwareTriggerMode        0u
#define TickTimer_UsingHWEnable              0u
#define TickTimer_EnableTriggerMode          0u
#define TickTimer_InterruptOnCaptureCount    0u
#define TickTimer_RunModeUsed                0u
#define TickTimer_ControlRegRemoved          0u

#if defined(TickTimer_TimerUDB_sCTRLReg_SyncCtl_ctrlreg__CONTROL_REG)
    #define TickTimer_UDB_CONTROL_REG_REMOVED            (0u)
#elif  (TickTimer_UsingFixedFunction)
    #define TickTimer_UDB_CONTROL_REG_REMOVED            (0u)
#else 
    #define TickTimer_UDB_CONTROL_REG_REMOVED            (1u)
#endif /* End TickTimer_TimerUDB_sCTRLReg_SyncCtl_ctrlreg__CONTROL_REG */


/***************************************
*       Type defines
***************************************/


/**************************************************************************
 * Sleep Wakeup Backup structure for Timer Component
 *************************************************************************/
typedef struct
{
    uint8 TimerEnableState;
    #if(!TickTimer_UsingFixedFunction)

        uint16 TimerUdb;
        uint8 InterruptMaskValue;
        #if (TickTimer_UsingHWCaptureCounter)
            uint8 TimerCaptureCounter;
        #endif /* variable declarations for backing up non retention registers in CY_UDB_V1 */

        #if (!TickTimer_UDB_CONTROL_REG_REMOVED)
            uint8 TimerControlRegister;
        #endif /* variable declaration for backing up enable state of the Timer */
    #endif /* define backup variables only for UDB implementation. Fixed function registers are all retention */

}TickTimer_backupStruct;


/***************************************
*       Function Prototypes
***************************************/

void    TickTimer_Start(void) ;
void    TickTimer_Stop(void) ;

void    TickTimer_SetInterruptMode(uint8 interruptMode) ;
uint8   TickTimer_ReadStatusRegister(void) ;
/* Deprecated function. Do not use this in future. Retained for backward compatibility */
#define TickTimer_GetInterruptSource() TickTimer_ReadStatusRegister()

#if(!TickTimer_UDB_CONTROL_REG_REMOVED)
    uint8   TickTimer_ReadControlRegister(void) ;
    void    TickTimer_WriteControlRegister(uint8 control) ;
#endif /* (!TickTimer_UDB_CONTROL_REG_REMOVED) */

uint16  TickTimer_ReadPeriod(void) ;
void    TickTimer_WritePeriod(uint16 period) ;
uint16  TickTimer_ReadCounter(void) ;
void    TickTimer_WriteCounter(uint16 counter) ;
uint16  TickTimer_ReadCapture(void) ;
void    TickTimer_SoftwareCapture(void) ;

#if(!TickTimer_UsingFixedFunction) /* UDB Prototypes */
    #if (TickTimer_SoftwareCaptureMode)
        void    TickTimer_SetCaptureMode(uint8 captureMode) ;
    #endif /* (!TickTimer_UsingFixedFunction) */

    #if (TickTimer_SoftwareTriggerMode)
        void    TickTimer_SetTriggerMode(uint8 triggerMode) ;
    #endif /* (TickTimer_SoftwareTriggerMode) */

    #if (TickTimer_EnableTriggerMode)
        void    TickTimer_EnableTrigger(void) ;
        void    TickTimer_DisableTrigger(void) ;
    #endif /* (TickTimer_EnableTriggerMode) */


    #if(TickTimer_InterruptOnCaptureCount)
        void    TickTimer_SetInterruptCount(uint8 interruptCount) ;
    #endif /* (TickTimer_InterruptOnCaptureCount) */

    #if (TickTimer_UsingHWCaptureCounter)
        void    TickTimer_SetCaptureCount(uint8 captureCount) ;
        uint8   TickTimer_ReadCaptureCount(void) ;
    #endif /* (TickTimer_UsingHWCaptureCounter) */

    void TickTimer_ClearFIFO(void) ;
#endif /* UDB Prototypes */

/* Sleep Retention APIs */
void TickTimer_Init(void)          ;
void TickTimer_Enable(void)        ;
void TickTimer_SaveConfig(void)    ;
void TickTimer_RestoreConfig(void) ;
void TickTimer_Sleep(void)         ;
void TickTimer_Wakeup(void)        ;


/***************************************
*   Enumerated Types and Parameters
***************************************/

/* Enumerated Type B_Timer__CaptureModes, Used in Capture Mode */
#define TickTimer__B_TIMER__CM_NONE 0
#define TickTimer__B_TIMER__CM_RISINGEDGE 1
#define TickTimer__B_TIMER__CM_FALLINGEDGE 2
#define TickTimer__B_TIMER__CM_EITHEREDGE 3
#define TickTimer__B_TIMER__CM_SOFTWARE 4



/* Enumerated Type B_Timer__TriggerModes, Used in Trigger Mode */
#define TickTimer__B_TIMER__TM_NONE 0x00u
#define TickTimer__B_TIMER__TM_RISINGEDGE 0x04u
#define TickTimer__B_TIMER__TM_FALLINGEDGE 0x08u
#define TickTimer__B_TIMER__TM_EITHEREDGE 0x0Cu
#define TickTimer__B_TIMER__TM_SOFTWARE 0x10u


/***************************************
*    Initialial Parameter Constants
***************************************/

#define TickTimer_INIT_PERIOD             23999u
#define TickTimer_INIT_CAPTURE_MODE       ((uint8)((uint8)0u << TickTimer_CTRL_CAP_MODE_SHIFT))
#define TickTimer_INIT_TRIGGER_MODE       ((uint8)((uint8)0u << TickTimer_CTRL_TRIG_MODE_SHIFT))
#if (TickTimer_UsingFixedFunction)
    #define TickTimer_INIT_INTERRUPT_MODE (((uint8)((uint8)1u << TickTimer_STATUS_TC_INT_MASK_SHIFT)) | \
                                                  ((uint8)((uint8)0 << TickTimer_STATUS_CAPTURE_INT_MASK_SHIFT)))
#else
    #define TickTimer_INIT_INTERRUPT_MODE (((uint8)((uint8)1u << TickTimer_STATUS_TC_INT_MASK_SHIFT)) | \
                                                 ((uint8)((uint8)0 << TickTimer_STATUS_CAPTURE_INT_MASK_SHIFT)) | \
                                                 ((uint8)((uint8)0 << TickTimer_STATUS_FIFOFULL_INT_MASK_SHIFT)))
#endif /* (TickTimer_UsingFixedFunction) */
#define TickTimer_INIT_CAPTURE_COUNT      (2u)
#define TickTimer_INIT_INT_CAPTURE_COUNT  ((uint8)((uint8)(1u - 1u) << TickTimer_CTRL_INTCNT_SHIFT))


/***************************************
*           Registers
***************************************/

#if (TickTimer_UsingFixedFunction) /* Implementation Specific Registers and Register Constants */


    /***************************************
    *    Fixed Function Registers
    ***************************************/

    #define TickTimer_STATUS         (*(reg8 *) TickTimer_TimerHW__SR0 )
    /* In Fixed Function Block Status and Mask are the same register */
    #define TickTimer_STATUS_MASK    (*(reg8 *) TickTimer_TimerHW__SR0 )
    #define TickTimer_CONTROL        (*(reg8 *) TickTimer_TimerHW__CFG0)
    #define TickTimer_CONTROL2       (*(reg8 *) TickTimer_TimerHW__CFG1)
    #define TickTimer_CONTROL2_PTR   ( (reg8 *) TickTimer_TimerHW__CFG1)
    #define TickTimer_RT1            (*(reg8 *) TickTimer_TimerHW__RT1)
    #define TickTimer_RT1_PTR        ( (reg8 *) TickTimer_TimerHW__RT1)

    #if (CY_PSOC3 || CY_PSOC5LP)
        #define TickTimer_CONTROL3       (*(reg8 *) TickTimer_TimerHW__CFG2)
        #define TickTimer_CONTROL3_PTR   ( (reg8 *) TickTimer_TimerHW__CFG2)
    #endif /* (CY_PSOC3 || CY_PSOC5LP) */
    #define TickTimer_GLOBAL_ENABLE  (*(reg8 *) TickTimer_TimerHW__PM_ACT_CFG)
    #define TickTimer_GLOBAL_STBY_ENABLE  (*(reg8 *) TickTimer_TimerHW__PM_STBY_CFG)

    #define TickTimer_CAPTURE_LSB         (* (reg16 *) TickTimer_TimerHW__CAP0 )
    #define TickTimer_CAPTURE_LSB_PTR       ((reg16 *) TickTimer_TimerHW__CAP0 )
    #define TickTimer_PERIOD_LSB          (* (reg16 *) TickTimer_TimerHW__PER0 )
    #define TickTimer_PERIOD_LSB_PTR        ((reg16 *) TickTimer_TimerHW__PER0 )
    #define TickTimer_COUNTER_LSB         (* (reg16 *) TickTimer_TimerHW__CNT_CMP0 )
    #define TickTimer_COUNTER_LSB_PTR       ((reg16 *) TickTimer_TimerHW__CNT_CMP0 )


    /***************************************
    *    Register Constants
    ***************************************/

    /* Fixed Function Block Chosen */
    #define TickTimer_BLOCK_EN_MASK                     TickTimer_TimerHW__PM_ACT_MSK
    #define TickTimer_BLOCK_STBY_EN_MASK                TickTimer_TimerHW__PM_STBY_MSK

    /* Control Register Bit Locations */
    /* Interrupt Count - Not valid for Fixed Function Block */
    #define TickTimer_CTRL_INTCNT_SHIFT                  0x00u
    /* Trigger Polarity - Not valid for Fixed Function Block */
    #define TickTimer_CTRL_TRIG_MODE_SHIFT               0x00u
    /* Trigger Enable - Not valid for Fixed Function Block */
    #define TickTimer_CTRL_TRIG_EN_SHIFT                 0x00u
    /* Capture Polarity - Not valid for Fixed Function Block */
    #define TickTimer_CTRL_CAP_MODE_SHIFT                0x00u
    /* Timer Enable - As defined in Register Map, part of TMRX_CFG0 register */
    #define TickTimer_CTRL_ENABLE_SHIFT                  0x00u

    /* Control Register Bit Masks */
    #define TickTimer_CTRL_ENABLE                        ((uint8)((uint8)0x01u << TickTimer_CTRL_ENABLE_SHIFT))

    /* Control2 Register Bit Masks */
    /* As defined in Register Map, Part of the TMRX_CFG1 register */
    #define TickTimer_CTRL2_IRQ_SEL_SHIFT                 0x00u
    #define TickTimer_CTRL2_IRQ_SEL                      ((uint8)((uint8)0x01u << TickTimer_CTRL2_IRQ_SEL_SHIFT))

    #if (CY_PSOC5A)
        /* Use CFG1 Mode bits to set run mode */
        /* As defined by Verilog Implementation */
        #define TickTimer_CTRL_MODE_SHIFT                 0x01u
        #define TickTimer_CTRL_MODE_MASK                 ((uint8)((uint8)0x07u << TickTimer_CTRL_MODE_SHIFT))
    #endif /* (CY_PSOC5A) */
    #if (CY_PSOC3 || CY_PSOC5LP)
        /* Control3 Register Bit Locations */
        #define TickTimer_CTRL_RCOD_SHIFT        0x02u
        #define TickTimer_CTRL_ENBL_SHIFT        0x00u
        #define TickTimer_CTRL_MODE_SHIFT        0x00u

        /* Control3 Register Bit Masks */
        #define TickTimer_CTRL_RCOD_MASK  ((uint8)((uint8)0x03u << TickTimer_CTRL_RCOD_SHIFT)) /* ROD and COD bit masks */
        #define TickTimer_CTRL_ENBL_MASK  ((uint8)((uint8)0x80u << TickTimer_CTRL_ENBL_SHIFT)) /* HW_EN bit mask */
        #define TickTimer_CTRL_MODE_MASK  ((uint8)((uint8)0x03u << TickTimer_CTRL_MODE_SHIFT)) /* Run mode bit mask */

        #define TickTimer_CTRL_RCOD       ((uint8)((uint8)0x03u << TickTimer_CTRL_RCOD_SHIFT))
        #define TickTimer_CTRL_ENBL       ((uint8)((uint8)0x80u << TickTimer_CTRL_ENBL_SHIFT))
    #endif /* (CY_PSOC3 || CY_PSOC5LP) */

    /*RT1 Synch Constants: Applicable for PSoC3 and PSoC5LP */
    #define TickTimer_RT1_SHIFT                       0x04u
    /* Sync TC and CMP bit masks */
    #define TickTimer_RT1_MASK                        ((uint8)((uint8)0x03u << TickTimer_RT1_SHIFT))
    #define TickTimer_SYNC                            ((uint8)((uint8)0x03u << TickTimer_RT1_SHIFT))
    #define TickTimer_SYNCDSI_SHIFT                   0x00u
    /* Sync all DSI inputs with Mask  */
    #define TickTimer_SYNCDSI_MASK                    ((uint8)((uint8)0x0Fu << TickTimer_SYNCDSI_SHIFT))
    /* Sync all DSI inputs */
    #define TickTimer_SYNCDSI_EN                      ((uint8)((uint8)0x0Fu << TickTimer_SYNCDSI_SHIFT))

    #define TickTimer_CTRL_MODE_PULSEWIDTH            ((uint8)((uint8)0x01u << TickTimer_CTRL_MODE_SHIFT))
    #define TickTimer_CTRL_MODE_PERIOD                ((uint8)((uint8)0x02u << TickTimer_CTRL_MODE_SHIFT))
    #define TickTimer_CTRL_MODE_CONTINUOUS            ((uint8)((uint8)0x00u << TickTimer_CTRL_MODE_SHIFT))

    /* Status Register Bit Locations */
    /* As defined in Register Map, part of TMRX_SR0 register */
    #define TickTimer_STATUS_TC_SHIFT                 0x07u
    /* As defined in Register Map, part of TMRX_SR0 register, Shared with Compare Status */
    #define TickTimer_STATUS_CAPTURE_SHIFT            0x06u
    /* As defined in Register Map, part of TMRX_SR0 register */
    #define TickTimer_STATUS_TC_INT_MASK_SHIFT        (TickTimer_STATUS_TC_SHIFT - 0x04u)
    /* As defined in Register Map, part of TMRX_SR0 register, Shared with Compare Status */
    #define TickTimer_STATUS_CAPTURE_INT_MASK_SHIFT   (TickTimer_STATUS_CAPTURE_SHIFT - 0x04u)

    /* Status Register Bit Masks */
    #define TickTimer_STATUS_TC                       ((uint8)((uint8)0x01u << TickTimer_STATUS_TC_SHIFT))
    #define TickTimer_STATUS_CAPTURE                  ((uint8)((uint8)0x01u << TickTimer_STATUS_CAPTURE_SHIFT))
    /* Interrupt Enable Bit-Mask for interrupt on TC */
    #define TickTimer_STATUS_TC_INT_MASK              ((uint8)((uint8)0x01u << TickTimer_STATUS_TC_INT_MASK_SHIFT))
    /* Interrupt Enable Bit-Mask for interrupt on Capture */
    #define TickTimer_STATUS_CAPTURE_INT_MASK         ((uint8)((uint8)0x01u << TickTimer_STATUS_CAPTURE_INT_MASK_SHIFT))

#else   /* UDB Registers and Register Constants */


    /***************************************
    *           UDB Registers
    ***************************************/

    #define TickTimer_STATUS              (* (reg8 *) TickTimer_TimerUDB_rstSts_stsreg__STATUS_REG )
    #define TickTimer_STATUS_MASK         (* (reg8 *) TickTimer_TimerUDB_rstSts_stsreg__MASK_REG)
    #define TickTimer_STATUS_AUX_CTRL     (* (reg8 *) TickTimer_TimerUDB_rstSts_stsreg__STATUS_AUX_CTL_REG)
    #define TickTimer_CONTROL             (* (reg8 *) TickTimer_TimerUDB_sCTRLReg_SyncCtl_ctrlreg__CONTROL_REG )
    
    #if(TickTimer_Resolution <= 8u) /* 8-bit Timer */
        #define TickTimer_CAPTURE_LSB         (* (reg8 *) TickTimer_TimerUDB_sT16_timerdp_u0__F0_REG )
        #define TickTimer_CAPTURE_LSB_PTR       ((reg8 *) TickTimer_TimerUDB_sT16_timerdp_u0__F0_REG )
        #define TickTimer_PERIOD_LSB          (* (reg8 *) TickTimer_TimerUDB_sT16_timerdp_u0__D0_REG )
        #define TickTimer_PERIOD_LSB_PTR        ((reg8 *) TickTimer_TimerUDB_sT16_timerdp_u0__D0_REG )
        #define TickTimer_COUNTER_LSB         (* (reg8 *) TickTimer_TimerUDB_sT16_timerdp_u0__A0_REG )
        #define TickTimer_COUNTER_LSB_PTR       ((reg8 *) TickTimer_TimerUDB_sT16_timerdp_u0__A0_REG )
    #elif(TickTimer_Resolution <= 16u) /* 8-bit Timer */
        #if(CY_PSOC3) /* 8-bit addres space */
            #define TickTimer_CAPTURE_LSB         (* (reg16 *) TickTimer_TimerUDB_sT16_timerdp_u0__F0_REG )
            #define TickTimer_CAPTURE_LSB_PTR       ((reg16 *) TickTimer_TimerUDB_sT16_timerdp_u0__F0_REG )
            #define TickTimer_PERIOD_LSB          (* (reg16 *) TickTimer_TimerUDB_sT16_timerdp_u0__D0_REG )
            #define TickTimer_PERIOD_LSB_PTR        ((reg16 *) TickTimer_TimerUDB_sT16_timerdp_u0__D0_REG )
            #define TickTimer_COUNTER_LSB         (* (reg16 *) TickTimer_TimerUDB_sT16_timerdp_u0__A0_REG )
            #define TickTimer_COUNTER_LSB_PTR       ((reg16 *) TickTimer_TimerUDB_sT16_timerdp_u0__A0_REG )
        #else /* 16-bit address space */
            #define TickTimer_CAPTURE_LSB         (* (reg16 *) TickTimer_TimerUDB_sT16_timerdp_u0__16BIT_F0_REG )
            #define TickTimer_CAPTURE_LSB_PTR       ((reg16 *) TickTimer_TimerUDB_sT16_timerdp_u0__16BIT_F0_REG )
            #define TickTimer_PERIOD_LSB          (* (reg16 *) TickTimer_TimerUDB_sT16_timerdp_u0__16BIT_D0_REG )
            #define TickTimer_PERIOD_LSB_PTR        ((reg16 *) TickTimer_TimerUDB_sT16_timerdp_u0__16BIT_D0_REG )
            #define TickTimer_COUNTER_LSB         (* (reg16 *) TickTimer_TimerUDB_sT16_timerdp_u0__16BIT_A0_REG )
            #define TickTimer_COUNTER_LSB_PTR       ((reg16 *) TickTimer_TimerUDB_sT16_timerdp_u0__16BIT_A0_REG )
        #endif /* CY_PSOC3 */
    #elif(TickTimer_Resolution <= 24u)/* 24-bit Timer */
        #define TickTimer_CAPTURE_LSB         (* (reg32 *) TickTimer_TimerUDB_sT16_timerdp_u0__F0_REG )
        #define TickTimer_CAPTURE_LSB_PTR       ((reg32 *) TickTimer_TimerUDB_sT16_timerdp_u0__F0_REG )
        #define TickTimer_PERIOD_LSB          (* (reg32 *) TickTimer_TimerUDB_sT16_timerdp_u0__D0_REG )
        #define TickTimer_PERIOD_LSB_PTR        ((reg32 *) TickTimer_TimerUDB_sT16_timerdp_u0__D0_REG )
        #define TickTimer_COUNTER_LSB         (* (reg32 *) TickTimer_TimerUDB_sT16_timerdp_u0__A0_REG )
        #define TickTimer_COUNTER_LSB_PTR       ((reg32 *) TickTimer_TimerUDB_sT16_timerdp_u0__A0_REG )
    #else /* 32-bit Timer */
        #if(CY_PSOC3 || CY_PSOC5) /* 8-bit address space */
            #define TickTimer_CAPTURE_LSB         (* (reg32 *) TickTimer_TimerUDB_sT16_timerdp_u0__F0_REG )
            #define TickTimer_CAPTURE_LSB_PTR       ((reg32 *) TickTimer_TimerUDB_sT16_timerdp_u0__F0_REG )
            #define TickTimer_PERIOD_LSB          (* (reg32 *) TickTimer_TimerUDB_sT16_timerdp_u0__D0_REG )
            #define TickTimer_PERIOD_LSB_PTR        ((reg32 *) TickTimer_TimerUDB_sT16_timerdp_u0__D0_REG )
            #define TickTimer_COUNTER_LSB         (* (reg32 *) TickTimer_TimerUDB_sT16_timerdp_u0__A0_REG )
            #define TickTimer_COUNTER_LSB_PTR       ((reg32 *) TickTimer_TimerUDB_sT16_timerdp_u0__A0_REG )
        #else /* 32-bit address space */
            #define TickTimer_CAPTURE_LSB         (* (reg32 *) TickTimer_TimerUDB_sT16_timerdp_u0__32BIT_F0_REG )
            #define TickTimer_CAPTURE_LSB_PTR       ((reg32 *) TickTimer_TimerUDB_sT16_timerdp_u0__32BIT_F0_REG )
            #define TickTimer_PERIOD_LSB          (* (reg32 *) TickTimer_TimerUDB_sT16_timerdp_u0__32BIT_D0_REG )
            #define TickTimer_PERIOD_LSB_PTR        ((reg32 *) TickTimer_TimerUDB_sT16_timerdp_u0__32BIT_D0_REG )
            #define TickTimer_COUNTER_LSB         (* (reg32 *) TickTimer_TimerUDB_sT16_timerdp_u0__32BIT_A0_REG )
            #define TickTimer_COUNTER_LSB_PTR       ((reg32 *) TickTimer_TimerUDB_sT16_timerdp_u0__32BIT_A0_REG )
        #endif /* CY_PSOC3 || CY_PSOC5 */ 
    #endif

    #define TickTimer_COUNTER_LSB_PTR_8BIT       ((reg8 *) TickTimer_TimerUDB_sT16_timerdp_u0__A0_REG )
    
    #if (TickTimer_UsingHWCaptureCounter)
        #define TickTimer_CAP_COUNT              (*(reg8 *) TickTimer_TimerUDB_sCapCount_counter__PERIOD_REG )
        #define TickTimer_CAP_COUNT_PTR          ( (reg8 *) TickTimer_TimerUDB_sCapCount_counter__PERIOD_REG )
        #define TickTimer_CAPTURE_COUNT_CTRL     (*(reg8 *) TickTimer_TimerUDB_sCapCount_counter__CONTROL_AUX_CTL_REG )
        #define TickTimer_CAPTURE_COUNT_CTRL_PTR ( (reg8 *) TickTimer_TimerUDB_sCapCount_counter__CONTROL_AUX_CTL_REG )
    #endif /* (TickTimer_UsingHWCaptureCounter) */


    /***************************************
    *       Register Constants
    ***************************************/

    /* Control Register Bit Locations */
    #define TickTimer_CTRL_INTCNT_SHIFT              0x00u       /* As defined by Verilog Implementation */
    #define TickTimer_CTRL_TRIG_MODE_SHIFT           0x02u       /* As defined by Verilog Implementation */
    #define TickTimer_CTRL_TRIG_EN_SHIFT             0x04u       /* As defined by Verilog Implementation */
    #define TickTimer_CTRL_CAP_MODE_SHIFT            0x05u       /* As defined by Verilog Implementation */
    #define TickTimer_CTRL_ENABLE_SHIFT              0x07u       /* As defined by Verilog Implementation */

    /* Control Register Bit Masks */
    #define TickTimer_CTRL_INTCNT_MASK               ((uint8)((uint8)0x03u << TickTimer_CTRL_INTCNT_SHIFT))
    #define TickTimer_CTRL_TRIG_MODE_MASK            ((uint8)((uint8)0x03u << TickTimer_CTRL_TRIG_MODE_SHIFT))
    #define TickTimer_CTRL_TRIG_EN                   ((uint8)((uint8)0x01u << TickTimer_CTRL_TRIG_EN_SHIFT))
    #define TickTimer_CTRL_CAP_MODE_MASK             ((uint8)((uint8)0x03u << TickTimer_CTRL_CAP_MODE_SHIFT))
    #define TickTimer_CTRL_ENABLE                    ((uint8)((uint8)0x01u << TickTimer_CTRL_ENABLE_SHIFT))

    /* Bit Counter (7-bit) Control Register Bit Definitions */
    /* As defined by the Register map for the AUX Control Register */
    #define TickTimer_CNTR_ENABLE                    0x20u

    /* Status Register Bit Locations */
    #define TickTimer_STATUS_TC_SHIFT                0x00u  /* As defined by Verilog Implementation */
    #define TickTimer_STATUS_CAPTURE_SHIFT           0x01u  /* As defined by Verilog Implementation */
    #define TickTimer_STATUS_TC_INT_MASK_SHIFT       TickTimer_STATUS_TC_SHIFT
    #define TickTimer_STATUS_CAPTURE_INT_MASK_SHIFT  TickTimer_STATUS_CAPTURE_SHIFT
    #define TickTimer_STATUS_FIFOFULL_SHIFT          0x02u  /* As defined by Verilog Implementation */
    #define TickTimer_STATUS_FIFONEMP_SHIFT          0x03u  /* As defined by Verilog Implementation */
    #define TickTimer_STATUS_FIFOFULL_INT_MASK_SHIFT TickTimer_STATUS_FIFOFULL_SHIFT

    /* Status Register Bit Masks */
    /* Sticky TC Event Bit-Mask */
    #define TickTimer_STATUS_TC                      ((uint8)((uint8)0x01u << TickTimer_STATUS_TC_SHIFT))
    /* Sticky Capture Event Bit-Mask */
    #define TickTimer_STATUS_CAPTURE                 ((uint8)((uint8)0x01u << TickTimer_STATUS_CAPTURE_SHIFT))
    /* Interrupt Enable Bit-Mask */
    #define TickTimer_STATUS_TC_INT_MASK             ((uint8)((uint8)0x01u << TickTimer_STATUS_TC_SHIFT))
    /* Interrupt Enable Bit-Mask */
    #define TickTimer_STATUS_CAPTURE_INT_MASK        ((uint8)((uint8)0x01u << TickTimer_STATUS_CAPTURE_SHIFT))
    /* NOT-Sticky FIFO Full Bit-Mask */
    #define TickTimer_STATUS_FIFOFULL                ((uint8)((uint8)0x01u << TickTimer_STATUS_FIFOFULL_SHIFT))
    /* NOT-Sticky FIFO Not Empty Bit-Mask */
    #define TickTimer_STATUS_FIFONEMP                ((uint8)((uint8)0x01u << TickTimer_STATUS_FIFONEMP_SHIFT))
    /* Interrupt Enable Bit-Mask */
    #define TickTimer_STATUS_FIFOFULL_INT_MASK       ((uint8)((uint8)0x01u << TickTimer_STATUS_FIFOFULL_SHIFT))

    #define TickTimer_STATUS_ACTL_INT_EN             0x10u   /* As defined for the ACTL Register */

    /* Datapath Auxillary Control Register definitions */
    #define TickTimer_AUX_CTRL_FIFO0_CLR             0x01u   /* As defined by Register map */
    #define TickTimer_AUX_CTRL_FIFO1_CLR             0x02u   /* As defined by Register map */
    #define TickTimer_AUX_CTRL_FIFO0_LVL             0x04u   /* As defined by Register map */
    #define TickTimer_AUX_CTRL_FIFO1_LVL             0x08u   /* As defined by Register map */
    #define TickTimer_STATUS_ACTL_INT_EN_MASK        0x10u   /* As defined for the ACTL Register */

#endif /* Implementation Specific Registers and Register Constants */

#endif  /* CY_Timer_v2_30_TickTimer_H */


/* [] END OF FILE */
