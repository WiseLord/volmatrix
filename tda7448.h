#ifndef TDA7448_H
#define TDA7448_H

#include <inttypes.h>

enum {
	TDA7448_SP_FRONTLEFT,
	TDA7448_SP_FRONTRIGHT,
	TDA7448_SP_REARLEFT,
	TDA7448_SP_REARRIGHT,
	TDA7448_SP_CENTER,
	TDA7448_SP_SUBWOOFER
};

#define TDA7448_CHANNELS	6

#define TDA7448_SP_MIN		-79
#define TDA7448_SP_MAX		0

#define TDA7448_BAL_MIN		-8
#define TDA7448_BAL_MAX		8

#define TDA7448_FRONT_MIN	-8
#define TDA7448_FRONT_MAX	8

#define TDA7448_CENTER_MIN	-16
#define TDA7448_CENTER_MAX	0

#define TDA7448_SUB_MIN		-16
#define TDA7448_SUB_MAX		0

void setSpeakers(void);

void incVolume(void);
void decVolume(void);
int8_t getVolume(void);

void incBalance(void);
void decBalance(void);
int8_t getBalance(void);

void incFront(void);
void decFront(void);
int8_t getFront(void);

void incCenter(void);
void decCenter(void);
int8_t getCenter(void);

void incSubwoofer(void);
void decSubwoofer(void);
int8_t getSubwoofer(void);

#endif /* TDA7448_H */
