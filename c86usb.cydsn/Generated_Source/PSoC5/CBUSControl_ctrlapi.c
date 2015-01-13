/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

#include "CBUSControl_ctrlapi.h"

void CBUSControl_Start(void)
{
	// ���ɂ��邱�Ɩ���
}

void CBUSControl_Stop(void)
{
	// ���ɂ��邱�Ɩ���
}

void CBUSControl_Write(uint16_t data)
{
	// �o�X�A�N�Z�X�҂�
	while((CBUSControl_STATUS_REG & CBUSControl_BUS_BUSY) != 0u)
	{
	}
	// ���C�g�w������
	CBUSControl_CMD_FIFO_REG = CBUSControl_COMMAND_WRITE;
	
	// FIFO�Ƀf�[�^�l�߂�
	CY_SET_REG16(CBUSControl_DATA_FIFO_PTR, data);
}


uint16_t CBUSControl_Read()
{
	// �o�X�A�N�Z�X�҂�
	while((CBUSControl_STATUS_REG & CBUSControl_BUS_BUSY) != 0u)
	{
	}
	// ���[�h�w������
	CBUSControl_CMD_FIFO_REG = CBUSControl_COMMAND_READ;

	while((CBUSControl_STATUS_REG & CBUSControl_DATA_VALID) != CBUSControl_DATA_VALID)
	{
		/* wait until input data are valid */
	}

	return ((((uint16) CBUSControl_DIN_MSB_DATA_REG) << 8u) | CBUSControl_DIN_LSB_DATA_REG);
}

void CBUSControl_BusyWait()
{
	// �o�X�A�N�Z�X�҂�
	while((CBUSControl_STATUS_REG & CBUSControl_BUS_BUSY) != 0u)
	{
	}
}

/* [] END OF FILE */
