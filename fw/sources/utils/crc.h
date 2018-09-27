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
 * @file    crc.h
 * @brief   Crc calculation
 *
 * @addtogroup utils
 * @{
 */

#ifndef __UTILS_CRC_H
#define __UTILS_CRC_H

#include <inttypes.h>
#include <stdlib.h>

/** Initial value for CRC8 calculation */
#define CRC8_INITIAL_VALUE  0x00

/**
 * Calculate CRC from initial value and single byte
 *
 * @param [in] buf  Byte to calculate crc for
 * @param [in] crc  Initial CRC value
 *
 * @return CRC 8 (polynomial 0x07)
 */
extern uint8_t CRC8_Add(uint8_t buf, uint8_t crc);

/**
 * Calculate CRC for buffer
 *
 * @param [in] buf  Data to calculate crc for
 * @param [in] len  Length of data buffer
 *
 * @return CRC 8 (polynomial 0x07)
 */
extern uint8_t CRC8(const uint8_t *buf, uint16_t len);

#endif

/** @} */
