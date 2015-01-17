#ifndef PINS_H
#define PINS_H

#include <avr/io.h>

#define CONCAT(x,y)			x ## y

#define DDR(x)				CONCAT(DDR,x)
#define PORT(x)				CONCAT(PORT,x)
#define PIN(x)				CONCAT(PIN,x)

typedef struct {
	volatile uint8_t *port;
	uint8_t mask;
} avrPort;

/* Matrix ports */
#define ROW_01				B
#define ROW_01_LINE		(1<<5)
#define ROW_02				B
#define ROW_02_LINE		(1<<2)
#define ROW_03				B
#define ROW_03_LINE		(1<<4)
#define ROW_04				B
#define ROW_04_LINE		(1<<1)
#define ROW_05				C
#define ROW_05_LINE		(1<<1)
#define ROW_06				B
#define ROW_06_LINE		(1<<3)
#define ROW_07				C
#define ROW_07_LINE		(1<<2)
#define ROW_08				C
#define ROW_08_LINE		(1<<0)

#define ROW_09				D
#define ROW_09_LINE		(1<<1)
#define ROW_10				D
#define ROW_10_LINE		(1<<0)
#define ROW_11				D
#define ROW_11_LINE		(1<<2)
#define ROW_12				C
#define ROW_12_LINE		(1<<3)
#define ROW_13				B
#define ROW_13_LINE		(1<<6)
#define ROW_14				D
#define ROW_14_LINE		(1<<3)
#define ROW_15				D
#define ROW_15_LINE		(1<<4)
#define ROW_16				B
#define ROW_16_LINE		(1<<7)

#define REG_DATA			B
#define REG_DATA_LINE	(1<<0)
#define REG_CLK				D
#define REG_CLK_LINE	(1<<7)

#define BUTTON				D
#define BUTTON_LINE		(1<<5)

#endif /* PINS_H */
