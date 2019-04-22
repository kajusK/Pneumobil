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

#include <hal.h>
#include <chprintf.h>

#include <modules/log.h>
#include <modules/config.h>
#include <utils/assert.h>
#include <utils/time.h>
#include <drivers/fatfs/sdc.h>
#include <drivers/rtc.h>

#include "state.h"
#include "logger.h"

#define RACE_LOG_DIR "race_log"
#define LOGGER_THREAD_PRIO (NORMALPRIO - 1)

/** Stack and stuff for thread */
THD_WORKING_AREA(loggeri_thread_area, 1024);

static FIL loggeri_syslog_file;
static FIL loggeri_race_file;

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
    strftime(buf, sizeof(buf), "%c", &timeinfo);

    /* Print errors are ignored - SD card could be removed in the meanwhile */
    f_printf(&loggeri_syslog_file, "%s [%s] %s <%s>: %s\n",
            buf, Log_GetModuleStr(log->module), Log_GetSourceStr(log->src),
            Log_GetSeverityStr(log->severity), log->msg);
    f_sync(&loggeri_syslog_file);
}

/**
 * Add new entry to race log, data are fetched from various modules
 */
static void Loggeri_AddRaceLogEntry(void)
{
    char speed[6];
    state_t *state = State_Get();

    if (!SDCd_IsReady()) {
        return;
    }

    chsnprintf(speed, sizeof(speed), "%.1f", state->car.speed_kmh);

    f_printf(&loggeri_race_file, "%d;%s;%d;%d;%d;%d;%d;%d;%d;%d\n",
            State_GetRaceTimeMs(), speed,
            state->car.distance_m, state->pneu.press1_kpa,
            state->pneu.press2_kpa, state->pneu.press3_kpa,
            state->pneu.piston_pct, state->car.throttle, state->car.brake,
            state->car.gear);
    f_sync(&loggeri_race_file);
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
        Log_Error(LOG_SOURCE_APP, "Unable to create syslog file");
    }

    res = f_mkdir(RACE_LOG_DIR);
    if (res != FR_OK && res != FR_EXIST) {
        Log_Error(LOG_SOURCE_APP, "Unable to create race log directory");
    }

    Logger_NewRaceLogFile();
}

static THD_FUNCTION(Logger_Thread, arg)
{
    systime_t time;
    uint32_t period = Config_GetUint(CONFIG_UINT_LOG_PERIOD_MS);
    (void) arg;

    while (true) {
        time = chTimeAddX(chVTGetSystemTime(), TIME_MS2I(period));
        Loggeri_AddRaceLogEntry();

        if (chVTGetSystemTime() < time) {
            chThdSleepUntil(time);
        }
    }
}

bool Logger_NewRaceLogFile(void)
{
    FRESULT res;
    struct tm timeinfo;
    race_mode_t mode;
    char filename[128];

    if (!SDCd_IsReady()) {
        Log_Warn(LOG_SOURCE_APP, "Unable to create log file, sd card not ready");
        return false;
    }

    mode = State_GetRaceMode();
    if (mode >= RACE_MODE_DEBUG) {
        return false;
    }

    RTCd_GetTime(&timeinfo);

    snprintf(filename, sizeof(filename), RACE_LOG_DIR "/%s_",
            loggeri_race_mode_str[mode]);
    strftime(&filename[strlen(filename)], sizeof(filename)-strlen(filename),
            "%d_%m_%Y_%H-%M-%S.csv", &timeinfo);

    res = f_open(&loggeri_race_file, filename, FA_WRITE | FA_CREATE_ALWAYS);
    if (res != FR_OK) {
        Log_Error(LOG_SOURCE_APP, "Failed to create log file %s", filename);
        return false;
    }

    f_puts("time [ms];speed [km/h];distance [m];"
        "pressure1 [kpa];pressure2 [kpa];pressure3 [kpa];"
        "piston_pos [%];throttle;brake;gear\n", &loggeri_race_file);
    f_sync(&loggeri_race_file);

    Log_Info(LOG_SOURCE_APP, "Created a log file %s", filename);
    return true;
}

void Logger_Init(void)
{
    log_severity_t severity[LOG_SOURCE_COUNT];

    for (int i = 0; i < LOG_SOURCE_COUNT; i++) {
        severity[i] = LOG_SEVERITY_INFO;
    }
    Log_Subscribe(Loggeri_LogCb, severity, true);

    /* Add callback for card inserted, if card inserted already, run it */
    if (SDCd_IsReady()) {
        Loggeri_CardInsertedCb();
    }
    SDCd_AddInsertCallback(Loggeri_CardInsertedCb);

    (void) chThdCreateStatic(loggeri_thread_area, sizeof(loggeri_thread_area),
                LOGGER_THREAD_PRIO, Logger_Thread, NULL);
}

/** @} */
