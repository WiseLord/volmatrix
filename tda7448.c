#include "tda7448.h"

static int8_t sp[TDA7448_CHANNELS];

static int8_t volume;		/* -79..0 dB */
static int8_t balance;		/*  -8..8 dB */
static int8_t front;		/*  -8..8 dB */
static int8_t center;		/* -16..0 dB */
static int8_t subwoofer;	/* -16..0 dB */

void setSpeakers(void)
{
	uint8_t i;

	for (i = 0; i < TDA7448_CHANNELS; i++)
		sp[i] = volume;

	if (balance > 0) {
		sp[TDA7448_SP_FRONTLEFT] -= balance;
		sp[TDA7448_SP_REARLEFT] -= balance;
	} else {
		sp[TDA7448_SP_FRONTRIGHT] += balance;
		sp[TDA7448_SP_REARRIGHT] += balance;
	}
	if (front > 0) {
		sp[TDA7448_SP_REARLEFT] -= front;
		sp[TDA7448_SP_REARRIGHT] -= front;
	} else {
		sp[TDA7448_SP_FRONTLEFT] += front;
		sp[TDA7448_SP_FRONTRIGHT] += front;
	}
	sp[TDA7448_SP_CENTER] += center;
	sp[TDA7448_SP_SUBWOOFER] += subwoofer;

	for (i = 0; i < TDA7448_CHANNELS; i++) {
		if (sp[i] < TDA7448_SP_MIN)
			sp[i] = TDA7448_SP_MIN;
	}

	for (i = 0; i < TDA7448_CHANNELS; i++) {
		// Write to I2C
	}

	return;
}
