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
 * @file    ecu/config_items.h
 * @brief   Configuration storage subsystem - configuration items
 *
 * Generated automatically by script, do not modify
 *
 * @addtogroup app-ecu
 * @{
 */

#ifndef __MODULES_CONFIG_ITEMS_H
#define __MODULES_CONFIG_ITEMS_H

#include <types.h>

typedef enum {
    CONFIG_UINT_PISTON_LEN,
    CONFIG_UINT_WHEEL_DIA_MM,
    CONFIG_UINT_PULSES_PER_WHEEL_ROTATION,
    CONFIG_UINT_COUNT,
} config_item_uint_t;

typedef enum {
    CONFIG_BOOL_CAN_PROMISCUOUS,        /* Receive all messages from can if true */
    CONFIG_BOOL_ENCODER_INVERT,
    CONFIG_BOOL_PNEU_NON_BLOCK,
    CONFIG_BOOL_COUNT,
} config_item_bool_t;

extern const uint32_t config_default_uint[CONFIG_UINT_COUNT];

extern const uint8_t config_default_bool[CONFIG_BOOL_COUNT/8+1];

#endif

/** @} */
