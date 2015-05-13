#ifndef EEPROM_H
#define EEPROM_H

#include <inttypes.h>

#define eepromVolume		((uint8_t*)0x00)

#define eepromDispMode		((uint8_t*)0x02)
#define eepromBrWork		((uint8_t*)0x04)
#define eepromBrStby		((uint8_t*)0x05)

#define eepromEncRes		((uint8_t*)0x06)

#define eepromZeroHour		((uint8_t*)0x07)

#define eepromFMFreq		((uint16_t*)0x10)
#define eepromFMFreqMin		((uint16_t*)0x14)
#define eepromFMFreqMax		((uint16_t*)0x16)

#define eepromFMTuner		((void*)0x1A)
#define eepromFMMono		((void*)0x1C)
#define eepromFMStep		((void*)0x1D)
#define eepromFMCtrl		((void*)0x1E)

#define eepromStations		((uint16_t*)0x20)
#define eepromFavStations	((uint16_t*)0x88)

#endif /* EEPROM_H */
