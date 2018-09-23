/*
    BUT pneumobil - Copyright (C) 2018 Jakub Kaderka.

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
 * @file    i2c.c
 * @brief   I2C driver
 *
 * @addtogroup drivers
 * @{
 */

#include <hal.h>

#include "utils/assert.h"
#include "drivers/i2c.h"

/** Timeout for i2c transmit command */
#define I2CD_TIMEOUT_MS 100

bool I2Cd_Transceive(uint8_t address, const uint8_t *txbuf, size_t txbytes,
        uint8_t *rxbuf, size_t rxbytes)
{
    msg_t res;

    ASSERT_NOT(txbuf == NULL || txbytes == 0);

#if I2C_USE_MUTUAL_EXCLUSION
    i2cAcquireBus(&I2CD1);
#endif
    //TODO
    res = i2cMasterTransmitTimeout(&I2CD1, address, txbuf, txbytes,
            rxbuf, rxbytes, TIME_MS2I(I2CD_TIMEOUT_MS));
#if I2C_USE_MUTUAL_EXCLUSION
    i2cReleaseBus(&I2CD1);
#endif

    if (res != MSG_OK) {
        return false;
    }
    return true;
}

void I2Cd_Init(bool full_speed)
{
    I2CConfig config;

    config.cr1 = 0;
    config.cr2 = 0;

    /* TODO fix timing */
    if (full_speed) {
        config.timingr = 0;
    } else {
        config.timingr = 0;
    }

    i2cStart(&I2CD1, &config);
}

/** @} */
