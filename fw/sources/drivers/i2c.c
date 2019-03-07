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

static I2CConfig i2cdi_config;

#ifndef STM32F4XX
static uint32_t I2Cdi_CalcTiming(bool full_speed)
{
    uint32_t retval;

#if STM32_I2C1CLK != 8000000
    #error "I2C timing parameters defined only for 8 MHz"
#endif
    if (full_speed) {
        retval = 0x00 << I2C_TIMINGR_PRESC_Pos;
        retval |= 0x9 << I2C_TIMINGR_SCLL_Pos;
        retval |= 0x3 << I2C_TIMINGR_SCLH_Pos;
        retval |= 0x1 << I2C_TIMINGR_SDADEL_Pos;
        retval |= 0x3 << I2C_TIMINGR_SCLDEL_Pos;
    } else {
        retval = 0x01 << I2C_TIMINGR_PRESC_Pos;
        retval |= 0x13 << I2C_TIMINGR_SCLL_Pos;
        retval |= 0xf << I2C_TIMINGR_SCLH_Pos;
        retval |= 0x2 << I2C_TIMINGR_SDADEL_Pos;
        retval |= 0x4 << I2C_TIMINGR_SCLDEL_Pos;
    }

    return retval;
}
#endif

bool I2Cd_Transceive(uint8_t address, const uint8_t *txbuf, size_t txbytes,
        uint8_t *rxbuf, size_t rxbytes)
{
    msg_t res;

    ASSERT_NOT(txbuf == NULL || txbytes == 0);

#if I2C_USE_MUTUAL_EXCLUSION
    i2cAcquireBus(&I2CD1);
#endif
    res = i2cMasterTransmitTimeout(&I2CD1, address, txbuf, txbytes,
            rxbuf, rxbytes, TIME_MS2I(I2CD_TIMEOUT_MS));
#if I2C_USE_MUTUAL_EXCLUSION
    i2cReleaseBus(&I2CD1);
#endif

    if (res == MSG_OK && i2cGetErrors(&I2CD1) == I2C_NO_ERROR) {
        return true;
    }
    /** timeout should never happen, driver must be restarted afterwards */
    if (res == MSG_TIMEOUT) {
        i2cStart(&I2CD1, &i2cdi_config);
    }
    return false;
}

void I2Cd_Init(bool full_speed)
{
#ifdef STM32F4XX
    i2cdi_config.op_mode = OPMODE_I2C;
    i2cdi_config.clock_speed = full_speed ? 400000 : 100000;
    i2cdi_config.duty_cycle = full_speed ? FAST_DUTY_CYCLE_2 : STD_DUTY_CYCLE;
#else
    i2cdi_config.cr1 = 0;
    i2cdi_config.cr2 = 0;
    i2cdi_config.timingr = I2Cdi_CalcTiming(full_speed);
#endif

    i2cStart(&I2CD1, &i2cdi_config);
}

/** @} */
