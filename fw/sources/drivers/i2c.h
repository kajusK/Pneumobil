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
 * @file    i2c.h
 * @brief   I2C driver
 *
 * @addtogroup drivers
 * @{
 */

#ifndef __DRIVERS_I2C_H
#define __DRIVERS_I2C_H

#include <inttypes.h>

/**
 * Send and receive data over i2c
 *
 * @param [in] address      i2c device address without rw bit
 * @param [in] txbuf        Data to be send
 * @param [in] txbytes      Amount of bytes to send
 * @param [out] rxbuf       Data buffer for received data
 * @param [in] rxbytes      Amount of data to receive, 0 for transmit only
 *
 * @return  Successfulness of the operation
 */
extern bool I2Cd_Transceive(uint8_t address, const uint8_t *txbuf,
        size_t txbytes, uint8_t *rxbuf, size_t rxbytes);

/**
 * Initialize I2C module
 *
 * @param [in] full_speed   true for 400 kHz, false for 100 kHz
 */
extern void I2Cd_Init(bool full_speed);

#endif

/** @} */
