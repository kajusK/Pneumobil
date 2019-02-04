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
 * @file    spi.c
 * @brief   SPI driver
 *
 * @addtogroup drivers
 * @{
 */

#include <ch.h>
#include <hal.h>

#include "utils/assert.h"
#include "drivers/spi.h"

#define SPID_DISP JOIN(SPID, SPI_DISP_SCK)
#define SPID_COMM JOIN(SPID, SPI_COMM_SCK)

MUTEX_DECL(spidi_disp_mutex);
MUTEX_DECL(spidi_comm_mutex);

void SPId_Init(void)
{
    SPIConfig config;

    config.end_cb = NULL;
    config.ssline = (ioline_t) 0;
    /* SPI4 96M in up to 10 mhz BME, ntf 10M - 16 divider, 6M */
    config.cr1 = SPI_CR1_BR_0 | SPI_CR1_BR_1 | SPI_CR1_MSTR;
    config.cr2 = 0;
    spiStart(&SPID_COMM, &config);

    /* spi2 48M, 40M sd card, 2MHz touch controller - 32 divider to 1,5M */
    config.cr1 = SPI_CR1_BR_2 | SPI_CR1_MSTR;
    spiStart(&SPID_DISP, &config);
}

void SPId_Select(spid_device_t device)
{
    switch (device) {
        case SPID_LCD_TOUCH:
            palClearLine(LINE_LCD_T_CS);
            chMtxLock(&spidi_disp_mutex);
            break;
        case SPID_LCD_SD:
            palClearLine(LINE_LCD_SD_CS);
            chMtxLock(&spidi_disp_mutex);
            break;
        case SPID_RFM:
            palClearLine(LINE_RFM_CS);
            chMtxLock(&spidi_comm_mutex);
            break;
        case SPID_BME:
            palClearLine(LINE_BME_CS);
            chMtxLock(&spidi_comm_mutex);
            break;
        default:
            ASSERT(false);
            break;
    }
}

void SPId_Unselect(spid_device_t device)
{
    switch (device) {
        case SPID_LCD_TOUCH:
            palSetLine(LINE_LCD_T_CS);
            chMtxUnlock(&spidi_disp_mutex);
            break;
        case SPID_LCD_SD:
            palSetLine(LINE_LCD_SD_CS);
            chMtxUnlock(&spidi_disp_mutex);
            break;
        case SPID_RFM:
            palSetLine(LINE_RFM_CS);
            chMtxUnlock(&spidi_comm_mutex);
            break;
        case SPID_BME:
            palSetLine(LINE_BME_CS);
            chMtxUnlock(&spidi_comm_mutex);
            break;
        default:
            ASSERT(false);
            break;
    }
}

SPIDriver *SPId_GetDrv(spid_device_t device)
{
    switch (device) {
        case SPID_LCD_TOUCH:
        case SPID_LCD_SD:
            return &SPID_DISP;
            break;
        case SPID_RFM:
        case SPID_BME:
            return &SPID_COMM;
            break;
        default:
            ASSERT(false);
            break;
    }
}

/** @} */
