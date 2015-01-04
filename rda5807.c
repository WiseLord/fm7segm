#include "rda5807.h"

#include "i2c.h"

#define CHAN_SPACING 5

static uint8_t wrBuf[5];
static uint8_t rdBuf[5];

static void rda5807WriteI2C(void)
{
	uint8_t i;

	I2CStart(RDA5807M_ADDR);
	for (i = 0; i < sizeof(wrBuf); i++)
		I2CWriteByte(wrBuf[i]);
	I2CStop();

	return;
}

static void rda5807ReadI2C(void)
{
	uint8_t i;

	I2CStart(RDA5807M_ADDR | I2C_READ);
	for (i = 0; i < sizeof(rdBuf) - 1; i++)
		I2CReadByte(&rdBuf[i], I2C_ACK);
	I2CReadByte(&rdBuf[sizeof(rdBuf) - 1], I2C_NOACK);
	I2CStop();

	return;
}

void rda5807Init(uint8_t mono)
{
	wrBuf[0] = RDA5807_DHIZ | RDA5807_DMUTE;
	if (mono)
		wrBuf[0] |= RDA5807_MONO;
	wrBuf[1] = RDA5807_CLK_MODE_32768 | RDA5807_NEW_METHOD | RDA5807_ENABLE;
	wrBuf[2] = 0;
	wrBuf[3] = (RDA5807_TUNE | RDA5807_BAND_US_EUROPE | RDA5807_SPACE_50);
	wrBuf[4] = RDA5807_SOFTMUTE_EN;

	rda5807WriteI2C();

	return;
}

void rda5807SetFreq(uint16_t freq)
{
	if (freq > RDA5807_FREQ_MAX)
		freq = RDA5807_FREQ_MAX;
	if (freq < RDA5807_FREQ_MIN)
		freq = RDA5807_FREQ_MIN;

	uint16_t chan = (freq - RDA5807_FREQ_MIN) / CHAN_SPACING;

	wrBuf[2] = chan >> 2;					/* 8 MSB */

	wrBuf[3] &= 0x3F;
	wrBuf[3] |= (chan & 0x03) << 6;			/* 2 LSB */

	rda5807WriteI2C();

	return;
}

uint8_t *rda5807ReadStatus(void)
{
	rda5807ReadI2C();

	return rdBuf;
}
