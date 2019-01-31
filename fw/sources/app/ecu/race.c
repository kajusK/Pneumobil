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
 * @file    ecu/race.c
 * @brief   Handling race modes, parameters changing,...
 *
 * @addtogroup app-ecu
 * @{
 */

#include <utils/assert.h>
#include <modules/log.h>
#include <modules/config.h>
#include <utils/time.h>

#include "race.h"

/** Current race mode */
static race_mode_t racei_mode = RACE_MODE_ARCADE;
/** Timestamp of the moment race was started */
static uint32_t racei_start_timestamp;

/** Deadtime for the current race mode */
static uint16_t racei_deadtime_ms;
/** Filling percent for current race mode */
static uint8_t racei_filling_pct;

uint8_t  Race_GetFillingPct(void)
{
    if (racei_filling_pct == 0) {
        return 100;
    }
    return racei_filling_pct;
}

uint16_t Race_GetDeadpointMs(void)
{
    return racei_deadtime_ms;
}

race_mode_t Race_GetMode(void)
{
    return racei_mode;
}

uint32_t Race_GetDurationMs(void)
{
    return millis() - racei_start_timestamp;
}

bool Race_Start(race_mode_t mode)
{
    racei_start_timestamp = millis();

    switch (mode) {
        case RACE_MODE_LONG_DISTANCE:
            racei_deadtime_ms = Config_GetUint(CONFIG_UINT_DEADTIME_MS_LONG_DIST);
            racei_filling_pct = Config_GetUint(CONFIG_UINT_FILLING_PCT_LONG_DIST);
            Log_Info(LOG_SOURCE_ECU, "Switched to Long Distance");
            break;
        case RACE_MODE_ACCELERATION:
            racei_deadtime_ms = Config_GetUint(CONFIG_UINT_DEADTIME_MS_ACCELERATION);
            racei_filling_pct = Config_GetUint(CONFIG_UINT_FILLING_PCT_ACCELERATION);
            Log_Info(LOG_SOURCE_ECU, "Switched to Acceleration");
            break;
        case RACE_MODE_ARCADE:
            racei_deadtime_ms = Config_GetUint(CONFIG_UINT_DEADTIME_MS_ARCADE);
            racei_filling_pct = Config_GetUint(CONFIG_UINT_FILLING_PCT_ARCADE);
            Log_Info(LOG_SOURCE_ECU, "Switched to Arcade");
            break;
        case RACE_MODE_DEBUG:
            racei_deadtime_ms = 0;
            racei_filling_pct = 0;
            Log_Info(LOG_SOURCE_ECU, "Switched to Debug");
            break;
        default:
            Log_Error(LOG_SOURCE_ECU, "Trying to switch to unknown mode %d", mode);
            return false;
    }

    racei_mode = mode;
    return true;
}


/** @} */
