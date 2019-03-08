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
 * @file    can.c
 * @brief   Can driver
 *
 * @addtogroup drivers
 * @{
 */


#include <ch.h>
#include <hal.h>

#include "utils/assert.h"
#include "drivers/can.h"

#define CAND_THREAD_PRIO   NORMALPRIO
/* Define system clock for mcus with more domains */
#ifndef STM32_PCLK
    #define STM32_PCLK STM32_PCLK1
#endif

/** The can device to be used */
#define CAND JOIN(CAND, CAN_CAN_RX)

static cand_rx_cb_t candi_rx_cb;

/** Stack and stuff for thread */
THD_WORKING_AREA(candi_thread_area, 512);

static THD_FUNCTION(Cand_Thread, arg)
{
    (void) arg;
    CANRxFrame frame;
    msg_t res;

    while (true) {
        res = canReceiveTimeout(&CAND, CAN_ANY_MAILBOX, &frame, TIME_INFINITE);
        if (res != MSG_OK) {
            continue;
        }

        if (candi_rx_cb != NULL) {
            candi_rx_cb(&frame);
        }
    }
}

bool Cand_SendFrame(const CANTxFrame *frame)
{
    msg_t res;
    uint32_t retries = CAN_TRANSMIT_RETRIES;

    ASSERT_NOT(frame == NULL);

    while (retries-- != 0) {
        res = canTransmitTimeout(&CAND, CAN_ANY_MAILBOX, frame, chTimeMS2I(CAN_TIMEOUT_MS));
        if (res == MSG_OK) {
                break;
        }
    }
    if (res != MSG_OK) {
        return false;
    }
    return true;
}

void Cand_RegisterRxCb(cand_rx_cb_t cb)
{
    candi_rx_cb = cb;
}

void Cand_Init(uint32_t baudrate, const cand_filter_t *filters, uint8_t count)
{
    static CANConfig config;
    CANFilter driver_filters[STM32_CAN_MAX_FILTERS];
    uint32_t brp;

    for (int i = 0; i < count && i < STM32_CAN_MAX_FILTERS; i++) {
        driver_filters[i].filter = i;
        driver_filters[i].mode = 0; /* mask mode */
        driver_filters[i].scale = 1; /* 32 bits mode */
        driver_filters[i].assignment = 0; /* must be 0 */
        driver_filters[i].register1 = filters[i].id;
        driver_filters[i].register2 = filters[i].mask;
    }
    canSTM32SetFilters(&CAND, STM32_CAN_MAX_FILTERS,
            count, driver_filters);

    /* Auto wakeup, auto recovery, no auto retransmission */
    config.mcr = CAN_MCR_ABOM | CAN_MCR_AWUM | CAN_MCR_NART;
    /*
     * bit time = tq (3 + TS[3:0] + TS[2:0]) = tq * 16
     * tq = (BRP[9:0] + 1) * APB period (1/STM32_PCLK)
     */
    brp = STM32_PCLK/baudrate/16 - 1;
    config.btr = CAN_BTR_BRP(brp) | CAN_BTR_TS1(12) | CAN_BTR_TS2(1) | CAN_BTR_SJW(1);

    canStart(&CAND, &config);
    (void) chThdCreateStatic(candi_thread_area, sizeof(candi_thread_area),
                    CAND_THREAD_PRIO, Cand_Thread, NULL);
}

/** @} */
