/*******************************************************************************
* File Name: EEPROM.h
* Version 2.10
*
* Description:
*  Provides the function definitions for the EEPROM APIs.
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_EEPROM_EEPROM_H)
#define CY_EEPROM_EEPROM_H

#include "cydevice_trm.h"
#include "CyFlash.h"

#if !defined(CY_PSOC5LP)
    #error Component EEPROM_v2_10 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5LP) */


/***************************************
*        Function Prototypes
***************************************/

#if (CY_PSOC3 || CY_PSOC5LP) 
    void EEPROM_Enable(void) ;
    void EEPROM_Start(void); 
    void EEPROM_Stop(void) ;
#endif /* (CY_PSOC3 || CY_PSOC5LP) */

cystatus EEPROM_EraseSector(uint8 sectorNumber) ;
cystatus EEPROM_Write(const uint8 * rowData, uint8 rowNumber) ;
cystatus EEPROM_StartWrite(const uint8 * rowData, uint8 rowNumber) \
            ;
cystatus EEPROM_QueryWrite(void) ;
cystatus EEPROM_ByteWrite(uint8 dataByte, uint8 rowNumber, uint8 byteNumber) \
            ;


/****************************************
*           API Constants
****************************************/

#define EEPROM_EEPROM_SIZE    		CYDEV_EE_SIZE
#define EEPROM_SPC_BYTE_WRITE_SIZE    (0x01u)


/*******************************************************************************
* Following code are OBSOLETE and must not be used starting from EEPROM 2.10
*******************************************************************************/
#define SPC_BYTE_WRITE_SIZE             (EEPROM_SPC_BYTE_WRITE_SIZE)

#endif /* CY_EEPROM_EEPROM_H */

/* [] END OF FILE */
