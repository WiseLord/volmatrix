#ifndef EEPROM_H
#define EEPROM_H

/* Parameters values */
#define eepromVolume			((void*)0x00)
#define eepromBalance			((void*)0x01)
#define eepromFrontRear			((void*)0x02)
#define eepromCenter			((void*)0x03)
#define eepromSubwoofer			((void*)0x04)

/* RC5 address and command */
#define eepromRC5Addr			((void*)0x10)
#define eepromRC5Cmd			((uint8_t*)0x20)

#endif /* EEPROM_H */

