#include <avr/pgmspace.h>
const uint8_t icons[] PROGMEM = {
	// Volume
	0x10, 0x18, 0x1C, 0x1E, 0x1F,
	// Bass
	0x1F, 0x1C, 0x1C, 0x1C, 0x1C,
	// Middle
	0x1C, 0x1C, 0x1F, 0x1C, 0x1C,
	// Treble
	0x1C, 0x1C, 0x1C, 0x1C, 0x1F,
	// Preamp
	0x00, 0x1F, 0x0E, 0x04, 0x00,
	// Frontrear
	0x11, 0x1B, 0x1F, 0x1B, 0x11,
	// Balance
	0x1F, 0x0E, 0x04, 0x0E, 0x1F,
	// Center
	0x04, 0x07, 0x07, 0x07, 0x04,
	// Subwoofer
	0x0C, 0x0F, 0x0F, 0x0F, 0x0C,

	// Tuner
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	// PC
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	// TV
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	// DVD
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	// Bluetooth
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	// USB
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	// Brightness
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	// Threshold
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF,

	// Mute
	0x11, 0x0A, 0x04, 0x0A, 0x11,
	// Loudness
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
};
