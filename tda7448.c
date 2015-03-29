#include "tda7448.h"
#include "i2c.h"

static int8_t sp[TDA7448_LINES_COUNT];

static tda7448Param tda7448Par[] = {
	{0, -79, 0},		/* Volume */
	{0, -16, 16},		/* Balance */
	{0, -16, 16},		/* Front */
	{0, -16, 0},		/* Center */
	{0, -16, 0}			/* Subwoofer */
};

void tda7448SetSpeakers(void)
{
	uint8_t i;
	uint8_t i2cData;

	for (i = 0; i < TDA7448_LINES_COUNT; i++)
		sp[i] = tda7448Par[TDA7448_SND_VOLUME].value;

	if (tda7448Par[TDA7448_SND_BALANCE].value > 0) {
		sp[TDA7448_FRONT_LEFT] -= tda7448Par[TDA7448_SND_BALANCE].value;
		sp[TDA7448_REAR_LEFT] -= tda7448Par[TDA7448_SND_BALANCE].value;
	} else {
		sp[TDA7448_FRONT_RIGHT] += tda7448Par[TDA7448_SND_BALANCE].value;
		sp[TDA7448_REAR_RIGHT] += tda7448Par[TDA7448_SND_BALANCE].value;
	}
	if (tda7448Par[TDA7448_SND_FRONT].value > 0) {
		sp[TDA7448_REAR_LEFT] -= tda7448Par[TDA7448_SND_FRONT].value;
		sp[TDA7448_REAR_RIGHT] -= tda7448Par[TDA7448_SND_FRONT].value;
	} else {
		sp[TDA7448_FRONT_LEFT] += tda7448Par[TDA7448_SND_FRONT].value;
		sp[TDA7448_FRONT_RIGHT] += tda7448Par[TDA7448_SND_FRONT].value;
	}
	sp[TDA7448_CENTER] += tda7448Par[TDA7448_SND_CENTER].value;
	sp[TDA7448_SUBWOOFER] += tda7448Par[TDA7448_SND_SUBWOOFER].value;

	I2CStart(TDA7448_I2C_ADDR);
	I2CWriteByte(TDA7448_AUTO_INC);
	for (i = 0; i < TDA7448_LINES_COUNT; i++) {
		/* Limit values sent to bus */
		if (sp[i] < tda7448Par[TDA7448_SND_VOLUME].min)
			sp[i] = tda7448Par[TDA7448_SND_VOLUME].min;
		i2cData = -sp[i];
		/* Jump at -72db in raw data according the datasheet */
		if (i2cData >= 72)
			i2cData += 56;
		I2CWriteByte(i2cData);
	}
	I2CStop();

	return;
}

void tda7448SetMute(uint8_t val)
{
	uint8_t i;

	if (val == MUTE_ON) {
		I2CStart(TDA7448_I2C_ADDR);
		I2CWriteByte(TDA7448_AUTO_INC);
		for (i = 0; i < TDA7448_LINES_COUNT; i++)
			I2CWriteByte(TDA7448_MUTE);
		I2CStop();
	} else {
		tda7448SetSpeakers();
	}

	return;
}

void tda7448ChangeParam(uint8_t param, int8_t diff)
{
	tda7448Par[param].value += diff;

	if (tda7448Par[param].value < tda7448Par[param].min)
		tda7448Par[param].value = tda7448Par[param].min;
	if (tda7448Par[param].value > tda7448Par[param].max)
		tda7448Par[param].value = tda7448Par[param].max;

	tda7448SetSpeakers();

	return;
}

int8_t tda7448GetParam(uint8_t param)
{
	return tda7448Par[param].value;
}
