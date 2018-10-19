
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
 * @file    eeprom.h
 * @brief   I2C Eeprom driver
 *
 * @addtogroup drivers
 * @{
 */

#ifndef __DRIVERS_EEPROM_H
#define __DRIVERS_EEPROM_H

#include <types.h>
#include <stdlib.h>

/**
 * Write data to eeprom
 *
 * @param [in] address      Address to write data to
 * @param [in] buf          Data to write
 * @param [in] size         Length of data buffer
 *
 * @return  Successfulness of the operation
 */
extern bool Eepromd_Write(uint16_t address, const uint8_t *buf, size_t size);

/**
 * Write data to eeprom
 *
 * @param [in] address      Address to read data from
 * @param [in] buf          Data buffer
 * @param [in] size         Amount of data to receive
 *
 * @return  Successfulness of the operation
 */
extern bool Eepromd_Read(uint16_t address, uint8_t *buf, size_t size);

#endif

/** @} */
