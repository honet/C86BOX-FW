#ifndef USB_VENDOR_REQUEST_H__
#define USB_VENDOR_REQUEST_H__

#include <stdint.h>

extern volatile uint8_t usbReq_reset;

extern volatile uint8_t usbReq_control;
extern volatile uint8_t usbReq_boardIdx;
extern volatile uint16_t usbReq_controlIdx;
extern volatile uint16_t usbReq_controlValue;

#endif
