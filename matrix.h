#ifndef MATRIX_H
#define MATRIX_H

#include <inttypes.h>
#include "pins.h"

#define ROWS		16

void matrixInit(void);
void matrixClear(void);

void showVolume(int8_t value);
void showBalance(int8_t value);
void showFront(int8_t value);
void showCenter(int8_t value);
void showSubwoofer(int8_t value);

#endif /* MATRIX_H */
