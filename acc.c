#include "acc.h"

volatile ACC_Achse_TypeDef achse[3];

void initAcc(void)
{
	writeSpi(0x20, 0x27);

	for(uint8_t i=0; i<3; i++)
	{
		achse[i].offset = 0;
	}
}

void updateAcc(void)
{
	uint8_t addr = 0x28;

	for(uint8_t i=0; i<3; i++)
	{
		achse[i].low = readSpi(addr);
		addr++;
		achse[i].high = readSpi(addr);
		addr++;
	}
}

int16_t getAchseAcc(uint8_t pos)
{
	int16_t out;

//	achse[2].low = 0x9a;
//	achse[2].high = 0xfe;


	out = ((int16_t)(achse[pos].high<<8 | achse[pos].low))>>4;

	out += achse[pos].offset;

	return out;
}

void calibrateAcc(void)
{
	int16_t tmp;
	updateAcc();

	for(uint8_t i=0; i<3; i++)
	{
		achse[i].offset = 0;
	}

	tmp = getAchseAcc(X_ACHSE);
	achse[X_ACHSE].offset = tmp * (-1);

	tmp = getAchseAcc(Y_ACHSE);
	achse[Y_ACHSE].offset = tmp * (-1);

	tmp = getAchseAcc(Z_ACHSE);
	achse[Z_ACHSE].offset = (tmp - 341) * (-1);
}

int16_t getOffset(uint8_t pos)
{
	return achse[pos].offset;
}
