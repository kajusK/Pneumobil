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
 * @file    config_items.h
 * @brief   Configuration storage subsystem - configuration items
 *
 * Generated automatically by script, do not modify
 *
 * @addtogroup modules
 * @{
 */

#ifndef __TEST_CONFIG_ITEMS_H
#define __TEST_CONFIG_ITEMS_H

#include <types.h>

typedef enum config_item_uint_t {
    CONFIG_UINT_TEST1,
    CONFIG_UINT_TEST2,
    CONFIG_UINT_TEST3,
    CONFIG_UINT_TEST4,
    CONFIG_UINT_PISTON_LEN,
    CONFIG_UINT_COUNT,
} config_item_uint_t;

typedef enum {
    CONFIG_BOOL_TEST1,
    CONFIG_BOOL_TEST2,
    CONFIG_BOOL_TEST3,
    CONFIG_BOOL_TEST4,
    CONFIG_BOOL_TEST5,
    CONFIG_BOOL_TEST6,
    CONFIG_BOOL_TEST7,
    CONFIG_BOOL_TEST8,
    CONFIG_BOOL_TEST9,
    CONFIG_BOOL_TEST10,
    CONFIG_BOOL_TEST11,
    CONFIG_BOOL_TEST12,
    CONFIG_BOOL_ENCODER_INVERT,
    CONFIG_BOOL_PNEU_NON_BLOCK,
    CONFIG_BOOL_COUNT,
} config_item_bool_t;

#undef extern
#define extern extern

extern const uint32_t config_default_uint[CONFIG_UINT_COUNT];
extern const uint8_t config_default_bool[CONFIG_BOOL_COUNT/8 + 1];

#undef extern
#define extern static

#endif

/** @} */
