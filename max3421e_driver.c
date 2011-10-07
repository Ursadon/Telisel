#include "max3421e_driver.h"
#include <stm_spi_master.h>
#include "cox_spi.h"
#include "stm32f10x_gpio.h"

void delay_uss(unsigned long us) {
	us = us * 16;
	while (us != 0) {
		asm("nop");
		us--;
	}
}

void max3421_wreg(unsigned char reg, unsigned char data) {

	unsigned char garbage;
	SS_ASSERT;

	garbage = pi_spi1.ReadWrite(reg | 2);
	garbage = pi_spi1.ReadWrite(data);

	SS_DEASSERT;
}

void max3421_wreg_as(unsigned char reg, unsigned char data) {

	unsigned char garbage;
	SS_ASSERT;

	garbage = pi_spi1.ReadWrite(reg | 3);
	garbage = pi_spi1.ReadWrite(data);

	SS_DEASSERT;
}

unsigned char max3421_rreg(unsigned char reg) {
	unsigned char garbage;
	SS_ASSERT;

	garbage = pi_spi1.ReadWrite(reg);

	SS_DEASSERT;
	return garbage;
}

unsigned char max3421_rreg_as(unsigned char reg) {
	unsigned char garbage;
	SS_ASSERT;

	garbage = pi_spi1.ReadWrite(reg | 1);

	SS_DEASSERT;
	return garbage;
}

extern unsigned char SUD[8];

//  Read a specified number of bytes from MAX3421 into an array
void max3421_rblock(unsigned char reg, unsigned char len, unsigned char *buffer) {
	unsigned char garbage, counter = 0;
	SS_ASSERT;

	garbage = pi_spi1.ReadWrite(reg);
	pi_spi1.Read(buffer,len);
	SS_DEASSERT;
}

//  Write a specified number of bytes to the MAX3421 from an array
void max3421_wblock(unsigned char reg, unsigned char len, unsigned char *buffer) {
	unsigned int garbage, count = 0;
	SS_ASSERT;

	garbage = pi_spi1.ReadWrite(reg);
	pi_spi1.Write(buffer,len);
	SS_DEASSERT;
}

void max3421_reset(void) {
	unsigned char garbage;
	max3421_wreg(rUSBCTL, bmCHIPRES); // chip reset
	max3421_wreg(rUSBCTL, 0x00); // remove the reset
	do {
		garbage = max3421_rreg(rUSBIRQ);
		garbage &= bmOSCOKIRQ;
	} while (garbage == 0);
}

void max3421_init(void) {

	SS_ASSERT;

	// MAX3421E: SPI=full-duplex, INT=neg level, GPX=SOF
	max3421_wreg(rPINCTL, (bmFDUPSPI));
	max3421_reset();

	// Configure for self-powered device
	// CONNECT=1 connects D+ pullup to Vcc, except...
	// VBGATE=1 disconnects D+ pullup if host turns off VBUS
	max3421_wreg(rUSBCTL, (bmCONNECT | bmVBGATE));
	ENABLE_IRQS;
	// Set the required IRQ enable bits in the MAX3421 IE registers
	max3421_wreg(rCPUCTL, bmIE);

	SS_DEASSERT;
}
