#ifndef TUNER_H
#define TUNER_H

#include <inttypes.h>

#include "tea5767.h"

#define FM_COUNT		64

#define SEARCH_DOWN		0
#define SEARCH_UP		1

#define FM_MONO			1
#define FM_STEREO		0

#define FM_FREQ_MIN		8750
#define FM_FREQ_MAX		10800

void tunerInit(void);
void tunerSetFreq(uint16_t freq);

uint16_t tunerGetFreq();

void tunerIncFreq(void);
void tunerDecFreq(void);

uint8_t stationNum(uint16_t freq);
void scanStoredFreq(uint8_t direction);
void loadStation(uint8_t num);
void storeStation(void);

void loadTunerParams(void);
void saveTunerParams(void);

#endif /* TUNER_H */
