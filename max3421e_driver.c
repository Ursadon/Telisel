#include "max3421e_driver.h"
#include "stm32f10x_spi.h"
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

	/* Send SPI1 data */
	SPI_I2S_SendData(SPI1, reg | 2);
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
	garbage = SPI_I2S_ReceiveData(SPI1);

	SPI_I2S_SendData(SPI1, data);
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
	garbage = SPI_I2S_ReceiveData(SPI1);

	SS_DEASSERT;
}

void max3421_wreg_as(unsigned char reg, unsigned char data) {

	unsigned char garbage;
	SS_ASSERT;

	/* Send SPI1 data */
	SPI_I2S_SendData(SPI1, reg | 3);
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
	garbage = SPI_I2S_ReceiveData(SPI1);

	SPI_I2S_SendData(SPI1, data);
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
	garbage = SPI_I2S_ReceiveData(SPI1);

	SS_DEASSERT;
}

unsigned char max3421_rreg(unsigned char reg) {
	SS_ASSERT;

	/* Send SPI1 data */
	SPI_I2S_SendData(SPI1, reg);
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);

	SS_DEASSERT;
	return (SPI_I2S_ReceiveData(SPI1));
}

unsigned char max3421_rreg_as(unsigned char reg) {
	SS_ASSERT;

	/* Send SPI1 data */
	SPI_I2S_SendData(SPI1, reg + 1);
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);

	SS_DEASSERT;
	return (SPI_I2S_ReceiveData(SPI1));
}

extern unsigned char SUD[8];

//  Read a specified number of bytes from MAX3421 into an array
void max3421_rblock(unsigned char reg, unsigned char len, unsigned char *buffer) {
	unsigned char garbage, counter = 0;
	SS_ASSERT;

	/* Send SPI1 data */
	SPI_I2S_SendData(SPI1, reg);
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
	garbage = SPI_I2S_ReceiveData(SPI1);

	for (counter = 0; counter < len; counter++) {
		SPI_I2S_SendData(SPI1, 0xFF); // write a dummy value
		while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
		while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
		*buffer++ = SPI_I2S_ReceiveData(SPI1);
	}
	SS_DEASSERT;
}

//  Write a specified number of bytes to the MAX3421 from an array
void max3421_wblock(unsigned char reg, unsigned char len, unsigned char *buffer) {
	unsigned int garbage, count = 0;
	SS_ASSERT;

	/* Send SPI1 data */
	SPI_I2S_SendData(SPI1, reg + 2);
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
	garbage = SPI_I2S_ReceiveData(SPI1);

	for (count = 0; count < len; count++) {
		SPI_I2S_SendData(SPI1, buffer[count]);
		while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
		while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
		garbage = SPI_I2S_ReceiveData(SPI1);
	}
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
