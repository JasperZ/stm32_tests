#include "usart.h"

volatile Buffer_TypeDef outputBuffer;
volatile Buffer_TypeDef inputBuffer;

void initUsart(void)
{
	outputBuffer.readPtr = outputBuffer.buffer;
	outputBuffer.writePtr = outputBuffer.buffer;
	outputBuffer.records = 0;

	inputBuffer.readPtr = inputBuffer.buffer;
	inputBuffer.writePtr = inputBuffer.buffer;
	inputBuffer.records = 0;

	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

	GPIOA->CRH |= GPIO_CRH_MODE9_0 | GPIO_CRH_MODE9_1 | GPIO_CRH_CNF9_1;
	GPIOA->CRH &= ~GPIO_CRH_CNF9_0;

	GPIOA->CRH &= ~(GPIO_CRH_MODE10_0 | GPIO_CRH_MODE10_1 | GPIO_CRH_CNF10_0);
	GPIOA->CRH |= GPIO_CRH_CNF10_1;

	USART1->CR1 = 0;
	USART1->CR2 = 0;
	USART1->CR3 = 0;

	GPIOA->CRH |= GPIO_CRH_CNF9_1 | GPIO_CRH_MODE9_1 | GPIO_CRH_MODE9_0;
	GPIOA->CRH |= GPIO_CRH_CNF10_0;

	//Baud 256000
	USART1->BRR = (uint32_t)188;
	//enable
	USART1->CR1 |= USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE;

	NVIC_EnableIRQ(USART1_IRQn);
	USART1->CR1 |= USART_CR1_UE;
}

void writeCharUsart(uint8_t c)
{
	*outputBuffer.writePtr = c;

	outputBuffer.writePtr++;

	if(outputBuffer.writePtr == outputBuffer.buffer+BUFFER_SIZE)
	{
		outputBuffer.writePtr = outputBuffer.buffer;
	}

	USART1->CR1 |= USART_CR1_TXEIE;
}

void writeStringUsart(uint8_t* string)
{
	uint8_t* ptr;

	ptr = string;

	while(*ptr != '\0')
	{
		writeCharUsart(*ptr);
		ptr++;
	}

	writeCharUsart('\r');
}

uint8_t readOutputBuffer(void)
{
	uint8_t out;

	out = *outputBuffer.readPtr;

	outputBuffer.readPtr++;

	if(outputBuffer.readPtr == outputBuffer.buffer+BUFFER_SIZE)
	{
		outputBuffer.readPtr = outputBuffer.buffer;
	}

	if(outputBuffer.readPtr == outputBuffer.writePtr)
	{
		USART1->CR1 &= ~USART_CR1_TXEIE;
	}

	return out;
}

uint8_t readUsart(void)
{
	uint8_t out;

	out = *inputBuffer.readPtr;

	inputBuffer.readPtr++;

	if(inputBuffer.readPtr == inputBuffer.buffer+BUFFER_SIZE)
	{
		inputBuffer.readPtr = inputBuffer.buffer;
	}

	if(out == '\r')
	{
		inputBuffer.records--;
	}

	return out;
}


void writeInputBuffer(uint8_t c)
{
	*inputBuffer.writePtr = c;

	inputBuffer.writePtr++;

	if(inputBuffer.writePtr == inputBuffer.buffer+BUFFER_SIZE)
	{
		inputBuffer.writePtr = inputBuffer.buffer;
	}

	if(c == '\r')
	{
		inputBuffer.records++;
	}
}

uint8_t anyInputUsart(void)
{
	return inputBuffer.records;
}

void USART1_IRQHandler(void)
{
	if((USART1->SR & USART_SR_RXNE) != 0)
	{
		USART1->SR &= ~USART_SR_RXNE;

		writeInputBuffer(USART1->DR);

	}

	if((USART1->SR & USART_SR_TXE) != 0)
	{
		USART1->SR &= ~USART_SR_TXE;

		if(outputBuffer.readPtr != outputBuffer.writePtr)
		{
			USART1->DR = readOutputBuffer();
		}
	}
}
