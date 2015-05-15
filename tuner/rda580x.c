#include "rda580x.h"

#include "../i2c.h"

static uint8_t wrBuf[14];
static uint8_t rdBuf[5];
static uint8_t _volume = RDA5807_VOL_MAX;

static void rda580xWriteI2C(void)
{
	uint8_t i;

	I2CStart(RDA5807M_I2C_ADDR);
	for (i = 0; i < sizeof(wrBuf); i++)
		I2CWriteByte(wrBuf[i]);
	I2CStop();

	return;
}

void rda580xInit(freqMethod frMeth)
{
	wrBuf[0] = RDA5807_DHIZ;
	wrBuf[1] = RDA5807_CLK_MODE_32768 | RDA5807_NEW_METHOD | RDA5807_ENABLE;
	wrBuf[2] = 0;
	wrBuf[3] = RDA5807_BAND | RDA5807_SPACE;
	wrBuf[4] = 0;
	wrBuf[5] = 0;
	wrBuf[6] = 0b1000 & RDA5807_SEEKTH;
	wrBuf[7] = RDA5807_LNA_PORT_SEL | RDA5807_VOLUME;
	wrBuf[8] = 0;
	wrBuf[9] = 0;
	if (frMeth == RDA5807_DIRECT_FREQ) {
		wrBuf[10] = 0x80 & RDA5807_TH_SOFRBLEND;
		wrBuf[11] = RDA5807_FREQ_MODE;
		wrBuf[12] = 0;
		wrBuf[13] = 0;
	}

	rda580xWriteI2C();

	return;
}

void rda580xSetFreq(uint16_t freq, uint8_t mono, freqMethod frMeth)
{
	uint16_t chan;

	if (mono)
		wrBuf[0] |= RDA5807_MONO;
	else
		wrBuf[0] &= ~RDA5807_MONO;

	if (frMeth == RDA5807_DIRECT_FREQ) {
		wrBuf[12] = ((freq - 5000) * 10) >> 8;
		wrBuf[13] = ((freq - 5000) * 10) & 0xFF;
	} else {
		chan = (freq - RDA5807_FREQ_MIN) / RDA5807_CHAN_SPACING;
		wrBuf[2] = chan >> 2;								/* 8 MSB */
		wrBuf[3] &= 0x3F;
		wrBuf[3] |= RDA5807_TUNE | ((chan & 0x03) << 6);	/* 2 LSB */
	}

	rda580xWriteI2C();

	return;
}

uint8_t *rda580xReadStatus(void)
{
	uint8_t i;

	I2CStart(RDA5807M_I2C_ADDR | I2C_READ);
	for (i = 0; i < sizeof(rdBuf) - 1; i++)
		I2CReadByte(&rdBuf[i], I2C_ACK);
	I2CReadByte(&rdBuf[sizeof(rdBuf) - 1], I2C_NOACK);
	I2CStop();

	return rdBuf;
}

void rda580xSetMute(uint8_t mute)
{
	if (mute)
		wrBuf[0] &= ~RDA5807_DMUTE;
	else
		wrBuf[0] |= RDA5807_DMUTE;
	wrBuf[3] &= ~RDA5807_TUNE;

	rda580xWriteI2C();

	return;
}

void rda580xSetVolume(int8_t value)
{
	_volume = value;

	wrBuf[7] &= ~RDA5807_VOLUME;
	if (_volume)
		wrBuf[7] |= (_volume - 1);

	rda580xSetMute(!_volume);

	return;
}

void rda580xPowerOn(void)
{
	wrBuf[1] |= RDA5807_ENABLE;

	rda580xSetMute(0);

	return;
}

void rda580xPowerOff(void)
{
	wrBuf[1] &= ~RDA5807_ENABLE;

	rda580xSetMute(1);

	return;
}
