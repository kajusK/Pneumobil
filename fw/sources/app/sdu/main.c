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
 * @file    sdu/main.c
 * @brief   Main file for pneumobil SDU
 *
 * @addtogroup app-sdu
 * @{
 */

#include <ch.h>
#include <hal.h>

#include <drivers/wdg.h>
#include <modules/log.h>
#include <modules/config.h>
#include <modules/comm/comm.h>

#include "version.h"

int main(void) {
    halInit();
    /** Kernel becomes thread, rtos is activated */
    chSysInit();

    Comm_Init();
    Log_Init();
    Log_Info(LOG_SOURCE_ECU, "SDU is booting...");
    Log_Debug(LOG_SOURCE_ECU, "SYSCLK=%u", STM32_SYSCLK);
    Log_Info(LOG_SOURCE_ECU, "Version %d.%d", VER_MAJOR, VER_MINOR);
    Log_Info(LOG_SOURCE_ECU, "Compiled: %s", __DATE__ "-" __TIME__);

    Config_SetBool(0, 0);
    Wdgd_Init();

    Log_Info(LOG_SOURCE_ECU, "Init done");

    while (1) {
        Comm_SendSystemStatus();
        chThdSleepMilliseconds(500);
    }
}

/** @} */
