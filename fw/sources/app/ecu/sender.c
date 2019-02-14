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
 * @file    ecu/sender.c
 * @brief   Periodic sending of car state to outside world
 *
 * @addtogroup app-ecu
 * @{
 */
#include <ch.h>

#include <utils/assert.h>
#include <modules/config.h>
#include <modules/comm/comm.h>

#include "car.h"
#include "ecu.h"
#include "ecu_io.h"
#include "race.h"

#include "sender.h"

/* Priority of the ECU thread should be higher than other threads */
#define SENDER_THREAD_PRIO (NORMALPRIO - 1)

/** Stack and stuff for thread */
THD_WORKING_AREA(senderi_thread_area, 1024);

static void Senderi_SendState(void)
{
    car_data_t state;
    ecu_inputs_t inputs;
    ecu_valves_t valves;

    Car_GetState(&state);
    ECU_GetRawInputs(&inputs);
    ECU_ValvesGet(&valves);

    Comm_SendCarState(state.speed_dms, state.speed_avg_dms, state.distance_m,
            Race_GetMode());
    Comm_SendCarIO(&inputs, &valves, ECU_GetGear());
    Comm_SendPneuState(state.pressure_kpa[0], state.pressure_kpa[1],
            state.pressure_kpa[2], ECU_GetRawPistonPosPct());
}

static THD_FUNCTION(Sender_Thread, arg)
{
    systime_t time;
    uint32_t looptime_ms = Config_GetUint(CONFIG_UINT_SENDER_PERIOD_MS);
    (void) arg;

    while (true) {
        time = chTimeAddX(chVTGetSystemTime(), chTimeMS2I(looptime_ms));
        Senderi_SendState();
        chThdSleepUntil(time);
    }
}

void Sender_Init(void)
{
    (void) chThdCreateStatic(senderi_thread_area, sizeof(senderi_thread_area),
                SENDER_THREAD_PRIO, Sender_Thread, NULL);
}

/** @} */
