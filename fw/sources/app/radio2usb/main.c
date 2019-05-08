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

#include <utils/crc.h>
#include <drivers/wdg.h>
#include <drivers/crs.h>
#include <drivers/rfm69.h>
#include <drivers/spi.h>
#include "usb.h"

#include "version.h"

#define NODE_ID 0x11
#define NETWORK_ID 0x20

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

static void rf_loop(void)
{
    uint8_t len, i, crc;
    uint8_t buf[128];

    len = rfm69_Receive(buf, sizeof(buf));
    if (len == 0) {
        return;
    }

    streamPut((BaseSequentialStream *)&SDU1, 0xff);
    streamPut((BaseSequentialStream *)&SDU1, len);
    crc = CRC8_Add(0xff, CRC8_INITIAL_VALUE);
    crc = CRC8_Add(len, crc);
    for (i = 0; i < len; i++) {
        streamPut((BaseSequentialStream *)&SDU1, buf[i]);
        crc = CRC8_Add(buf[i], crc);
    }
    streamPut((BaseSequentialStream *)&SDU1, crc);
}

int main(void)
{
    halInit();
    /** main() becomes thread, rtos is activated */
    chSysInit();

    usb_init();
    /* Synchronize system clock to usb sync events for precise USB timing */
    Crsd_SyncToUsb();
    SPId_Init();
    Wdgd_Init();

    if (rfm69_Init(NODE_ID, NETWORK_ID, true, false) == false) {
        chprintf((BaseSequentialStream *)&SDU1, "Failed to initialize RF node\r\n");
    }
    rfm69_SetBitrate(115200);

    chprintf((BaseSequentialStream *)&SDU1, "Waiting for data\r\n");

    while (1) {
        rf_loop();
    }
}

/** @} */
