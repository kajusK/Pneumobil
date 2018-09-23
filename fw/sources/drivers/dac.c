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
 * @file    dac.c
 * @brief   DAC driver
 *
 * @addtogroup drivers
 * @{
 */
#include <hal.h>

#include "drivers/dac.h"

void Dacd_SetOutput(uint16_t out)
{
    dacPutChannelX(&DACD1, 1, out);
}

void Dacd_Init(void)
{
    DACConfig config;

    /** Init value of dac output */
    config.init = 0;
    config.datamode = DAC_DHRM_12BIT_RIGHT;
    config.cr = 0;

    dacStart(&DACD1, &config);
}

/** @} */

