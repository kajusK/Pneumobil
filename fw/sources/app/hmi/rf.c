/*
    BUT pneumobil - Copyright (C) 2019 Jakub Kaderka.

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
 * @file    hmi/rf.c
 * @brief   Sending logs and telemetry to pc
 *
 * @addtogroup app-hmi
 */

#include <hal.h>

#include <modules/log.h>
#include <modules/config.h>
#include <drivers/rfm69.h>
#include "state.h"
#include "rf.h"

#define NODE_ID 0x10
#define NETWORK_ID 0x20
#define DEST_ID 0x11

#define RF_THREAD_PRIO (NORMALPRIO - 1)

/** Stack and stuff for thread */
THD_WORKING_AREA(rfi_thread_area, 512);
MUTEX_DECL(rfi_mutex);

/** Structure for sending data */
typedef struct {
    uint16_t speed_dms;
    uint16_t speed_avg_dms;
    uint16_t speed_max_dms;
    uint16_t distance_m;
    uint8_t race_mode;
    uint8_t filling_pct;
    uint16_t deadtime_ms;
    uint16_t press1_kpa;
    uint16_t press2_kpa;
    uint16_t press3_kpa;
    uint8_t time_s;
    uint8_t time_m;
    uint16_t bat_mv;
    uint16_t bat_ma;
    uint8_t piston_pct;
    struct {
        uint8_t brake: 1;
        uint8_t throttle: 1;
        uint8_t valve_f1: 2;
        uint8_t valve_f2: 2;
        uint8_t valve_b1: 2;
        uint8_t valve_b2: 2;
    };
} __attribute__((packed)) rf_data_t;

/**
 * Callback for log module, will send log messages to PC
 *
 * @param [in] log      Pointer to generated log message
 */
static void Rfi_LogCb(const log_msg_t *log)
{
    chMtxLock(&rfi_mutex);
    rfm69_Send(DEST_ID, (uint8_t *) log, sizeof(log));
    chMtxUnlock(&rfi_mutex);
}

/**
 * Send current car state over rf link
 */
static void Rfi_SendState(void)
{
    state_t *state;
    uint32_t time;
    rf_data_t data;

    state = State_Get();
    time = State_GetRaceTimeMs()/1000;

    data.speed_dms = state->car.speed_kmh/3.6*100;
    data.speed_avg_dms = state->car.speed_avg_kmh/3.6*100;
    data.speed_max_dms = state->car.speed_max_kmh/3.6*100;
    data.distance_m = state->car.distance_m;

    data.race_mode = state->car.mode;
    data.filling_pct = 0;
    data.deadtime_ms = 0;

    data.press1_kpa = state->pneu.press1_kpa;
    data.press2_kpa = state->pneu.press2_kpa;
    data.press3_kpa = state->pneu.press3_kpa;
    data.piston_pct = state->pneu.piston_pct;

    data.time_s = time % 60;
    data.time_m = time / 60;

    data.bat_mv = state->psu.battery.bat1_mv + state->psu.battery.bat2_mv;
    data.bat_ma = state->psu.battery.cur_ma;
    data.brake = state->car.brake;
    data.throttle = state->car.throttle;
    data.valve_f1 = state->pneu.valve_front1;
    data.valve_f2 = state->pneu.valve_front2;
    data.valve_b1 = state->pneu.valve_back1;
    data.valve_b2 = state->pneu.valve_back2;

    chMtxLock(&rfi_mutex);
    rfm69_Send(DEST_ID, (uint8_t *) &data, sizeof(data));
    chMtxUnlock(&rfi_mutex);
}

static THD_FUNCTION(Rf_Thread, arg)
{
    systime_t time;
    uint32_t period = Config_GetUint(CONFIG_UINT_RF_PERIOD_MS);
    (void) arg;

    while (true) {
        time = chTimeAddX(chVTGetSystemTime(), TIME_MS2I(period));
        Rfi_SendState();

        if (chVTGetSystemTime() < time) {
            chThdSleepUntil(time);
        }
    }
}

void Rf_Init(void)
{
    log_severity_t severity[LOG_SOURCE_COUNT];

    if (!Config_GetBool(CONFIG_BOOL_RF_ENABLED)) {
        Log_Info(LOG_SOURCE_APP, "RF not enabled, skipping");
        return;
    }

    if (rfm69_Init(NODE_ID, NETWORK_ID, true, false) == false) {
        Log_Error(LOG_SOURCE_APP, "Unable to initialize rf module, not responding");
        return;
    }
    rfm69_SetPowerLevel(Config_GetFloat(CONFIG_FLOAT_RF_POWER_DBM));
    rfm69_SetBitrate(115200);

    for (int i = 0; i < LOG_SOURCE_COUNT; i++) {
        severity[i] = LOG_SEVERITY_INFO;
    }
    Log_Subscribe(Rfi_LogCb, severity, true);

    Log_Info(LOG_SOURCE_APP, "Starting sending data over RF link");
    (void) chThdCreateStatic(rfi_thread_area, sizeof(rfi_thread_area),
                RF_THREAD_PRIO, Rf_Thread, NULL);
}

/** @} */
