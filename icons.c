#include <avr/pgmspace.h>
const uint8_t icons[] PROGMEM = {
	// Volume
	0x10, 0x18, 0x1C, 0x1E, 0x1F,
	// Bass
	0x1F, 0x1E, 0x1C, 0x1C, 0x1C,
	// Middle
	0x1C, 0x1E, 0x1F, 0x1E, 0x1C,
	// Treble
	0x1C, 0x1C, 0x1C, 0x1E, 0x1F,
	// Preamp
	0x1F, 0x1F, 0x0E, 0x04, 0x00,
	// Frontrear
	0x11, 0x1B, 0x1F, 0x1B, 0x11,
	// Balance
	0x1F, 0x0E, 0x04, 0x0E, 0x1F,
	// Center
	0x04, 0x07, 0x07, 0x07, 0x04,
	// Subwoofer
	0x0C, 0x0F, 0x0F, 0x0F, 0x0C,

	// Tuner
	0x1C, 0x1F, 0x1C, 0x1C, 0x1C,
	// PC
	0x17, 0x15, 0x1D, 0x15, 0x17,
	// TV
	0x1E, 0x13, 0x12, 0x12, 0x1E,
	// DVD
	0x0E, 0x1F, 0x1B, 0x1F, 0x0E,
	// Bluetooth
	0x00, 0x00, 0x1F, 0x0A, 0x00,
	// USB
	0x03, 0x04, 0x1F, 0x08, 0x06,

	// Mute
	0x11, 0x0A, 0x04, 0x0A, 0x11,
	// Loudness
	0x1F, 0x1E, 0x1C, 0x1E, 0x1F,

	// Brightness
	0x0E, 0x1F, 0x1F, 0x1F, 0x0E,
};
