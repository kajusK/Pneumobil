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

#ifdef BOARD_HMI
    #define SPID_DISP JOIN(SPID, SPI_DISP_SCK)
    #define SPID_COMM JOIN(SPID, SPI_COMM_SCK)

/*
 * Configuration for SPI driver for rf module
 *
 * SPI4 96M clk,  10M BME, rfm 10M - 16 divider to 6M
 */
static const SPIConfig spidi_conf_comm = {
    false,
    NULL, /* End callback */
    (SPI_CR1_BR_2 | SPI_CR1_MSTR), /* CR1 */
    0, /* CR2 */
};

/*
 * Configuration for SPI driver for LCD modules
 *
 * spi2 48M clk, 40M sd card, 2MHz touch controller - 32 divider to 1,5M
 */
static const SPIConfig spidi_conf_disp = {
    false,
    NULL, /* End callback */
    (SPI_CR1_BR_0 | SPI_CR1_BR_1 | SPI_CR1_MSTR), /* CR1 */
    0, /* CR2 */
};
#else
/*
 * Configuration for SPI driver for rf module
 *
 * SPI1 48M clk, rfm 10M - 16 divider to 6M
 */
static const SPIConfig spidi_conf = {
    false, /* circular */
    NULL, /* End callback */
    (SPI_CR1_BR_2 | SPI_CR1_MSTR), /* CR1 */
    0, /* CR2 */
};

#endif

void SPId_Init(void)
{
#ifdef BOARD_HMI
    spiStart(&SPID_COMM, &spidi_conf_comm);
    spiStart(&SPID_DISP, &spidi_conf_disp);
#else
    spiStart(&SPID1, &spidi_conf);

#endif
}

void SPId_Select(spid_device_t device)
{
    spiAcquireBus(SPId_GetDrv(device));

    switch (device) {
        case SPID_RFM:
            palClearLine(LINE_RFM_CS);
            break;
#ifdef BOARD_HMI
        case SPID_LCD_TOUCH:
            palClearLine(LINE_LCD_T_CS);
            break;
        case SPID_LCD_SD:
            palClearLine(LINE_LCD_SD_CS);
            break;
        case SPID_BME:
            palClearLine(LINE_BME_CS);
            break;
#endif
        default:
            ASSERT(false);
            break;
    }
}

void SPId_Unselect(spid_device_t device)
{
    switch (device) {
        case SPID_RFM:
            palSetLine(LINE_RFM_CS);
            break;
#ifdef BOARD_HMI
        case SPID_LCD_TOUCH:
            palSetLine(LINE_LCD_T_CS);
            break;
        case SPID_LCD_SD:
            palSetLine(LINE_LCD_SD_CS);
            break;
        case SPID_BME:
            palSetLine(LINE_BME_CS);
            break;
#endif
        default:
            ASSERT(false);
            break;
    }
    spiReleaseBus(SPId_GetDrv(device));
}

SPIDriver *SPId_GetDrv(spid_device_t device)
{
#ifdef BOARD_HMI
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
#else
    (void) device;
    return &SPID1;
#endif
}

/** @} */
