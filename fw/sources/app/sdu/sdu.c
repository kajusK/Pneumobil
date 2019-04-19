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
 * @file    sdu/sdu.c
 * @brief   SDU logic
 *
 * @addtogroup app-sdu
 * @{
 */

#include <ch.h>
#include <hal.h>
#include <utils/assert.h>

#include "sdu.h"

typedef struct {
    uint16_t speed_dms;
    uint16_t speed_avg_dms;
    race_mode_t mode;
} sdu_state_t;

#define SDU_THREAD_PRIO NORMALPRIO
/** Stack and stuff for thread */
THD_WORKING_AREA(sdui_thread_area, 128);

static sdu_state_t sdui_state;
static uint8_t sdui_7seg_digits[3];

/**
 * Show requested digit on 7 seg display
 *
 * @param [in] num      0-9
 * @param [in] pos      Position of the segment to drive (0-2), 0 is left
 */
static void Sdui_Set7SegVal(uint8_t num, uint8_t pos)
{
    ASSERT_NOT(num > 9);

    palWriteLine(LINE_A, num & 0x01);
    palWriteLine(LINE_B, (num >> 1) & 0x01);
    palWriteLine(LINE_C, (num >> 2) & 0x01);
    palWriteLine(LINE_D, (num >> 3) & 0x01);

    switch (pos) {
        case 0:
            palSetLine(LINE_SEG1);
            palClearLine(LINE_SEG2);
            palClearLine(LINE_SEG3);
            break;
        case 1:
            palClearLine(LINE_SEG1);
            palSetLine(LINE_SEG2);
            palClearLine(LINE_SEG3);
            break;
        case 2:
            palClearLine(LINE_SEG1);
            palClearLine(LINE_SEG2);
            palSetLine(LINE_SEG3);
            break;
        default:
            palClearLine(LINE_SEG1);
            palClearLine(LINE_SEG2);
            palClearLine(LINE_SEG3);
            break;
    }
}

/**
 * Set digits to be shown on 7 seg based on given speed
 *
 * @param [in] speed_dms    Car speed
 */
static void Sdui_SetSpeed(uint16_t speed_dms)
{
    uint16_t speed;

    speed = (speed_dms * 36) / 10;
    if (speed > 999) {
        speed = 999;
    }

    sdui_7seg_digits[0] = speed / 100;

    speed -= sdui_7seg_digits[0]*100;
    sdui_7seg_digits[1] = speed / 10;

    speed -= sdui_7seg_digits[1]*10;
    sdui_7seg_digits[2] = speed;
}

/**
 * Set leds state
 *
 * @param [in] mode    Car mode
 */
static void Sdui_SetLeds(race_mode_t mode)
{
    switch (mode) {
        case RACE_MODE_LONG_DISTANCE:
            palSetLine(LINE_LED1);
            palClearLine(LINE_LED2);
            palSetLine(LINE_LED3);
            break;

        case RACE_MODE_ARCADE:
            palClearLine(LINE_LED1);
            palSetLine(LINE_LED2);
            palSetLine(LINE_LED3);
            break;

        case RACE_MODE_ACCELERATION:
            palSetLine(LINE_LED1);
            palSetLine(LINE_LED2);
            palClearLine(LINE_LED3);
            break;

        default:
            palSetLine(LINE_LED1);
            palSetLine(LINE_LED2);
            palSetLine(LINE_LED3);
            break;
    }
}

/**
 * Keeps care of the 7 seg display multiplexing
 */
static THD_FUNCTION(Sdu_Thread, arg)
{
    (void) arg;
    int i;

    while (true) {
        if (i >= 3) {
            i = 0;
        }
        Sdui_Set7SegVal(sdui_7seg_digits[i], i);
        i++;

        chThdSleepMilliseconds(1);
    }
}

void Sdu_SetCarState(uint16_t speed_dms, uint16_t speed_avg_dms, race_mode_t mode)
{
    sdui_state.speed_dms = speed_dms;
    sdui_state.speed_avg_dms = speed_avg_dms;
    sdui_state.mode = mode;

    if (mode == RACE_MODE_LONG_DISTANCE) {
        Sdui_SetSpeed(speed_avg_dms);
    } else {
        Sdui_SetSpeed(speed_dms);
    }

    Sdui_SetLeds(mode);
}

void Sdu_Init(void)
{
    (void) chThdCreateStatic(sdui_thread_area, sizeof(sdui_thread_area),
                SDU_THREAD_PRIO, Sdu_Thread, NULL);
}

/** @} */
