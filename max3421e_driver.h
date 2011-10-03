#ifndef  _MAX3421E_DRIVER_H_
#define  _MAX3421E_DRIVER_H_

#include "max3421_registers.h"
#include "stm32f10x_gpio.h"

#define SS_DEASSERT  GPIO_SetBits(GPIOA,GPIO_Pin_4)
#define SS_ASSERT GPIO_ResetBits(GPIOA,GPIO_Pin_4)
#define ENABLE_IRQS max3421_wreg(rEPIEN,(bmSUDAVIE)); max3421_wreg(rUSBIEN,(bmURESIE | bmURESDNIE));

#define SET_MAX3420_BIT(reg,bm)   max3421_wreg(reg,(max3421_rreg(reg)|bm))
#define CLR_MAX3420_BIT(reg,bm)   max3421_wreg(reg,(max3421_rregs(reg)&~bm))
#define ENABLE_IRQS max3421_wreg(rEPIEN,(bmSUDAVIE)); max3421_wreg(rUSBIEN,(bmURESIE | bmURESDNIE));

#define STALL_EP0              max3421_wreg(rEPSTALLS,0x23) // Set all three EP0 stall bits--data stage IN/OUT and status stage
#define TRANSMIT_ACK           max3421_rreg_as(rFNADDR) // Set ACKSTAT bit

#define SETBIT(reg,val) max3421_wreg(reg,(max3421_rreg(reg)|val));
#define CLRBIT(reg,val) max3421_wreg(reg,(max3421_rreg(reg)&~val));

#define L0_OFF max3421_wreg(rGPIO,(max3421_rreg(rGPIO) & 0xFE));
#define L0_ON max3421_wreg(rGPIO,(max3421_rreg(rGPIO) | 0x01));
#define L1_OFF max3421_wreg(rGPIO,(max3421_rreg(rGPIO) & 0xFD));
#define L1_ON max3421_wreg(rGPIO,(max3421_rreg(rGPIO) | 0x02));
#define L2_OFF max3421_wreg(rGPIO,(max3421_rreg(rGPIO) & 0xFB));
#define L2_ON max3421_wreg(rGPIO,(max3421_rreg(rGPIO) | 0x04));
#define L3_OFF max3421_wreg(rGPIO,(max3421_rreg(rGPIO) & 0xF7));
#define L3_ON max3421_wreg(rGPIO,(max3421_rreg(rGPIO) | 0x08));
#define L0_BLINK max3421_wreg(rGPIO,(max3421_rreg(rGPIO) ^ 0x01));
#define L1_BLINK max3421_wreg(rGPIO,(max3421_rreg(rGPIO) ^ 0x02));
#define L2_BLINK max3421_wreg(rGPIO,(max3421_rreg(rGPIO) ^ 0x04));
#define L3_BLINK max3421_wreg(rGPIO,(max3421_rreg(rGPIO) ^ 0x08));

void max3421_wreg(unsigned char reg, unsigned  char);
void max3421_wreg_as(unsigned char reg, unsigned char data);
void  max3421_wblock(unsigned char reg, unsigned char len, unsigned char *buffer);
unsigned char max3421_rreg(unsigned char reg);
unsigned char max3421_rreg_as(unsigned char reg);
void max3421_rblock(unsigned char reg, unsigned char len, unsigned char *buffer);
void max3421_init(void);
void max3421_reset(void);

#endif /* _MAX3421E_DRIVER_H_ */
