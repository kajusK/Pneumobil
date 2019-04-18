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
 * @file    crs.c
 * @brief   Clock recovery system driver
 *
 * This driver allows to synchronize the system timer to USB clock for higher
 * precision
 *
 * @addtogroup drivers
 * @{
 */
#include <hal.h>
#include "drivers/crs.h"

void Crsd_SyncToUsb(void)
{
    CRS->CR = CRS_CR_AUTOTRIMEN | CRS_CR_CEN;
}

/** @} */

