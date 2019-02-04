/*
    BUT pneumobil - Copyright (C) 2018 Jakub Kaderka.

    This file is part of BUT pneumobil ECU.

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
 * @file    hmi/ugfx/gmouse_lld_ADS7843.h
 * @brief   ADS47843 SPI touch screen  driver code for uGFX
 *
 * @addtogroup app-hmi
 * @{
 */

#ifndef _GINPUT_LLD_MOUSE_BOARD_H
#define _GINPUT_LLD_MOUSE_BOARD_H

#include "drivers/spi.h"

// Resolution and Accuracy Settings
#define GMOUSE_ADS7843_PEN_CALIBRATE_ERROR              10
#define GMOUSE_ADS7843_PEN_CLICK_ERROR                  10
#define GMOUSE_ADS7843_PEN_MOVE_ERROR                   4
#define GMOUSE_ADS7843_FINGER_CALIBRATE_ERROR           20
#define GMOUSE_ADS7843_FINGER_CLICK_ERROR               25
#define GMOUSE_ADS7843_FINGER_MOVE_ERROR                20

// How much extra data to allocate at the end of the GMouse structure for the board's use
#define GMOUSE_ADS7843_BOARD_DATA_SIZE                  0

static bool_t init_board(GMouse* m, unsigned driverinstance) {
    (void) m;

    if (driverinstance) {
            return FALSE;
    }

    return TRUE;
}

static GFXINLINE bool_t getpin_pressed(GMouse* m) {
    (void) m;
    return (!palReadLine(LINE_LCD_T_IRQ));
}

static GFXINLINE void aquire_bus(GMouse* m) {
    (void) m;
    SPId_Select(SPI_LCD_TOUCH);
}

static GFXINLINE void release_bus(GMouse* m) {
    (void) m;
    SPId_Unselect(SPI_LCD_TOUCH);
}

static GFXINLINE uint16_t read_value(GMouse* m, uint16_t port) {
    (void) m;
    static uint8_t txbuf[3] = {0};
    static uint8_t rxbuf[3] = {0};

    txbuf[0] = port;
    spiExchange(SPId_GetDrv(SPID_TOUCH), 3, txbuf, rxbuf);

    return ((uint16_t) rxbuf[1] << 5) | (rxbuf[2] >> 3);
}

#endif /* _GINPUT_LLD_MOUSE_BOARD_H */
