#include "spi.h"

void initSpi(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

	//SPI SCK
	GPIOA->CRL |= GPIO_CRL_MODE5_1 | GPIO_CRL_MODE5_0 | GPIO_CRL_CNF5_1;
	GPIOA->CRL &= ~GPIO_CRL_CNF5_0;

	//SPI MOSI
	GPIOA->CRL |= GPIO_CRL_MODE7_1 | GPIO_CRL_MODE7_0 | GPIO_CRL_CNF7_1;
	GPIOA->CRL &= ~GPIO_CRL_CNF7_0;

	//SPI MISO
	GPIOA->CRL |= GPIO_CRL_CNF6_1;
	GPIOA->CRL &= ~GPIO_CRL_CNF6_0;

	//SPI NSS
	GPIOA->CRL |= GPIO_CRL_MODE4_1 | GPIO_CRL_MODE4_0;
	GPIOA->CRL &= ~(GPIO_CRL_CNF4_0 | GPIO_CRL_CNF4_1);
	GPIOA->ODR |= GPIO_ODR_ODR4;

	SPI1->CR1 |= SPI_CR1_DFF | SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0 | SPI_CR1_MSTR | SPI_CR1_CPOL | SPI_CR1_CPHA;

	SPI1->CR2 |= SPI_CR2_SSOE;

	SPI1->CR1 |= SPI_CR1_SPE;
}

uint8_t readSpi(uint8_t addr)
{
	uint16_t out = (0x80 | addr)<<8;

	GPIOA->ODR &= ~GPIO_ODR_ODR4;
	SPI1->DR = out;

	while((SPI1->SR & SPI_SR_BSY) != 0)
	{
		;
	}

	while((SPI1->SR & SPI_SR_RXNE) == 0)
	{
		;
	}

	GPIOA->ODR |= GPIO_ODR_ODR4;
	out = SPI1->DR;

	return out;
}

uint8_t writeSpi(uint8_t addr, uint8_t value)
{
	uint16_t out;

	out = ((uint16_t)addr)<<8;
	out |= (uint16_t)value;

	GPIOA->ODR &= ~GPIO_ODR_ODR4;
	SPI1->DR = out;

	while((SPI1->SR & SPI_SR_BSY) != 0)
	{
		;
	}

	while((SPI1->SR & SPI_SR_TXE) == 0)
	{
		;
	}

	GPIOA->ODR |= GPIO_ODR_ODR4;
	out = SPI1->DR;

	return out;
}
