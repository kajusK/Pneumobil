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
 * @file    spi.h
 * @brief   SPI driver
 *
 * @addtogroup drivers
 * @{
 */

#ifndef __DRIVERS_SPI_H
#define __DRIVERS_SPI_H

#include <types.h>
#include <hal.h>

#include "version.h"

typedef enum {
    SPID_RFM,
#ifdef BOARD_HMI
    SPID_LCD_TOUCH,
    SPID_LCD_SD,
    SPID_BME,
#endif
} spid_device_t;

/**
 * Initialize SPI driver
 */
extern void SPId_Init(void);

/**
 * Set CS pin and lock access to corresponing device
 *
 * @param [in] device       Device to acquire
 */
extern void SPId_Select(spid_device_t device);

/**
 * Unset CS pin and unlock access to corresponing device
 *
 * @param [in] device       Device to release
 */
extern void SPId_Unselect(spid_device_t device);

/**
 * Get SPID driver for given device
 *
 * @param [in] device       Device to get driver for
 * @return Correcponsing SPI driver
 */
extern SPIDriver *SPId_GetDrv(spid_device_t device);

#endif

/** @} */
