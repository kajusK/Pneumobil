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
 * @file    sys.c
 * @brief   System driver - flash, reboot,...
 *
 * System functions handling - flash memory access, reboot, reset sources, etc.
 *
 * @addtogroup drivers
 * @{
 */
#include <ch.h>
#include <hal.h>

#include "modules/log.h"
#include "drivers/sys.h"

void Sysd_Reboot(void)
{
    Log_Warn(LOG_SOURCE_DRIVER, "Rebooting in 1 second!");
    chThdSleepMilliseconds(1000);
    NVIC_SystemReset();
}

/** @} */

