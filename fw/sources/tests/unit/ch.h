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

typedef _Bool bool;
#define true 1
#define false 0

#define THD_WORKING_AREA(name, size) void *name
#define THD_FUNCTION(name, arg) void name(int arg)
#define MEMORYPOOL_DECL(name, size, align, foo) char name
#define MAILBOX_DECL(name, point, size) void *name = point
#define TIME_I2S(data) data
#define TIME_INFINITE -1
#define TIME_IMMEDIATE 0
#define NORMALPRIO 100

typedef void *msg_t;

uint32_t chVTGetSystemTime(void);
uint32_t *chThdCreateStatic(void *wsp, size_t size,
                             uint32_t prio, void (*pf)(int arg), void *arg);

#endif
