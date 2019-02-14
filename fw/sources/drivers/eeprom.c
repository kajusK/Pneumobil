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
 * @file    eeprom.c
 * @brief   I2C Eeprom driver
 *
 * @addtogroup drivers
 * @{
 */

#include <ch.h>
#include <string.h>

#include "utils/assert.h"
#include "drivers/i2c.h"
#include "drivers/eeprom.h"

/** I2C address of the eeprom */
#define EEPROM_I2C_ADDR (0xa0 >> 1)

/** Size of eeprom page */
#define EEPROM_PAGE_SIZE 32

/** Eeprom size in bytes */
#define EEPROM_SIZE 8000

/** Delay required before writing another page */
#define EEPROM_WRITE_PAGE_MS 5

bool Eepromd_Write(uint16_t address, const uint8_t *buf, size_t size)
{
    uint8_t data[EEPROM_PAGE_SIZE+2];
    uint16_t pageAddr = address;
    uint16_t pos = 0;
    uint8_t bytes;
    bool ret = true;

    ASSERT_NOT(buf == NULL);
    if (size == 0) {
        return true;
    }

    if (address + size >= EEPROM_SIZE) {
        return false;
    }

    if (address % EEPROM_PAGE_SIZE != 0) {
        bytes = EEPROM_PAGE_SIZE - (address % EEPROM_PAGE_SIZE);
    } else {
        bytes = EEPROM_PAGE_SIZE;
    }

    if (bytes > size) {
        bytes = size;
    }


    do {
        data[0] = (pageAddr >> 8) & 0xff;
        data[1] = pageAddr & 0xff;
        memcpy(&data[2], &buf[pos], bytes);
        ret = I2Cd_Transceive(EEPROM_I2C_ADDR, data, bytes+2, NULL, 0);
        if (ret != true) {
            return false;
        }
        chThdSleep(TIME_MS2I(EEPROM_WRITE_PAGE_MS));

        pos += bytes;
        pageAddr += bytes;

        if ((size - pos) > EEPROM_PAGE_SIZE) {
            bytes = EEPROM_PAGE_SIZE;
        } else {
            bytes = size - pos;
        }
    } while (pos < size);

    return true;
}

bool Eepromd_Read(uint16_t address, uint8_t *buf, size_t size)
{
    uint8_t data[2];

    ASSERT_NOT(buf == NULL);
    if (size == 0) {
        return true;
    }

    data[0] = (address >> 8) & 0xff;
    data[1] = address & 0xff;

    return I2Cd_Transceive(EEPROM_I2C_ADDR, data, 2, buf, size);
}

/** @} */
