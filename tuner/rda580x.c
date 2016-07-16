#include "rda580x.h"

#include "../i2c.h"
#ifdef _RDS
#include "rds.h"
#endif

static uint8_t wrBuf[14];
static uint8_t rdBuf[12];
static uint8_t _volume = RDA5807_VOL_MAX;
static rda580xIC _ic;

static void rda580xWriteI2C(uint8_t bytes)
{
	uint8_t i;

	if (!bytes)
		bytes = sizeof(wrBuf);

	I2CStart(RDA5807M_I2C_ADDR);
	for (i = 0; i < bytes; i++)
		I2CWriteByte(wrBuf[i]);
	I2CStop();

	return;
}

void rda580xInit(rda580xIC ic)
{
	_ic = ic;

	wrBuf[0] = RDA5807_DHIZ;
	wrBuf[1] = RDA5807_CLK_MODE_32768 | RDA5807_RDS_EN | RDA5807_NEW_METHOD | RDA5807_ENABLE;
	wrBuf[2] = 0;
	wrBuf[3] = RDA5807_BAND_EASTEUROPE | RDA5807_SPACE_50;
	wrBuf[4] = 0;
	wrBuf[5] = 0;
	wrBuf[6] = 0b1000 & RDA5807_SEEKTH;
	wrBuf[7] = RDA5807_LNA_PORT_SEL | RDA5807_VOLUME;
	wrBuf[8] = 0;
	wrBuf[9] = 0;

	if (_ic == RDA580X_RDA5807_DF) {
		wrBuf[10] = 0x80 & RDA5807_TH_SOFRBLEND;
		wrBuf[11] = RDA5807_FREQ_MODE;
	} else {
		wrBuf[10] = (0x80 & RDA5807_TH_SOFRBLEND) | RDA5807_65M_50M_MODE;
		wrBuf[11] = 0;
	}
	wrBuf[12] = 0;
	wrBuf[13] = 0;

	rda580xWriteI2C(0);

	return;
}

void rda580xSetFreq(uint16_t freq, uint8_t mono)
{
	uint16_t chan;

	if (mono)
		wrBuf[0] |= RDA5807_MONO;
	else
		wrBuf[0] &= ~RDA5807_MONO;

	if (_ic == RDA580X_RDA5807_DF) {
		wrBuf[12] = ((freq - 5000) * 10) >> 8;
		wrBuf[13] = ((freq - 5000) * 10) & 0xFF;
	} else {
		chan = (freq - 6500) / RDA5807_CHAN_SPACING;
		wrBuf[2] = chan >> 2;								/* 8 MSB */
		wrBuf[3] &= 0x3F;
		wrBuf[3] |= RDA5807_TUNE | ((chan & 0x03) << 6);	/* 2 LSB */
	}

	rda580xWriteI2C(0);

#ifdef _RDS
	rdsDisable();
#endif

	return;
}

uint8_t *rda580xReadStatus(void)
{
	uint8_t i;

	I2CStart(RDA5807M_I2C_ADDR | I2C_READ);
	for (i = 0; i < sizeof(rdBuf) - 1; i++)
		rdBuf[i] = I2CReadByte(I2C_ACK);
	rdBuf[sizeof(rdBuf) - 1] = I2CReadByte(I2C_NOACK);
	I2CStop();

	// Get RDS data
#ifdef _RDS
	/* If seek/tune is complete and current channel is a station */
	if ((rdBuf[0] & RDA5807_STC) && (rdBuf[2] & RDA5807_FM_TRUE)) {
		/* If RDS ready and sync flag are set */
		if ((rdBuf[0] & RDA5807_RDSR) && (rdBuf[0] & RDA5807_RDSS)) {
			/* If there are no errors in blocks A and B */
			if (!(rdBuf[3] & (RDA5807_BLERA | RDA5807_BLERB))) {
				/* Send rdBuf[4..11] as 16-bit blocks A-D */
				rdsSetBlocks(&rdBuf[4]);
			}
		}
	}
#endif

	return rdBuf;
}

void rda580xSetMute(uint8_t mute)
{
	if (mute)
		wrBuf[0] &= ~RDA5807_DMUTE;
	else
		wrBuf[0] |= RDA5807_DMUTE;
	wrBuf[3] &= ~RDA5807_TUNE;

	rda580xWriteI2C(8);

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

	rda580xSetMute(1);

	return;
}

void rda580xPowerOff(void)
{
	wrBuf[1] &= ~RDA5807_ENABLE;

	rda580xSetMute(1);

	return;
}
