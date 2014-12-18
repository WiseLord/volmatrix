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

void incVolume(void)
{
	volume++;

	if (volume > TDA7448_SP_MAX)
		volume = TDA7448_SP_MAX;

	setSpeakers();

	return;
}

void decVolume(void)
{
	volume--;

	if (volume < TDA7448_SP_MIN)
		volume = TDA7448_SP_MIN;

	setSpeakers();

	return;
}

int8_t getVolume(void)
{
	return volume;
}

void incBalance(void)
{
	balance++;

	if (balance > TDA7448_BAL_MAX)
		balance = TDA7448_BAL_MAX;

	setSpeakers();

	return;
}

void decBalance(void)
{
	balance--;

	if (balance < TDA7448_BAL_MIN)
		balance = TDA7448_BAL_MIN;

	setSpeakers();

	return;
}

int8_t getBalance(void)
{
	return volume;
}

void incFront(void)
{
	front++;

	if (front > TDA7448_FRONT_MAX)
		front = TDA7448_FRONT_MAX;

	setSpeakers();

	return;
}

void decFront(void)
{
	front--;

	if (front < TDA7448_FRONT_MIN)
		front = TDA7448_FRONT_MIN;

	setSpeakers();

	return;
}

int8_t getFront(void)
{
	return front;
}

void incCenter(void)
{
	center++;

	if (center > TDA7448_CENTER_MAX)
		center = TDA7448_CENTER_MAX;

	setSpeakers();

	return;
}

void decCenter(void)
{
	center--;

	if (center < TDA7448_CENTER_MIN)
		center = TDA7448_CENTER_MIN;

	setSpeakers();

	return;
}

int8_t getCenter(void)
{
	return center;
}

void incSubwoofer(void)
{
	subwoofer++;

	if (subwoofer > TDA7448_SUB_MAX)
		subwoofer = TDA7448_SUB_MAX;

	setSpeakers();

	return;
}

void decSubwoofer(void)
{
	subwoofer--;

	if (subwoofer < TDA7448_SUB_MIN)
		subwoofer = TDA7448_SUB_MIN;

	setSpeakers();

	return;
}

int8_t getSubwoofer(void)
{
	return subwoofer;
}
