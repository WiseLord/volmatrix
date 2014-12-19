#include "tda7448.h"

static int8_t sp[TDA7448_CHANNELS];

static tda7448Param tda7448Par[] = {
	{0, -79, 0},	/* Volume */
	{0, -8, 8},		/* Balance */
	{0, -8, 8},		/* Front */
	{0, -16, 0},	/* Center */
	{0, -16, 0}		/* Subwoofer */
};

void tda7448SetSpeakers(void)
{
	uint8_t i;

	for (i = 0; i < TDA7448_CHANNELS; i++)
		sp[i] = tda7448Par[TDA7448_SND_VOLUME].value;

	if (tda7448Par[TDA7448_SND_BALANCE].value > 0) {
		sp[TDA7448_SP_FRONTLEFT] -= tda7448Par[TDA7448_SND_BALANCE].value;
		sp[TDA7448_SP_REARLEFT] -= tda7448Par[TDA7448_SND_BALANCE].value;
	} else {
		sp[TDA7448_SP_FRONTRIGHT] += tda7448Par[TDA7448_SND_BALANCE].value;
		sp[TDA7448_SP_REARRIGHT] += tda7448Par[TDA7448_SND_BALANCE].value;
	}
	if (tda7448Par[TDA7448_SND_FRONT].value > 0) {
		sp[TDA7448_SP_REARLEFT] -= tda7448Par[TDA7448_SND_FRONT].value;
		sp[TDA7448_SP_REARRIGHT] -= tda7448Par[TDA7448_SND_FRONT].value;
	} else {
		sp[TDA7448_SP_FRONTLEFT] += tda7448Par[TDA7448_SND_FRONT].value;
		sp[TDA7448_SP_FRONTRIGHT] += tda7448Par[TDA7448_SND_FRONT].value;
	}
	sp[TDA7448_SP_CENTER] += tda7448Par[TDA7448_SND_CENTER].value;
	sp[TDA7448_SP_SUBWOOFER] += tda7448Par[TDA7448_SND_SUBWOOFER].value;

	for (i = 0; i < TDA7448_CHANNELS; i++) {
		if (sp[i] < TDA7448_SP_MIN)
			sp[i] = TDA7448_SP_MIN;
	}

	for (i = 0; i < TDA7448_CHANNELS; i++) {
		// Write to I2C
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
