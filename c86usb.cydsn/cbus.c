
#include <project.h>
#include "CBUSControl_ctrlapi.h"
#include "cbus.h"

/* ------------------------
   ADDR RANGE

  -------------
              0xffff ffff | 1111 1111 1111 1111, 1111 1111 1111 1111
  BANK1       0x8000 0000 | 1000 0000 0000 0000, 0000 0000 0000 0000
  -------------
              0x7fff ffff | 0111 1111 1111 1111, 1111 1111 1111 1111
  BANK0       0x0000 0000 | 0000 0000 0000 0000, 0000 0000 0000 0000
  -------------

  upper[23:16] bitassign = ABCD0000
*/
static void CBUS_SetAddr(uint8_t slot, uint32_t addr)
{
	// アドレスは20bitまで。
	uint8_t ah = (uint8_t)(addr>>8);
	uint8_t au = (uint8_t)((addr>>16)&0x0f);
	
	switch(slot){
	case 0:
		CBusAddrRegU_Write((ah&0x80) ? au : (au|0xf0) );  //    0000 1111
		CBusAddrRegH_Write(ah);
		break;
	case 1:
		CBusAddrRegU_Write((ah&0x80) ? (au|0x80) : (au|0x70) ); // A23 1000 0111
		CBusAddrRegH_Write(ah^0x80); // bit7反転
		break;
	case 2:
		CBusAddrRegU_Write((ah&0x80) ? (au|0x40) : (au|0xb0) ); // A22 0100 1011
		CBusAddrRegH_Write(ah^0x80); // bit7反転
		break;
	case 3:
		CBusAddrRegU_Write((ah&0x80) ? (au|0x20) : (au|0xd0) ); // A21 0010 1101
		CBusAddrRegH_Write(ah^0x80); // bit7反転
		break;
	}
	CBusAddrRegL_Write((uint8_t)(addr&0xff));
}

void cbus_reset(void)
{
	DigitalReset_Write(1);
	CyDelayUs(1000);
	DigitalReset_Write(0);
	CyDelay(1000);
}


void cbus_write(uint8_t slot, uint32_t addr, uint16_t data)
{
	CBUSControl_BusyWait();
	CBUS_SetAddr(slot, addr);
	CBUSControl_Write(data);
}

uint16_t cbus_read(uint8_t slot, uint32_t addr)
{
	CBUSControl_BusyWait();
	CBUS_SetAddr(slot, addr);
	return CBUSControl_Read();
}

