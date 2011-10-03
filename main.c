#include "stm32f10x.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_flash.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_exti.h"
#include "max3421e_driver.h"
#include "misc.h"
#include "usb_ch9.h"

#define GREEN_ON  GPIO_SetBits(GPIOC,GPIO_Pin_9)
#define GREEN_OFF GPIO_ResetBits(GPIOC,GPIO_Pin_9)

#define BLUE_ON  GPIO_SetBits(GPIOC,GPIO_Pin_8)
#define BLUE_OFF GPIO_ResetBits(GPIOC,GPIO_Pin_8)

SPI_InitTypeDef SPI_InitStructure;
GPIO_InitTypeDef GPIO_InitStructure;
ErrorStatus HSEStartUpStatus;
EXTI_InitTypeDef EXTI_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;

extern unsigned char Suspended;
extern unsigned char inhibit_send;
unsigned int int_count = 0;

void delay_u(unsigned long us) {
	us = us * 8;
	while (us != 0) {
		us--;
	}
}
void SetSysClockTo24(void) {
	/* SYSCLK, HCLK, PCLK2 and PCLK1 configuration -----------------------------*/
	/* RCC system reset(for debug purpose) */
	RCC_DeInit();

	/* Enable HSE */
	RCC_HSEConfig(RCC_HSE_ON);

	/* Wait till HSE is ready */
	HSEStartUpStatus = RCC_WaitForHSEStartUp();

	if (HSEStartUpStatus == SUCCESS) {
		/* Enable Prefetch Buffer */
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

		/* Flash 0 wait state */
		FLASH_SetLatency(FLASH_Latency_0);

		/* HCLK = SYSCLK */
		RCC_HCLKConfig(RCC_SYSCLK_Div1);

		/* PCLK2 = HCLK */
		RCC_PCLK2Config(RCC_HCLK_Div1);

		/* PCLK1 = HCLK */
		RCC_PCLK1Config(RCC_HCLK_Div1);

		/* PLLCLK = 8MHz * 3 = 24 MHz */
		RCC_PLLConfig(0x00010000, 0x00040000);

		/* Enable PLL */
		RCC_PLLCmd(ENABLE);

		/* Wait till PLL is ready */
		while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET) {
		}

		/* Select PLL as system clock source */
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

		/* Wait till PLL is used as system clock source */
		while (RCC_GetSYSCLKSource() != 0x08) {
		}
	} else { /* If HSE fails to start-up, the application will have wrong clock configuration.
	 User can add here some code to deal with this error */

		/* Go to infinite loop */
		while (1) {
		}
	}
}
void NVIC_Configuration(void) {
	//
	// Set the Vector Table base location at 0x08000000
	//
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);

	//
	// Configure one bit for preemption priority
	//
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

	//
	// Enable the EXTI9_5 Interrupt
	//
	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

int main(void) {
	SetSysClockTo24();
	/* Enable peripheral clocks ----------------------------------------------*/
	/* GPIOA, GPIOB and SPI1 clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB
			| RCC_APB2Periph_GPIOC | RCC_APB2Periph_SPI1 | RCC_APB2Periph_AFIO,
			ENABLE);
	/* SPI2 Peripheral clock enable */
	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

	/* Configure SPI1 pins: SCK, MISO and MOSI -------------------------------*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure pins: ~SS ------------------------------*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure pins: BUTTON ------------------------------*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure pins: PORTC ------------------------------*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* Configure pins: interrupts------------------------------*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* SPI1 Configuration ----------------------------------------------------*/
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI1, &SPI_InitStructure);

	/* Enable SPI1 */
	SPI_Cmd(SPI1, ENABLE);

	max3421_init();
	//    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource3);
	//    EXTI_InitStructure.EXTI_Line = EXTI_Line3;
	//    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	//    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	//    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	//    EXTI_Init(&EXTI_InitStructure);
	//    NVIC_Configuration();

	BLUE_ON;
	int vl = 0;
	while (1) {
		if (Suspended) {
			check_for_resume();
		}
		//if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3) == 0) {
		service_irqs();
		//}
		if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 1) {
			max3421_wreg(rEP3INFIFO,2);			// send the "keys up" code
			max3421_wreg(rEP3INFIFO,0);
			max3421_wreg(rEP3INFIFO,20);
			max3421_wreg_as(rEP3INBC,3);				// arm it
			GREEN_ON;
		}
		asm("nop");
	}
}

//**************************************************************************
//
//This function handles External lines 9 to 5 interrupt request.
//
//**************************************************************************
void EXTI3_IRQHandler(void) {
	int_count++;
	if (EXTI_GetITStatus(EXTI_Line3) != RESET) {
		//service_irqs();
		EXTI_ClearITPendingBit(EXTI_Line3);
	}
}
