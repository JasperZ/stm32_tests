#include "stm32f10x.h"

extern void initSpi(void);
extern uint8_t readSpi(uint8_t addr);
extern uint8_t writeSpi(uint8_t addr, uint8_t value);
