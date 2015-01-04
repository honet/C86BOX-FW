#ifndef USB_VENDOR_REQUEST_H__
#define USB_VENDOR_REQUEST_H__

#include <stdint.h>
#include "cbus_board.h"

extern volatile uint8_t usbReq_reset;
extern volatile uint8_t usbReq_setBoardType;
extern volatile uint32_t usbReq_boardType[NMAXBOARDS];

#endif
