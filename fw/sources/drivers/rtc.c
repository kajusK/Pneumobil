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
 * @file    rtc.c
 * @brief   Real time clock driver
 *
 * @addtogroup drivers
 * @{
 */
#include <hal.h>

#include "modules/log.h"
#include "utils/assert.h"
#include "drivers/rtc.h"

void RTCd_GetTime(struct tm *timp)
{
    RTCDateTime timespec;

    ASSERT_NOT(timp == NULL);

    rtcGetTime(&RTCD1, &timespec);
    rtcConvertDateTimeToStructTm(&timespec, timp, NULL);
}

void RTCd_SetTime(const struct tm *timp)
{
    RTCDateTime timespec;

    ASSERT_NOT(timp == NULL);

    Log_Debug(LOG_SOURCE_DRIVER, "Updating RTC time");

    rtcConvertStructTmToDateTime(timp, 0, &timespec);
    rtcSetTime(&RTCD1, &timespec);
}

/** @} */

