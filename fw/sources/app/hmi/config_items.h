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
 * @file    hmi/config_items.h
 * @brief   Configuration storage subsystem - configuration items
 *
 * Generated automatically by script, do not modify
 *
 * @addtogroup app-hmi
 * @{
 */

#ifndef __HMI_CONFIG_ITEMS_H
#define __HMI_CONFIG_ITEMS_H

#include <types.h>

/** Config items list - autogenerated by sw/generators/config_items.py */
typedef enum {
    CONFIG_UINT_BACKLIGHT, /* Default lcd backlight percentage */
    CONFIG_UINT_CONTRAST, /* Default LCD contrast */
    CONFIG_UINT_LOG_PERIOD_MS, /* Logging period on sd card in ms */
    CONFIG_UINT_ARCADE_DEADTIME_MAX_MS, /* Maximum length of deadtime in arcade mode */
    CONFIG_UINT_ACCELERATION_DEADTIME_MAX_MS, /* Maximum length of deadtime in acceleration mode */
    CONFIG_UINT_LONG_DIST_DEADTIME_MAX_MS, /* Maximum length of deadtime in long distance mode */
    CONFIG_UINT_COUNT,
} config_item_uint_t;

/** Config items list - autogenerated by sw/generators/config_items.py */
typedef enum {
    CONFIG_BOOL_LCD_ROTATE, /* Flip lcd horizontally */
    CONFIG_BOOL_COUNT,
} config_item_bool_t;

/** Config items list - autogenerated by sw/generators/config_items.py */
typedef enum {
    CONFIG_FLOAT_TOUCH_AX, /* LCD calibration data */
    CONFIG_FLOAT_TOUCH_BX, /* LCD calibration data */
    CONFIG_FLOAT_TOUCH_CX, /* LCD calibration data */
    CONFIG_FLOAT_TOUCH_AY, /* LCD calibration data */
    CONFIG_FLOAT_TOUCH_BY, /* LCD calibration data */
    CONFIG_FLOAT_TOUCH_CY, /* LCD calibration data */
    CONFIG_FLOAT_COUNT,
} config_item_float_t;

extern const uint32_t config_default_uint[CONFIG_UINT_COUNT];

extern const uint8_t config_default_bool[CONFIG_BOOL_COUNT/8+1];

extern const float config_default_float[CONFIG_FLOAT_COUNT];

#endif

/** @} */
