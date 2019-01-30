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
 * @file    hmi/logger.c
 * @brief   Logging to the SD card
 *
 * @addtogroup app-hmi
 * @{
 */

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ff.h>

#include <modules/log.h>
#include <utils/assert.h>
#include <utils/time.h>
#include <drivers/fatfs/sdc.h>
#include <drivers/rtc.h>

#include "state.h"
#include "logger.h"

#define RACE_LOG_DIR "/race_log"

static FIL loggeri_syslog_file;
static FIL loggeri_race_file;
static uint32_t loggeri_log_created_timestamp;

static const char loggeri_severity_str[LOG_SEVERITY_COUNT][8] = {
    "Unknown",
    "Error",
    "Warn",
    "Info",
    "Debug",
};

static const char loggeri_source_str[LOG_SOURCE_COUNT][7] = {
    "SYSTEM",
    "DRIVER",
    "COMM",
    "CONFIG",
    "ECU",
    "HMI",
};

static const char loggeri_race_mode_str[4][14] = {
    "arcade",
    "acceleration",
    "long_distance",
};

/**
 * Callback for log module, will save log message to syslog on sdcard
 *
 * @param [in] log      Pointer to generated log message
 */
static void Loggeri_LogCb(const log_msg_t *log)
{
    struct tm timeinfo;
    char buf[32];

    ASSERT_NOT(log == NULL || log->src >= LOG_SOURCE_COUNT ||
            log->severity >= LOG_SEVERITY_COUNT);

    if (!SDCd_IsReady()) {
        return;
    }

    RTCd_GetTime(&timeinfo);
    asctime_r(&timeinfo, buf);

    /* Print errors are ignored - SD card could be removed,... */
    f_printf(&loggeri_syslog_file, "%s %s <%s>: %s\n",
            buf, loggeri_source_str[log->src],
            loggeri_severity_str[log->severity],
            log->msg);
    f_sync(&loggeri_syslog_file);
}

/**
 * Will be called on card insertion
 *
 * Create directory structure, log files, etc...
 */
static void Loggeri_CardInsertedCb(void)
{
    FRESULT res;

    res = f_open(&loggeri_syslog_file, "syslog.txt", FA_WRITE | FA_OPEN_APPEND);
    if (res != FR_OK) {
        Log_Error(LOG_SOURCE_HMI, "Unable to create syslog file");
    }

    res = f_mkdir(RACE_LOG_DIR);
    if (res != FR_OK && res != FR_EXIST) {
        Log_Error(LOG_SOURCE_HMI, "Unable to create race log directory");
    }

    Logger_NewRaceLogFile();
}

bool Logger_NewRaceLogFile(void)
{
    FRESULT res;
    struct tm timeinfo;
    state_race_mode_t mode;
    char filename[128];

    if (!SDCd_IsReady()) {
        Log_Warn(LOG_SOURCE_HMI, "Unable to create log file, sd card not ready");
        return false;
    }

    mode = State_GetRaceMode();
    if (mode >= RACE_MODE_DEBUG) {
        return false;
    }

    RTCd_GetTime(&timeinfo);

    snprintf(filename, 128, RACE_LOG_DIR "/%s_", loggeri_race_mode_str[mode]);
    asctime_r(&timeinfo, &filename[strlen(filename)]);
    strncat(filename, ".csv", 128 - strlen(filename));

    res = f_open(&loggeri_race_file, filename, FA_WRITE | FA_CREATE_ALWAYS);
    if (res != FR_OK) {
        Log_Error(LOG_SOURCE_HMI, "Failed to create log file %s", filename);
        return false;
    }

    f_puts("time [ms];speed [km/h];distance [m];"
        "pressure1 [kpa];pressure2 [kpa];pressure3 [kpa];"
        "piston_pos [%];throttle;brake;gear\n", &loggeri_race_file);
    f_sync(&loggeri_race_file);

    Log_Info(LOG_SOURCE_HMI, "Created a log file %s", filename);
    return true;
}

/**
 * Add new entry to race log, data are fetched from various modules
 */
void Logger_AddRaceLogEntry(void)
{
    char speed[6];
    uint32_t time;
    state_t *state = State_Get();

    if (!SDCd_IsReady()) {
        return;
    }

    time = millis() - loggeri_log_created_timestamp;

    snprintf(speed, sizeof(speed), "%.1f", state->car.speed_kmh);

    f_printf(&loggeri_race_file, "%d;%s;%d;%d;%d;%d;%d;%d;%d;%d\n",
            time, speed, state->car.distance_m, state->pneu.press1_kpa,
            state->pneu.press2_kpa, state->pneu.press3_kpa,
            state->pneu.piston_pct, state->car.throttle, state->car.brake,
            state->car.gear);
    f_sync(&loggeri_race_file);
}

/** Should be called on card insertion */
void Logger_Init(void)
{
    log_severity_t severity[LOG_SOURCE_COUNT];

    for (int i = 0; i < LOG_SOURCE_COUNT; i++) {
        severity[i] = LOG_SEVERITY_INFO;
    }
    Log_Subscribe(Loggeri_LogCb, severity);

    /* Add callback for card inserted, if card inserted already, run it */
    if (SDCd_IsReady()) {
        Loggeri_CardInsertedCb();
    }
    SDCd_AddInsertCallback(Loggeri_CardInsertedCb);
}

/** @} */