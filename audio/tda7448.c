#include "tda7448.h"

#include <avr/pgmspace.h>
#include "../i2c.h"

static sndParam *sndPar;
static uint8_t _input;

void tda7448Init(sndParam *sp)
{
	sndPar = sp;

	return;
}

void tda7448SetSpeakers(int8_t val)
{
	uint8_t i;
	uint8_t i2cData;
	int8_t sp[TDA7448_LINES_COUNT];
	int8_t volMin = pgm_read_byte(&sndPar[MODE_SND_VOLUME].grid->min);

	for (i = 0; i < TDA7448_LINES_COUNT; i++)
		sp[i] = sndPar[MODE_SND_VOLUME].value;

	if (sndPar[MODE_SND_BALANCE].value > 0) {
		sp[TDA7448_FRONT_LEFT] -= sndPar[MODE_SND_BALANCE].value;
		sp[TDA7448_REAR_LEFT] -= sndPar[MODE_SND_BALANCE].value;
	} else {
		sp[TDA7448_FRONT_RIGHT] += sndPar[MODE_SND_BALANCE].value;
		sp[TDA7448_REAR_RIGHT] += sndPar[MODE_SND_BALANCE].value;
	}

	if (sndPar[MODE_SND_FRONTREAR].value > 0) {
		sp[TDA7448_REAR_LEFT] -= sndPar[MODE_SND_FRONTREAR].value;
		sp[TDA7448_REAR_RIGHT] -= sndPar[MODE_SND_FRONTREAR].value;
	} else {
		sp[TDA7448_FRONT_LEFT] += sndPar[MODE_SND_FRONTREAR].value;
		sp[TDA7448_FRONT_RIGHT] += sndPar[MODE_SND_FRONTREAR].value;
	}

	sp[TDA7448_CENTER] += sndPar[MODE_SND_CENTER].value;
	sp[TDA7448_SUBWOOFER] += sndPar[MODE_SND_SUBWOOFER].value;

	I2CStart(TDA7448_I2C_ADDR);
	I2CWriteByte(TDA7448_AUTO_INC);
	for (i = 0; i < TDA7448_LINES_COUNT; i++) {
		/* Limit values sent to bus */
		if (sp[i] < volMin)
			sp[i] = volMin;
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
		tda7448SetSpeakers(0);
	}

	return;
}

void tda7448MixSetSpeakers(int8_t val)
{
	uint8_t i;
	uint8_t i2cData;
	int8_t sp[TDA7448_LINES_COUNT];
	int8_t volMin = pgm_read_byte(&sndPar[MODE_SND_VOLUME].grid->min);

	uint8_t left, right;

	for (i = 0; i < TDA7448_LINES_COUNT; i++)
		sp[i] = sndPar[MODE_SND_VOLUME].value;

	switch (_input) {
	case 0:
		left = TDA7448_FRONT_LEFT;
		right = TDA7448_FRONT_RIGHT;
		break;
	case 1:
		left = TDA7448_REAR_LEFT;
		right = TDA7448_REAR_RIGHT;
		break;
	default:
		left = TDA7448_CENTER;
		right = TDA7448_SUBWOOFER;
		break;
	}

	if (sndPar[MODE_SND_BALANCE].value > 0)
		sp[left] -= sndPar[MODE_SND_BALANCE].value;
	else
		sp[right] += sndPar[MODE_SND_BALANCE].value;

	sp[left] += sndPar[MODE_SND_GAIN0 + _input].value;
	sp[right] += sndPar[MODE_SND_GAIN0 + _input].value;

	I2CStart(TDA7448_I2C_ADDR);
	I2CWriteByte(TDA7448_AUTO_INC);
	for (i = 0; i < TDA7448_LINES_COUNT; i++) {
		if (i == left || i == right) {
			/* Limit values sent to bus */
			if (sp[i] < volMin)
				sp[i] = volMin;
			i2cData = -sp[i];
			/* Jump at -72db in raw data according the datasheet */
			if (i2cData >= 72)
				i2cData += 56;
		} else {
			i2cData = TDA7448_MUTE;
		}
		I2CWriteByte(i2cData);
	}
	I2CStop();

	return;
}

void tda7448MixSetMute(int8_t val)
{
	uint8_t i;

	if (val == MUTE_ON) {
		I2CStart(TDA7448_I2C_ADDR);
		I2CWriteByte(TDA7448_AUTO_INC);
		for (i = 0; i < TDA7448_LINES_COUNT; i++)
			I2CWriteByte(TDA7448_MUTE);
		I2CStop();
	} else {
		tda7448MixSetSpeakers(0);
	}

	return;
}

void tda7448MixSetInput(uint8_t in)
{
	_input = in;
	tda7448MixSetSpeakers(0);

	return;
}
