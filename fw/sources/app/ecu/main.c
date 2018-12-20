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
 * @file    ecu/main.c
 * @brief   Main file for pneumobil ECU
 *
 * @addtogroup app-ecu
 * @{
 */

#include <ch.h>
#include <hal.h>
#include <chprintf.h>

#include <modules/log.h>
#include <modules/config.h>
#include "ecu.h"

#define BLINK_THREAD_PRIO (NORMALPRIO - 2)
THD_WORKING_AREA(blink_thread_area, 32);

static THD_FUNCTION(Blink_Thread, arg)
{
    (void) arg;

    while (1) {
        ECU_ValvesMoveFront(true);
        palSetLine(LINE_LED_SYS_ACTIVE);
        chThdSleepMilliseconds(500);
        palClearLine(LINE_LED_SYS_ACTIVE);
        ECU_ValvesMoveBack(true);
        chThdSleepMilliseconds(500);
    }
}

/*
 * Application entry point.
 */
int main(void) {
  /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */
  halInit();
  chSysInit();

    (void) chThdCreateStatic(blink_thread_area, sizeof(blink_thread_area),
            BLINK_THREAD_PRIO, Blink_Thread, NULL);
  /*
   * Activates the serial driver 2 using the driver default configuration.
   */
  sdStart(&SD1, NULL);
  chprintf((BaseSequentialStream *)&SD1, "SYSCLK=%u\r\n", STM32_SYSCLK);

  /*
  chprintf(cfg->out, "*** Kernel:       %s\r\n", CH_KERNEL_VERSION);
  chprintf(cfg->out, "*** Compiled:     %s\r\n", __DATE__ " - " __TIME__);
#ifdef PORT_COMPILER_NAME
  chprintf(cfg->out, "*** Compiler:     %s\r\n", PORT_COMPILER_NAME);
#endif
  chprintf(cfg->out, "*** Architecture: %s\r\n", PORT_ARCHITECTURE_NAME);
#ifdef PORT_CORE_VARIANT_NAME
  chprintf(cfg->out, "*** Core Variant: %s\r\n", PORT_CORE_VARIANT_NAME);
#endif
  chprintf(cfg->out, "*** System Clock: %d\r\n", cfg->sysclk);
#ifdef PORT_INFO
  chprintf(cfg->out, "*** Port Info:    %s\r\n", PORT_INFO);
#endif
#ifdef PLATFORM_NAME
  chprintf(cfg->out, "*** Platform:     %s\r\n", PLATFORM_NAME);
#endif
#ifdef BOARD_NAME
  chprintf(cfg->out, "*** Test Board:   %s\r\n", BOARD_NAME);
#endif
*/

  /*
   * Normal main() thread activity, in this demo it does nothing except
   * sleeping in a loop and check the button state.
   */
  Config_Reset();
  Log_Init();
  Wdgd_Init();
  //ECU_Init();


  while (true) {
    chThdSleepMilliseconds(500);
    chprintf((BaseSequentialStream *)&SD1, "SYSCLK=%u\r\n", STM32_SYSCLK);
  }
}

/** @} */
