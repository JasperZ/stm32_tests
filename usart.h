#include "stm32f10x.h"

#define BUFFER_SIZE 256

typedef struct
{
	uint8_t buffer[BUFFER_SIZE];
	uint8_t* writePtr;
	uint8_t* readPtr;
	uint8_t records;
} Buffer_TypeDef;

extern void initUsart(void);
extern void writeCharUsart(uint8_t c);
extern void writeStringUsart(uint8_t* string);
uint8_t readOutputBuffer(void);
extern uint8_t readUsart(void);
void writeInputBuffer(uint8_t c);
extern uint8_t anyInputUsart(void);
extern void USART1_IRQHandler(void);
