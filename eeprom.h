#ifndef EEPROM_H
#define EEPROM_H

#include <inttypes.h>

#define eepromVolume		((uint8_t*)0x00)

#define eepromDispMode		((uint8_t*)0x02)
#define eepromBrWork		((uint8_t*)0x04)
#define eepromBrStby		((uint8_t*)0x05)

#define eepromEncoder		((uint8_t*)0x06)

#define eepromZeroHour		((uint8_t*)0x07)

#define eepromFMFreq		((uint16_t*)0x08)
#define eepromFMTuner		((void*)0x0A)
#define eepromFMMono		((void*)0x0C)
#define eepromFMStep		((void*)0x0D)
#define eepromFMCtrl		((void*)0x0E)

#define eepromStations		((uint16_t*)0x10)
#define eepromFavStations	((uint16_t*)0x78)

#endif /* EEPROM_H */
