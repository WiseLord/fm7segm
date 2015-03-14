#ifndef EEPROM_H
#define EEPROM_H

#include <inttypes.h>

#define eepromVolume		((uint8_t*)0x00)

#define eepromDispMode		((uint8_t*)0x02)
#define eepromBrWork		((uint8_t*)0x04)
#define eepromBrStby		((uint8_t*)0x05)
#define eepromEncoder		((uint8_t*)0x06)

#define eepromFMFreq		((uint16_t*)0x08)
#define eepromFMMono		((void*)0x0C)
#define eepromFMStep		((void*)0x0D)
#define eepromFMCtrl		((void*)0x0E)

#define eepromStations		((uint16_t*)0x10)

#endif /* EEPROM_H */
