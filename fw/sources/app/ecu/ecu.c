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
 * @file    ecu.c
 * @brief   Engine control block - timing of valves, etc
 *
 * @addtogroup app-ecu
 * @{
 */

#include <ch.h>

#include <utils/assert.h>
#include <modules/log.h>
#include <modules/config.h>
#include <drivers/encoder.h>
#include "ecu_io.h"
#include "race.h"
#include "ecu.h"

/** Run control thread every x ms */
#define ECU_LOOP_CYCLE_MS 5

/* Priority of the ECU thread should be higher than other threads */
#define ECU_THREAD_PRIO (NORMALPRIO + 1)

/** Stack and stuff for thread */
THD_WORKING_AREA(ecui_thread_area, 512);

/** Current ECU state */
typedef enum {
    ECU_STATE_INIT,         /** ECU needs to be initialized */
    ECU_STATE_IDLE,         /** Throttle not pressed */
    ECU_STATE_DEADPOINT,    /** Piston in deadpoint, waiting for next cycle */
    ECU_STATE_MOVE,         /** Piston in movement */
} ecu_state_t;

/** Direction in which piston is currently moving (or was moving before stop */
typedef enum {
    ECU_DIR_BACK,
    ECU_DIR_FRONT,
} ecu_dir_t;

/** Structure for holding all ECU control data */
typedef struct {
    ecu_inputs_t inputs;    /** Input pins state */
    ecu_state_t state;      /** Current ecu state */
    ecu_dir_t dir;          /** Direction of the movement */
    uint8_t filling_pct;    /** Amount of piston length percent to fill */
    uint8_t deadtime_ms;    /** Delay between two pneumatic cycles */
    systime_t timeout;      /** Time when deadpoint delay should end */
    uint8_t piston_start_pct; /** Position in moment of direction change */
    bool dual;              /** Use dual valves mode */
} ecu_control_t;

/**
 * Check if Motor can be unlocked
 *
 * @param [in] inputs   ECU input pins
 * @return true if enabled
 */
static bool ECUi_PneuEnabled(const ecu_inputs_t *inputs)
{
    ASSERT_NOT(inputs == NULL);

    if (Config_GetBool(CONFIG_BOOL_PNEU_NON_BLOCK)) {
        return true;
    }

    if (!inputs->brake && !inputs->shifting) {
        return true;
    }
    return false;
}

/**
 * Initialize rotary encoder by finding edge positions
 *
 * @param [in,out] control  Control structure
 */
static void ECUi_EdgesInit(ecu_control_t *control)
{
    ecu_inputs_t *inputs = &control->inputs;
    uint16_t pistonLen;

    ASSERT_NOT(control == NULL);

    if (control->state != ECU_STATE_INIT && control->state != ECU_STATE_IDLE) {
        Log_Error(LOG_SOURCE_APP, "Init called with state %d", control->state);
        control->state = ECU_STATE_INIT;
    }
    pistonLen = Config_GetUint(CONFIG_UINT_PISTON_LEN);

    if (inputs->endstop_back) {
        Log_Info(LOG_SOURCE_APP, "Init: Back endstop hit");
        Encoderd_Set(0);
        if (pistonLen != 0) {
            /* Last direction */
            control->dir = ECU_DIR_BACK;
            control->state = ECU_STATE_IDLE;
        } else {
            control->dir = ECU_DIR_FRONT;
        }
    }

    if (inputs->endstop_front) {
        Log_Info(LOG_SOURCE_APP, "Init: Front endstop hit");

        if (pistonLen != 0) {
            Encoderd_Set(pistonLen);
            /* Last direction */
            control->dir = ECU_DIR_FRONT;
            control->state = ECU_STATE_IDLE;
        } else if (control->dir == ECU_DIR_FRONT) {
            int16_t len = Encoderd_Get();
            if (len < 0) {
                Config_SetBool(CONFIG_BOOL_ENCODER_INVERT, true);
                Encoderd_InvertDirection();
                len = -len;
                Encoderd_Set(len);
            }
            Config_SetUint(CONFIG_UINT_PISTON_LEN, len);
            Log_Info(LOG_SOURCE_APP, "Init done: Piston len: %d", len);
            control->state = ECU_STATE_IDLE;
        }
    }

    /* End initialization if piston len is known, edge will be found during
     * normal mode (endstop hit fixes the encoder positon) */
    if (pistonLen != 0) {
        if (Config_GetBool(CONFIG_BOOL_ENCODER_INVERT)) {
            Encoderd_InvertDirection();
        }
        /* Edge was not hit in first run, initialize to default */
        if (control->state == ECU_STATE_INIT) {
            Encoderd_Set(pistonLen/2);
            control->dir = ECU_DIR_BACK;
        }
        control->state = ECU_STATE_IDLE;
        Log_Info(LOG_SOURCE_APP, "Fast init done, len: %d", pistonLen);
    }

    /* Set valves according to current state */
    if (!inputs->throttle || !ECUi_PneuEnabled(inputs)) {
        ECU_ValvesClose();
    } else if (control->state == ECU_STATE_IDLE) {
        ECU_ValvesClose();
    } else if (control->dir == ECU_DIR_FRONT) {
        ECU_ValvesMoveFront(control->dual);
    } else {
        ECU_ValvesMoveBack(control->dual);
    }
}

/**
 * Calculate piston position in percent
 *
 * @param [in] inputs       Input pins of ecu module
 * @return  Piston position in percent
 */
static uint8_t ECUi_GetPistonPosPct(const ecu_inputs_t *inputs)
{
    uint8_t posPct;
    uint16_t len = Config_GetUint(CONFIG_UINT_PISTON_LEN);
    int16_t encoder = Encoderd_Get();

    ASSERT_NOT(inputs == NULL);

    if (inputs->endstop_back) {
        if (encoder != 0) {
            Log_Debug(LOG_SOURCE_APP, "Set encoder from %d to 0", encoder);
            Encoderd_Set(0);
            encoder = 0;
        }
    }
    if (inputs->endstop_front) {
        if (encoder != len) {
            Log_Debug(LOG_SOURCE_APP, "Set encoder from %d to %d", encoder, len);
            Encoderd_Set(len);
            encoder = len;
        }
    }

    /* counter over/underflowed due to counting single step multiple times */
    if (encoder < 0) {
        Encoderd_Set(0);
        encoder = 0;
        Log_Warn(LOG_SOURCE_APP, "Encoder underflowed");
    } else if (encoder > len) {
        Encoderd_Set(len);
        Log_Warn(LOG_SOURCE_APP, "Encoder oveflowed");
    }

    posPct = ((int32_t) encoder*100)/len;
    if (posPct > 100) {
        posPct = 100;
    }

    return posPct;
}

/**
 * Calculate and set valves state for new time period
 *
 * When throttle is pressed, do full pneu cycle
 * When the driver is shifting, close input valves
 *
 * @param [in,out] control  Control structure
 */
static void ECUi_PneuStep(ecu_control_t *control)
{
    static ecu_state_t prevState = ECU_STATE_IDLE;

    int8_t pctRemaining;
    ecu_inputs_t *inputs = &control->inputs;
    uint8_t pistonPct = ECUi_GetPistonPosPct(&control->inputs);
    bool overFilling = false;

    ASSERT_NOT(control == NULL);

    /** Pneumatic must not be blocked */
    if (!ECUi_PneuEnabled(&control->inputs)) {
        control->state = ECU_STATE_IDLE;
        ECU_ValvesClose();
        return;
    }

    /* Percent remaining to reach required filling pct */
    pctRemaining = abs((int16_t) pistonPct - control->piston_start_pct);
    pctRemaining = control->filling_pct - pctRemaining;

    if (pctRemaining <= 0) {
        overFilling = true;
    }
    /* Fallback endstop in move direction hit, consider filling pct done */
    if (inputs->endstop_back && control->dir == ECU_DIR_BACK) {
        overFilling = true;
    }
    if (inputs->endstop_front && control->dir == ECU_DIR_FRONT) {
        overFilling = true;
    }

    /* Next state */
    switch (control->state) {
        case ECU_STATE_IDLE:
            if (inputs->throttle) {
                control->state = ECU_STATE_MOVE;
            }
            break;
        case ECU_STATE_MOVE:
            if (overFilling) {
                if (control->deadtime_ms == 0) {
                    if (inputs->throttle) {
                        control->piston_start_pct = pistonPct;
                        control->state = ECU_STATE_MOVE;
                        if (control->dir == ECU_DIR_BACK) {
                            control->dir = ECU_DIR_FRONT;
                        } else {
                            control->dir = ECU_DIR_BACK;
                        }
                    } else {
                        control->state = ECU_STATE_IDLE;
                    }
                } else {
                    control->state = ECU_STATE_DEADPOINT;
                }
            }
            break;
        case ECU_STATE_DEADPOINT:
            if (control->timeout <= chVTGetSystemTime()) {
                if (inputs->throttle) {
                    control->state = ECU_STATE_MOVE;
                } else {
                    control->state = ECU_STATE_IDLE;
                }
            }
            break;

        default:
            Log_Error(LOG_SOURCE_APP, "Unexpected state occured\n");
            control->state = ECU_STATE_IDLE;
            break;
    }

    /* Actions on state change */
    if (prevState != control->state) {
        switch (control->state) {
            case ECU_STATE_MOVE:
                control->piston_start_pct = pistonPct;
                if (control->dir == ECU_DIR_BACK) {
                    control->dir = ECU_DIR_FRONT;
                } else {
                    control->dir = ECU_DIR_BACK;
                }
                break;
            case ECU_STATE_DEADPOINT:
                control->timeout = chTimeAddX(chVTGetSystemTime(),
                        TIME_MS2I(control->deadtime_ms));
                break;
            default:
                break;
        }
    }
    prevState = control->state;

    /* Set valves according to the current ECU state */
    switch (control->state) {
        case ECU_STATE_MOVE:
            if (control->dir == ECU_DIR_BACK) {
                ECU_ValvesMoveBack(control->dual);
            } else {
                ECU_ValvesMoveFront(control->dual);
            }
            break;
        case ECU_STATE_IDLE:
            ECU_ValvesClose();
            break;
        case ECU_STATE_DEADPOINT:
            if (control->dir == ECU_DIR_BACK) {
                ECU_ValvesBackOut(control->dual);
                ECU_ValvesFrontClose();
            } else {
                ECU_ValvesFrontOut(control->dual);
                ECU_ValvesBackClose();
            }
            break;
        default:
            Log_Error(LOG_SOURCE_APP, "Unexpected state %d", control->state);
            control->state = ECU_STATE_IDLE;
            break;
    }
}

static THD_FUNCTION(ECU_Thread, arg)
{
    ecu_control_t control;
    systime_t time;
    (void) arg;

    control.state = ECU_STATE_INIT;
    control.dir = ECU_DIR_BACK;

    Log_Info(LOG_SOURCE_APP, "Initializing ECU");

    while (1) {
        time = chTimeAddX(chVTGetSystemTime(), TIME_MS2I(ECU_LOOP_CYCLE_MS));
        ECU_GetInputs(&control.inputs);
        control.filling_pct = Race_GetFillingPct();
        control.deadtime_ms = Race_GetDeadpointMs();

        if (control.filling_pct == 0) {
            /* Debug mode, do nothing */
        } else if (control.state == ECU_STATE_INIT) {
            ECUi_EdgesInit(&control);
            control.piston_start_pct = ECUi_GetPistonPosPct(&control.inputs);
        } else {
            ECUi_PneuStep(&control);
        }
        chThdSleepUntil(time);
    }
}

uint8_t ECU_GetRawPistonPosPct(void)
{
    uint16_t len = Config_GetUint(CONFIG_UINT_PISTON_LEN);
    int16_t encoder = Encoderd_Get();
    uint16_t posPct;

    if (encoder < 0) {
        encoder = 0;
    }

    posPct = ((int32_t) encoder*100)/len;
    if (posPct > 100) {
        posPct = 100;
    }

    return posPct;
}

void ECU_Init(void)
{
    ECU_IOInit();
    Encoderd_Init();

    (void) chThdCreateStatic(ecui_thread_area, sizeof(ecui_thread_area),
                ECU_THREAD_PRIO, ECU_Thread, NULL);
}

/** @} */
