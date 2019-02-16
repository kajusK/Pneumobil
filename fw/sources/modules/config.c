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
 * @file    config.c
 * @brief   System configuration subsystem
 *
 * Reading and writing system wide configuration
 *
 * @addtogroup modules
 * @{
 */

#include "version.h"
#include "setup.h"
#include "utils/assert.h"
#include "modules/log.h"
#include "modules/storage.h"

#include "modules/config.h"

uint32_t config_item_uint[CONFIG_UINT_COUNT];
uint8_t config_item_bool[CONFIG_BOOL_COUNT];
float config_item_float[CONFIG_FLOAT_COUNT];

system_info_t *Config_GetSystem(void)
{
    static system_info_t info = {
        VER_MAJOR,
        VER_MINOR,
        SYS_NAME,
        __DATE__
    };

    return &info;
}

uint32_t Config_GetUint(config_item_uint_t item)
{
    ASSERT_NOT(item >= CONFIG_UINT_COUNT || CONFIG_UINT_COUNT == 0);
    return config_item_uint[item];
}

void Config_SetUint(config_item_uint_t item, uint32_t value)
{
    ASSERT_NOT(item >= CONFIG_UINT_COUNT || CONFIG_UINT_COUNT == 0);

    Log_Debug(LOG_SOURCE_CONFIG, "Unsigned item %d set to %u", item, value);
    config_item_uint[item] = value;
#ifdef HAS_STORAGE
    Storage_Update(STORAGE_UPDATE_UINT);
#endif
}

void Config_ResetUint(void)
{
    int i;
    Log_Debug(LOG_SOURCE_CONFIG, "Reseting unsigned items");

    for (i = 0; i < CONFIG_UINT_COUNT; i++) {
        config_item_uint[i] = config_default_uint[i];
    }
#ifdef HAS_STORAGE
    Storage_Update(STORAGE_UPDATE_UINT);
#endif
}

float Config_GetFloat(config_item_float_t item)
{
    ASSERT_NOT(item >= CONFIG_FLOAT_COUNT || CONFIG_FLOAT_COUNT == 0);
    return config_item_float[item];
}

void Config_SetFloat(config_item_float_t item, float value)
{
    ASSERT_NOT(item >= CONFIG_FLOAT_COUNT || CONFIG_FLOAT_COUNT == 0);

    Log_Debug(LOG_SOURCE_CONFIG, "Float item %d set to %f", item, value);
    config_item_float[item] = value;
#ifdef HAS_STORAGE
    Storage_Update(STORAGE_UPDATE_FLOAT);
#endif
}

void Config_ResetFloat(void)
{
    int i;
    Log_Debug(LOG_SOURCE_CONFIG, "Reseting float items");

    for (i = 0; i < CONFIG_FLOAT_COUNT; i++) {
        config_item_float[i] = config_default_float[i];
    }
#ifdef HAS_STORAGE
    Storage_Update(STORAGE_UPDATE_FLOAT);
#endif
}

bool Config_GetBool(config_item_bool_t item)
{
    uint8_t mask;
    ASSERT_NOT(item >= CONFIG_BOOL_COUNT || CONFIG_BOOL_COUNT == 0);

    mask = 1 << (item % 8);
    return config_item_bool[item / 8] & mask;
}

void Config_SetBool(config_item_bool_t item, bool value)
{
    uint8_t mask;
    ASSERT_NOT(item >= CONFIG_BOOL_COUNT || CONFIG_BOOL_COUNT == 0);

    Log_Debug(LOG_SOURCE_CONFIG, "Bool item %d set to %d", item, value);
    mask = 1 << (item % 8);
    if (value) {
        config_item_bool[item / 8] |= mask;
    } else {
        config_item_bool[item / 8] &= ~mask;
    }
#ifdef HAS_STORAGE
    Storage_Update(STORAGE_UPDATE_BOOL);
#endif
}

void Config_ResetBool(void)
{
    int i;
    Log_Debug(LOG_SOURCE_CONFIG, "Reseting bool items");

    for (i = 0; i < CONFIG_BOOL_COUNT/8 + 1; i++) {
        config_item_bool[i] = config_default_bool[i];
    }
#ifdef HAS_STORAGE
    Storage_Update(STORAGE_UPDATE_BOOL);
#endif
}

void Config_Reset(void)
{
    Config_ResetUint();
    Config_ResetBool();
    Config_ResetFloat();
}

/** @} */
