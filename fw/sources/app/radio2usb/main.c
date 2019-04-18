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
 * @file    radio2usb/main.c
 * @brief   Main file for pneumobil Radio2USB converter
 *
 * @addtogroup app-radio2usb
 * @{
 */

#include <ch.h>
#include <hal.h>
#include <chprintf.h>

#include <drivers/wdg.h>
#include <drivers/crs.h>
#include "usb.h"

#include "version.h"

static void usb_init(void)
{
    sduObjectInit(&SDU1);
    sduStart(&SDU1, &serusbcfg);

    /*
    * Activates the USB driver and then the USB bus pull-up on D+.
    * Note, a delay is inserted in order to not have to disconnect the cable
    * after a reset.
    */
    usbDisconnectBus(serusbcfg.usbp);
    chThdSleepMilliseconds(1500);
    usbStart(serusbcfg.usbp, &usbcfg);
    usbConnectBus(serusbcfg.usbp);
}

int main(void) {
    halInit();
    /** main() becomes thread, rtos is activated */
    chSysInit();

    usb_init();

    /* Synchronize system clock to usb sync events for precise USB timing */
    Crsd_SyncToUsb();

//    Wdgd_Init();

    while (1) {
        chThdSleepMilliseconds(500);
        chprintf((BaseSequentialStream *)&SDU1, "\r\n\nrunning\r\n");
    }
}

/** @} */
