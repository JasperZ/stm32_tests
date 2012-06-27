#include "stm32f10x.h"

#define G_WERTEBEREICH (uint8_t)12
#define X_ACHSE (uint8_t)0
#define Y_ACHSE (uint8_t)1
#define Z_ACHSE (uint8_t)2

//Raw data struct
typedef struct
{
	uint8_t high;
	uint8_t low;
	int16_t offset;
} ACC_Achse_TypeDef;

extern void initAcc(void);
extern void updateAcc(void);
extern int16_t getAchseAcc(uint8_t pos);
extern void calibrateAcc(void);
extern int16_t getOffset(uint8_t pos);
