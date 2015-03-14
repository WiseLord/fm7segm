#ifndef VOLUME_H
#define VOLUME_H

#include "pins.h"
#include "tuner.h"

#if defined(RDA5807)
#define VOL_MIN				RDA5807_VOL_MIN
#define VOL_MAX				RDA5807_VOL_MAX
#else
#define VOL_MIN				0
#define VOL_MAX				16
#endif

void volumeInit(void);

void setVolume(int8_t value);
int8_t getVolume(void);

void muteVolume(void);
void unmuteVolume(void);

void volumeLoadParams(void);
void volumeSaveParams(void);

#endif /* VOLUME_H */
