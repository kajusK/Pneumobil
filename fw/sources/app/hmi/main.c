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
 * @file    hmi/main.c
 * @brief   Main file for pneumobil HMI
 *
 * @addtogroup app-hmi
 * @{
 */

#include <ch.h>
#include <hal.h>

#include <drivers/wdg.h>
#include <drivers/i2c.h>
#include <drivers/spi.h>
#include <drivers/fatfs/sdc.h>
#include <modules/log.h>
#include <modules/config.h>
#include <modules/storage.h>
#include <modules/comm/comm.h>

#include "state.h"
#include "logger.h"
#include "version.h"
#include "gui/gui.h"
#include "rf.h"

int main(void) {
    int slowCount = 0;

    halInit();
    /** Kernel becomes thread, rtos is activated */
    chSysInit();

    Log_Init();
    Comm_Init();
    Log_Info(LOG_SOURCE_APP, "HMI is booting...");
    Log_Debug(LOG_SOURCE_APP, "SYSCLK=%u", STM32_SYSCLK);
    Log_Info(LOG_SOURCE_APP, "Version %d.%d", VER_MAJOR, VER_MINOR);
    Log_Info(LOG_SOURCE_APP, "Compiled: %s", __DATE__ "-" __TIME__);

    //Wdgd_Init();
    I2Cd_Init(true);
    Storage_Init();
    Storage_LoadAll();
    SDCd_Init();
    State_Init();
    Logger_Init();

    SPId_Init();
    //Rf_Init();
    Gui_Init();

    while (1) {
        Gui_Update();
        if (++slowCount == 5) {
            slowCount = 0;
            Comm_SendSystemStatus();
            palToggleLine(LINE_LED_SYS_ACTIVE);
        }
        chThdSleepMilliseconds(100);
    }
}

/** @} */
