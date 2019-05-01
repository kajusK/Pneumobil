/*
    BUT pneumobil - Copyright (C) 2019 Jakub Kaderka.

    This file is part of BUT pneumobil.

    BUT pneumobil is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    BUT pneumobil is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * @file    rfm69.c
 * @brief   HopeRF RFM69 wireless transceiver driver
 *
 * Based on https://github.com/nayem-cosmic/RFM69-Library-AVR
 *
 * @addtogroup drivers
 * @{
 */

#include <ch.h>

#include "modules/log.h"
#include "drivers/spi.h"
#include "utils/assert.h"

#include "drivers/rfm69_registers.h"
#include "drivers/rfm69.h"

typedef enum {
    RF69_MODE_SLEEP,
    RF69_MODE_STANDBY,
    RF69_MODE_SYNTH,
    RF69_MODE_RX,
    RF69_MODE_TX,
} rfm69_mode_t;

/** Internal clock frequency in Hz */
#define RFM69_XO               32000000
/** Step width of synthetisher in Hz */
#define RFM69_FSTEP 61.03515625

/** Timeout for pooling operations */
#define RFM69_OP_TIMEOUT_MS 100
/** Maximum wait time for channel free detection */
#define RFM69_CSMA_TIMEOUT_MS 500

/** Assume channel activity if rssi is above this limit */
#define RFM69_CSMA_LIMIT_DBM -100

/** Amount of bytes that can be send in one frame (66 bytes buffer - len, id) */
#define RFM69_MAX_PAYLOAD_LEN 64

/** Currently selected rfm mode */
static rfm69_mode_t rfm69i_mode = RF69_MODE_STANDBY;
/** True for rfm69HW version */
static bool rfm69i_isHxxversion = false;
/** If true check if channel is empty before transmitting */
static bool rfm69i_csma_ca = false;

/**
 * Read register value
 *
 * @param addr  Register address (7th bit must be zero)
 * @return register value
 */
static uint8_t rfm69i_ReadReg(uint8_t addr)
{
    ASSERT_NOT(addr & 0x80);

    uint8_t rxbuf[2], txbuf[2];

    txbuf[0] = addr & 0x7f;
    txbuf[1] = 0x00;

    SPId_Select(SPID_RFM);
    spiExchange(SPId_GetDrv(SPID_RFM), 2, txbuf, rxbuf);
    SPId_Unselect(SPID_RFM);
    return rxbuf[1];
}

/**
 * Write value to register
 *
 * @param addr  Register address
 * @param value Value to be written
 */
static void rfm69i_WriteReg(uint8_t addr, uint8_t value)
{
    uint8_t txbuf[2];

    txbuf[0] = addr | 0x80;
    txbuf[1] = value;

    SPId_Select(SPID_RFM);
    spiSend(SPId_GetDrv(SPID_RFM), 2, txbuf);
    SPId_Unselect(SPID_RFM);
}

/**
 * Modify content of the selected bits in register
 *
 * register = (register & mask) | bits;
 *
 * @param addr  Register address
 * @param mask  Mask to be anded with the register value
 * @param bits  Value to be ored with the register value
 */
static inline void rfm69i_ModifyReg(uint8_t addr, uint8_t mask, uint8_t bits)
{
    uint8_t reg;
    reg = rfm69i_ReadReg(addr);
    rfm69i_WriteReg(addr, (reg & mask) | bits);
}

/**
 * Wait until flag is set in given register or timeout expires
 *
 * @param reg   register address
 * @param flag  flag to be checked (waits until read(reg) & flag is true)
 */
static void rfm69i_WaitForFlagSet(uint8_t reg, uint8_t flag)
{
    systime_t timeout;
    timeout = chTimeAddX(chVTGetSystemTime(), TIME_MS2I(RFM69_OP_TIMEOUT_MS));

    while ((rfm69i_ReadReg(reg) & flag) == 0 && chVTGetSystemTime() < timeout) {
        chThdSleepMilliseconds(1);
    }
}

/**
 * Control high power output (+20 dBm) mode of Hxx devices
 *
 * Before switching to receive mode, call with false
 *
 * @param on    Enable high power mode if true, else disable
 */
static void rfm69i_EnableHighPowerOutput(bool on)
{
    if (on == 1) {
        rfm69i_WriteReg(REG_TESTPA1, 0x5D);
        rfm69i_WriteReg(REG_TESTPA2, 0x7C);
    } else {
        rfm69i_WriteReg(REG_TESTPA1, 0x55);
        rfm69i_WriteReg(REG_TESTPA2, 0x70);
    }
}

/**
 * Switch to given rfm69 mode, wait until mode is switched if required
 *
 * When switching between tx and rx mode, high power control is handled for
 * RFM69HW
 *
 * @param mode  Required mode
 * @param wait  Wait for mode switch
 */
static void rfm69i_SetMode(rfm69_mode_t mode, bool wait)
{
    if (mode == rfm69i_mode) {
        return;
    }

    switch (mode)
    {
        case RF69_MODE_TX:
            rfm69i_ModifyReg(REG_OPMODE, 0xe3, RF_OPMODE_TRANSMITTER);
            if (rfm69i_isHxxversion) {
                rfm69i_EnableHighPowerOutput(true);
            }
            break;
        case RF69_MODE_RX:
            rfm69i_ModifyReg(REG_OPMODE, 0xe3, RF_OPMODE_RECEIVER);
            if (rfm69i_isHxxversion) {
                rfm69i_EnableHighPowerOutput(false);
            }
            break;
        case RF69_MODE_SYNTH:
            rfm69i_ModifyReg(REG_OPMODE, 0xe3, RF_OPMODE_SYNTHESIZER);
            break;
        case RF69_MODE_STANDBY:
            rfm69i_ModifyReg(REG_OPMODE, 0xe3, RF_OPMODE_STANDBY);
            break;
        case RF69_MODE_SLEEP:
            rfm69i_ModifyReg(REG_OPMODE, 0xe3, RF_OPMODE_SLEEP);
            break;
        default:
            return;
    }

    if (wait) {
        rfm69i_WaitForFlagSet(REG_IRQFLAGS1, RF_IRQFLAGS1_MODEREADY);
    }
    rfm69i_mode = mode;
}

void rfm69_SetPowerLevel(int8_t dBm)
{
    uint8_t powerLevel;

    if (!rfm69i_isHxxversion) {
        if (dBm < -18) {
            dBm = -18;
        } else if (dBm > 13) {
            dBm = 13;
        }
        rfm69i_WriteReg(REG_PALEVEL, RF_PALEVEL_PA1_ON | (18 + dBm));
        return;
    }

    if (dBm < -2) {
        dBm = -2;
    } else if (dBm > 20) {
        dBm = 20;
    }

    /* Turn off over current protection for high power output */
    rfm69i_WriteReg(REG_OCP, dBm > 17 ? RF_OCP_OFF : RF_OCP_ON);

    if (dBm <= 13) {
        powerLevel = 18 + dBm;
        rfm69i_WriteReg(REG_PALEVEL, RF_PALEVEL_PA1_ON);
    } else {
        powerLevel = 11 + dBm;
        rfm69i_WriteReg(REG_PALEVEL, RF_PALEVEL_PA1_ON | RF_PALEVEL_PA2_ON);
    }

    rfm69i_ModifyReg(REG_PALEVEL, 0xe0, powerLevel);

    Log_Info(LOG_SOURCE_DRIVER, "Setting RFM69 output power to %d dBm", dBm);
}

void rfm69_SetEncrypt(const uint8_t* key)
{
    uint8_t addr;
    rfm69i_SetMode(RF69_MODE_STANDBY, false);

    if (key != NULL) {
        addr = REG_AESKEY1 | 0x80;

        SPId_Select(SPID_RFM);
        spiSend(SPId_GetDrv(SPID_RFM), 1, &addr);
        spiSend(SPId_GetDrv(SPID_RFM), 16, key);
        SPId_Unselect(SPID_RFM);
        Log_Info(LOG_SOURCE_DRIVER, "Enabling RFM69 encryption");
    } else {
        Log_Info(LOG_SOURCE_DRIVER, "Disabling RFM69 encryption");
    }
    rfm69i_ModifyReg(REG_PACKETCONFIG2, 0xfe, key != NULL ? 1 : 0);
}

int16_t rfm69i_GetRSSIdBm(void)
{
    return -(rfm69i_ReadReg(REG_RSSIVALUE) >> 1);
}

uint32_t rfm69_GetFrequencyHz(void)
{
    return RFM69_FSTEP * (((uint32_t) rfm69i_ReadReg(REG_FRFMSB) << 16) +
            ((uint16_t) rfm69i_ReadReg(REG_FRFMID) << 8) + rfm69i_ReadReg(REG_FRFLSB));
}

void rfm69_SetFrequencyHz(uint32_t freq_Hz)
{
    uint32_t value = freq_Hz / RFM69_FSTEP;

    rfm69i_SetMode(RF69_MODE_STANDBY, false);

    rfm69i_WriteReg(REG_FRFMSB, value >> 16);
    rfm69i_WriteReg(REG_FRFMID, value >> 8);
    rfm69i_WriteReg(REG_FRFLSB, value);

    Log_Info(LOG_SOURCE_DRIVER, "Setting RFM69 frequency to %d Hz", freq_Hz);
}

void rfm69_SetBitrate(uint32_t bitrate)
{
    uint32_t value = RFM69_XO / bitrate;

    rfm69i_SetMode(RF69_MODE_STANDBY, false);

    rfm69i_WriteReg(REG_BITRATEMSB, value >> 8);
    rfm69i_WriteReg(REG_BITRATELSB, value);
}

void rfm69_EnablePromiscuous(bool enable)
{
    if (enable) {
        rfm69i_ModifyReg(REG_PACKETCONFIG1, 0xf9, RF_PACKET1_ADRSFILTERING_OFF);
    } else {
        rfm69i_ModifyReg(REG_PACKETCONFIG1, 0xf9, RF_PACKET1_ADRSFILTERING_NODEBROADCAST);
    }
}

void rfm69_Send(uint8_t dest, const void* buffer, uint8_t len)
{
    systime_t timeout;
    uint8_t byte;

    ASSERT_NOT(buffer == NULL || len == 0);

    if (len > RFM69_MAX_PAYLOAD_LEN) {
        len = RFM69_MAX_PAYLOAD_LEN;
    }

    /* clear fifo */
    rfm69i_WriteReg(REG_IRQFLAGS2, RF_IRQFLAGS2_FIFOOVERRUN);

    /* Check if channel is empty before transmitting if required */
    if (rfm69i_csma_ca) {
        rfm69i_SetMode(RF69_MODE_RX, false);

        /* Wait for channel to become free */
        timeout = chTimeAddX(chVTGetSystemTime(), TIME_MS2I(RFM69_OP_TIMEOUT_MS));
        do {
            /* Restart receiving */
            rfm69i_ModifyReg(REG_PACKETCONFIG2, 0xfb, RF_PACKET2_RXRESTART);
            rfm69i_WaitForFlagSet(REG_RSSICONFIG, RF_RSSI_DONE);
        } while (rfm69i_GetRSSIdBm() > RFM69_CSMA_LIMIT_DBM && chVTGetSystemTime() < timeout);
    }

    /* Fill the fifo and send frame */
    rfm69i_SetMode(RF69_MODE_STANDBY, false);

    SPId_Select(SPID_RFM);
    byte = REG_FIFO | 0x80;
    spiSend(SPId_GetDrv(SPID_RFM), 1, &byte);
    spiSend(SPId_GetDrv(SPID_RFM), 1, &len);
    spiSend(SPId_GetDrv(SPID_RFM), 1, &dest);
    spiSend(SPId_GetDrv(SPID_RFM), len, buffer);
    SPId_Unselect(SPID_RFM);

    rfm69i_SetMode(RF69_MODE_TX, false);
    /* Wait for packet to be sent */
    rfm69i_WaitForFlagSet(REG_IRQFLAGS2, RF_IRQFLAGS2_PACKETSENT);

    rfm69i_SetMode(RF69_MODE_STANDBY, false);
}

uint8_t rfm69_Receive(uint8_t *buffer, uint8_t max_len)
{
    uint8_t len, addr, byte;

    rfm69i_SetMode(RF69_MODE_RX, false);

    if ((rfm69i_ReadReg(REG_IRQFLAGS2) & RF_IRQFLAGS2_PAYLOADREADY) == 0) {
        return 0;
    }

    rfm69i_SetMode(RF69_MODE_STANDBY, false);

    SPId_Select(SPID_RFM);
    byte = REG_FIFO;
    spiSend(SPId_GetDrv(SPID_RFM), 1, &byte);
    spiReceive(SPId_GetDrv(SPID_RFM), 1, &len);
    spiReceive(SPId_GetDrv(SPID_RFM), 1, &addr);

    if (len > max_len) {
        len = max_len;
    }
    if (len > RFM69_MAX_PAYLOAD_LEN) {
        len = RFM69_MAX_PAYLOAD_LEN;
    }
    spiReceive(SPId_GetDrv(SPID_RFM), len, buffer);
    SPId_Unselect(SPID_RFM);

    rfm69i_SetMode(RF69_MODE_RX, false);

    return len;
}

bool rfm69_Init(uint8_t nodeID, uint8_t networkID, bool isHxx, bool csma_ca)
{
    systime_t timeout;

    /* do sync, fail if device not responding for longer time */
    timeout = chTimeAddX(chVTGetSystemTime(), TIME_MS2I(RFM69_OP_TIMEOUT_MS));
    while (rfm69i_ReadReg(REG_SYNCVALUE1) != 0xaa) {
        if (chVTGetSystemTime() > timeout) {
            Log_Error(LOG_SOURCE_DRIVER, "RFM69 init not responding");
            return false;
        }
        rfm69i_WriteReg(REG_SYNCVALUE1, 0xaa);
    }
    while (rfm69i_ReadReg(REG_SYNCVALUE1) != 0x55) {
        if (chVTGetSystemTime() > timeout) {
            Log_Error(LOG_SOURCE_DRIVER, "RFM69 not syncing to 0x55");
            return false;
        }
        rfm69i_WriteReg(REG_SYNCVALUE1, 0x55);
    }

    rfm69i_WriteReg(REG_OPMODE, RF_OPMODE_SEQUENCER_ON | RF_OPMODE_LISTEN_OFF | RF_OPMODE_STANDBY);
    rfm69i_WriteReg(REG_DATAMODUL, RF_DATAMODUL_DATAMODE_PACKET | RF_DATAMODUL_MODULATIONTYPE_FSK | RF_DATAMODUL_MODULATIONSHAPING_00);
    rfm69i_WriteReg(REG_BITRATEMSB, RF_BITRATEMSB_9600);
    rfm69i_WriteReg(REG_BITRATELSB, RF_BITRATELSB_9600);
    rfm69i_WriteReg(REG_FDEVMSB, RF_FDEVMSB_50000);
    rfm69i_WriteReg(REG_FDEVLSB, RF_FDEVLSB_50000);
    rfm69i_WriteReg(REG_FRFMSB, RF_FRFMSB_868);
    rfm69i_WriteReg(REG_FRFMID, RF_FRFMID_868);
    rfm69i_WriteReg(REG_FRFLSB, RF_FRFLSB_868);
    rfm69i_WriteReg(REG_PALEVEL, RF_PALEVEL_PA0_OFF | RF_PALEVEL_PA1_ON | RF_PALEVEL_PA2_ON | RF_PALEVEL_OUTPUTPOWER_11111);
    rfm69i_WriteReg(REG_OCP, RF_OCP_ON | RF_OCP_TRIM_95);
    rfm69i_WriteReg(REG_RXBW, RF_RXBW_DCCFREQ_010 | RF_RXBW_MANT_16 | RF_RXBW_EXP_2);
    rfm69i_WriteReg(REG_DIOMAPPING1, RF_DIOMAPPING1_DIO0_01);
    rfm69i_WriteReg(REG_DIOMAPPING2, RF_DIOMAPPING2_CLKOUT_OFF);
    rfm69i_WriteReg(REG_IRQFLAGS2, RF_IRQFLAGS2_FIFOOVERRUN);
    rfm69i_WriteReg(REG_RSSITHRESH, 220);
    rfm69i_WriteReg(REG_SYNCCONFIG, RF_SYNC_ON | RF_SYNC_FIFOFILL_AUTO | RF_SYNC_SIZE_2 | RF_SYNC_TOL_0);
    rfm69i_WriteReg(REG_SYNCVALUE1, 0x2D);
    rfm69i_WriteReg(REG_SYNCVALUE2, networkID);
    rfm69i_WriteReg(REG_PACKETCONFIG1, RF_PACKET1_FORMAT_VARIABLE | RF_PACKET1_DCFREE_OFF | RF_PACKET1_CRC_ON | RF_PACKET1_CRCAUTOCLEAR_ON | RF_PACKET1_ADRSFILTERING_NODEBROADCAST);
    rfm69i_WriteReg(REG_PAYLOADLENGTH, 66);
    rfm69i_WriteReg(REG_NODEADRS, nodeID);
    rfm69i_WriteReg(REG_BROADCASTADRS, 0xff);
    rfm69i_WriteReg(REG_FIFOTHRESH, RF_FIFOTHRESH_TXSTART_FIFONOTEMPTY | RF_FIFOTHRESH_VALUE);
    rfm69i_WriteReg(REG_PACKETCONFIG2, RF_PACKET2_RXRESTARTDELAY_2BITS | RF_PACKET2_AUTORXRESTART_ON | RF_PACKET2_AES_OFF);
    rfm69i_WriteReg(REG_TESTDAGC, RF_DAGC_IMPROVED_LOWBETA0);

    rfm69i_isHxxversion = isHxx;
    rfm69i_csma_ca = csma_ca;

    /* no encryption, persists over reboots, start with none... */
    rfm69_SetEncrypt(NULL);
    rfm69i_SetMode(RF69_MODE_STANDBY, false);

    /* Turn off over current protection for hxx version */
    rfm69i_WriteReg(REG_OCP, isHxx ? RF_OCP_OFF : RF_OCP_ON);
    rfm69_SetPowerLevel(20);

    return true;
}

/** @} */

