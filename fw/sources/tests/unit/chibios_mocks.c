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
 * @file    chibi_mocks.c
 * @brief   Stubs and mocks of chibios functions
 *
 * @addtogroup tests
 * @{
 */

#include "ch.h"
#include <types.h>

systime_t chVTGetSystemTime(void)
{
    static uint32_t time = 1;
    return time++;
}

systime_t chVTTimeElapsedSinceX(systime_t time)
{
    (void) time;
    return 1;
}

void chThdSleep(uint32_t interval)
{
    (void) interval;
}

void chThdSleepUntil(uint32_t time)
{
    (void) time;
}

uint32_t chTimeAddX(uint32_t systime, uint32_t val)
{
    return systime + val;
}

uint32_t *chThdCreateStatic(void *wsp, size_t size,
                             uint32_t prio, void (*pf)(int arg), void *arg)
{
    (void) wsp;
    (void) size;
    (void) prio;
    (void) pf;
    (void) arg;

    return (uint32_t *) 123;
}

void chMtxLock(mutex_t *mp)
{
    (void) mp;
}

void chMtxUnlock(mutex_t *mp)
{
    (void) mp;
}

uint32_t chTimeI2S(uint32_t time)
{
    return time;
}

void chBSemReset(mutex_t *sem, bool b)
{
    (void) sem;
    (void) b;
}

msg_t chBSemWaitTimeout(mutex_t *sem, uint32_t timeout)
{
    (void) sem;
    (void) timeout;
    return MSG_OK;
}

void chBSemSignal(mutex_t *sem)
{
    (void) sem;
}

void chBSemObjectInit(mutex_t *sem, bool b)
{
    (void) sem;
    (void) b;
}

/** @} */
