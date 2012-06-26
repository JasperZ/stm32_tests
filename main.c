#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "usart.h"
#include "spi.h"
#include "acc.h"

uint32_t test = 0;
volatile uint32_t cmp = 732;

void initClock(void)
{
	//warte bis interner Oszillator betriebsbereit
	while((RCC->CR & RCC_CR_HSIRDY) == 0)
	{
		;
	}

	//PLL Multiplikator auf 12 einstellen => (4/2)*12=48MHZ
	//AHB Bus PLL=48MHZ
	//APB1 Bus PPL/2=24MHZ
	//APB2 Bus PLL/1=48MHZ
	//USBCLK PLL/1=48MHZ
	RCC->CFGR |= RCC_CFGR_PLLMULL_3 | RCC_CFGR_PLLMULL_1 | RCC_CFGR_PPRE1_2 | RCC_CFGR_USBPRE;

	//aktiviere PLL
	RCC->CR |= RCC_CR_PLLON;

	//warte auf PLL lock
	while((RCC->CR & RCC_CR_PLLRDY) == 0)
	{
		;
	}

	//setze PLL als Systemtakt
	RCC->CFGR |= RCC_CFGR_SW_1;

	//warte bis PLL als Systemtakt verwendet wird
	while((RCC->CFGR & RCC_CFGR_SWS_1) == 0)
	{
		;
	}
}

void initTimer1(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;

	TIM1->CR1 |= TIM_CR1_ARPE | TIM_CR1_DIR | TIM_CR1_URS;
	TIM1->DIER |= TIM_DIER_UIE;
	TIM1->PSC = 0;
	TIM1->ARR = (uint16_t)65535;

	NVIC_EnableIRQ(TIM1_UP_IRQn);
	TIM1->CR1 |= TIM_CR1_CEN;
}

void TIM1_UP_IRQHandler(void)
{
	TIM1->SR &= ~TIM_SR_UIF;
	test++;
}

void parseInput(void)
{
	uint8_t buffer[128];
	uint8_t i;
	uint8_t tmp;

	if(anyInputUsart() > 0)
	{
		i = 0;
		tmp = 0;

		do
		{
			if(i != 0)
			{
				buffer[i-1] = tmp;
			}

			tmp = readUsart();
			i++;
		} while((i<128) && (tmp != '\r'));

		buffer[i-1] = '\0';
/*
		i = 0;

		while(*(buffer+i) != '\0')
		{
			writeUsart(*(buffer+i));
			i++;
		}
*/
		if(strcmp((char*)buffer, "on") == 0)
		{
			GPIOA->ODR |= GPIO_ODR_ODR4;
		}
		else if(strcmp((char*)buffer, "off") == 0)
		{
			GPIOA->ODR &= ~GPIO_ODR_ODR4;
		}
		else if(strcmp((char*)buffer, "+") == 0)
		{
			cmp += 100;
		}
		else if(strcmp((char*)buffer, "-") == 0)
		{
			cmp -=100;
		}
		else if(strcmp((char*)buffer, "cal") == 0)
		{

			writeStringUsart("calibrate");

			calibrateAcc();
		}
	}
}

int main(void)
{
	uint8_t buffer[16];

	initClock();
	initTimer1();
	initUsart();
	initSpi();

	initAcc();

	GPIOA->CRL |= GPIO_CRL_MODE3_1 | GPIO_CRL_MODE3_0;
	GPIOA->CRL &= ~GPIO_CRL_CNF3_0;

	while(1)
	{
		if(test > cmp)
		{
			updateAcc();
/*
			sprintf(buffer, "X-Achse: %i Offset: %i", data1[0][X_ACHSE], getOffset(X_ACHSE));
			writeStringUsart(buffer);
			sprintf(buffer, "Y-Achse: %i Offset: %i", data1[0][Y_ACHSE], getOffset(Y_ACHSE));
			writeStringUsart(buffer);
			sprintf(buffer, "Z-Achse: %i Offset: %i", data1[0][Z_ACHSE], getOffset(Z_ACHSE));
			writeStringUsart(buffer);
			writeCharUsart('\r');
*//**/
			sprintf(buffer, "%i:%i:%i", getAchseAcc(X_ACHSE), getAchseAcc(Y_ACHSE), getAchseAcc(Z_ACHSE));
			writeStringUsart(buffer);
/**/
			//LED Blinken
			if((GPIOA->ODR & GPIO_ODR_ODR3) != 0)
			{
				GPIOA->ODR &= ~GPIO_ODR_ODR3;
			}
			else
			{
				GPIOA->ODR |= GPIO_ODR_ODR3;
			}

			test = 0;
		}

		parseInput();
	}

	return 0;
}
