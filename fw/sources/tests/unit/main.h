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
 * @file    main.h
 * @brief   Main file for unit tests
 *
 * @addtogroup tests
 * @{
 */

#ifndef __MAIN_H_
#define __MAIN_H_

#include <unity_fixture.h>
#include <types.h>

extern void Log_RunTests(void);
extern void Comm_UartLink_RunTests(void);
extern void Comm_CanLink_RunTests(void);
extern void Eeprom_RunTests(void);
extern void CRC_RunTests(void);
extern void Config_RunTests(void);
extern void Buttons_RunTests(void);
extern void ECU_RunTests(void);

extern uint8_t assert_should_fail;

/** assert testing */
#define ASSERT(exp) \
    _Pragma("GCC diagnostic push") \
/*    _Pragma("GCC diagnostic warning \"-w\"") */ \
    if (assert_should_fail) { \
        TEST_ASSERT_FALSE(exp); \
        return; \
    } else { \
        TEST_ASSERT_TRUE(exp); \
    } \
    _Pragma("GCC diagnostic pop") \

#define ASSERT_NOT(exp) ASSERT(!(exp))

#define extern static

#endif

/** @} */
