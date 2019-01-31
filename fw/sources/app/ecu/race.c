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
#include <modules/config.h>
#include <utils/time.h>

#include "race.h"

/** Current race mode */
static race_mode_t racei_mode = RACE_MODE_ARCADE;
static uint32_t racei_start_timestamp;

static void Race_StartArcade(void)
{

}

static void Race_StartAcceleration(void)
{

}

static void Race_StartLongDistance(void)
{

}

static void Race_StartDebug(void)
{

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
    racei_mode = mode;

    return true;
}


/** @} */
