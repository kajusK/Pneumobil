/*
    BUT pneumobil - Copyright (C) 2018 Jakub Kaderka.

    This file is part of BUT pneumobil ECU.

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
 * @file    config_items.c
 * @brief   config items for unit testing
 *
 * @addtogroup tests
 * @{
 */

#include "config_items.h"

const uint32_t config_default_uint[CONFIG_UINT_COUNT] = {
    1000,
    2000,
    3000,
    4000,
};

const uint8_t config_default_bool[CONFIG_BOOL_COUNT/8 + 1] = {
    0xf0,
    0x0a,
};

const float config_default_float[CONFIG_FLOAT_COUNT] = {
    1.234,
    4.321,
};

/** @} */
