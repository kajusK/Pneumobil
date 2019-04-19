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
 * @file    wdg.c
 * @brief   Watchdog driver
 *
 * @addtogroup drivers
 * @{
 */

#include <ch.h>
#include <hal.h>

#include "drivers/wdg.h"

#define WDG_THREAD_PRIO   (NORMALPRIO + 1)

/** Stack and stuff for thread */
THD_WORKING_AREA(wdgi_thread_area, 32);

static const WDGConfig wdgdi_config = {
  STM32_IWDG_PR_64,
  STM32_IWDG_RL(1000),
#if STM32_IWDG_IS_WINDOWED
  STM32_IWDG_WIN_DISABLED
#endif
};

static THD_FUNCTION(Wdgd_Thread, arg)
{
    (void) arg;

    while (true) {
        wdgReset(&WDGD1);
        chThdSleepMilliseconds(50);
    }
}

void Wdgd_Init(void)
{
    wdgStart(&WDGD1, &wdgdi_config);

    (void) chThdCreateStatic(wdgi_thread_area, sizeof(wdgi_thread_area),
                    WDG_THREAD_PRIO, Wdgd_Thread, NULL);
}

/** @} */
