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
 * @file    config.h
 * @brief   Configuration storage subsystem
 *
 * @addtogroup modules
 * @{
 */

#ifndef __MODULES_CONFIG_H
#define __MODULES_CONFIG_H

#include <types.h>

#include "config_items.h"

typedef struct {
    uint8_t major;
    uint8_t minor;
    char name[8];
    char build_on[16];
} system_info_t;

/**
 * Get system info
 *
 * @return System info structure
 */
extern system_info_t *Config_GetSystem(void);

/**
 * Get unsigned item from config module
 *
 * @param [in] item ID of item requested
 * @return requested item value
 */
extern uint32_t Config_GetUint(config_item_uint_t item);

/**
 * Set unsigned item to config module
 *
 * @param [in] item ID of item requested
 * @param [in] value value to stora
 */
extern void Config_SetUint(config_item_uint_t item, uint32_t value);

/**
 * Reset unsigned items to default values
 */
extern void Config_ResetUint(void);

/**
 * Get bool item from config module
 *
 * @param [in] item ID of item requested
 * @return requested item value
 */
extern bool Config_GetBool(config_item_bool_t item);

/**
 * Set bool item to config module
 *
 * @param [in] item ID of item requested
 * @param [in] value value to stora
 */
extern void Config_SetBool(config_item_bool_t item, bool value);

/**
 * Reset bool items to default values
 */
extern void Config_ResetBool(void);

/**
 * Reset all data to default values
 */
extern void Config_Reset(void);

#endif

/** @} */
