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
 * @file    ch.h
 * @brief   Header files with Chibios defines for tests
 *
 * @addtogroup tests
 * @{
 */

#ifndef __CH_H_
#define __CH_H_

#include <inttypes.h>
#include <stdlib.h>

#ifndef bool
    typedef _Bool bool;
    #define true 1
    #define false 0
#endif

typedef uint32_t mutex_t;

#define THD_WORKING_AREA(name, size) void *name
#define THD_FUNCTION(name, arg) void name(int arg)
#define MEMORYPOOL_DECL(name, size, align, foo) char name
#define MAILBOX_DECL(name, point, size) void *name = point
#define TIME_I2S(data) data
#define TIME_I2MS(data) data
#define TIME_MS2I(data) data
#define TIME_INFINITE -1
#define TIME_IMMEDIATE 0
#define NORMALPRIO 100
#define MUTEX_DECL(name)    mutex_t name

typedef void *msg_t;
typedef uint32_t systime_t;

systime_t chVTGetSystemTime(void);
void chThdSleep(uint32_t interval);
void chThdSleepUntil(uint32_t time);
uint32_t chTimeAddX(uint32_t systime, uint32_t val);
uint32_t *chThdCreateStatic(void *wsp, size_t size,
                             uint32_t prio, void (*pf)(int arg), void *arg);
systime_t chVTTimeElapsedSinceX(systime_t time);
void chMtxLock(mutex_t *mp);
void chMtxUnlock(mutex_t *mp);

#endif

/** @} */
