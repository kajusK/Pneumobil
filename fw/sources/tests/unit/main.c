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
 * @file    main.c
 * @brief   Main file for unit tests
 *
 * @addtogroup tests
 * @{
 */

#include "main.h"

uint8_t assert_should_fail = false;

static void RunAll(void)
{
    CRC_RunTests();
    Buttons_RunTests();
    Log_RunTests();
    Comm_UartTransport_RunTests();
    Comm_CanTransport_RunTests();
    Comm_Session_RunTests();
    Eeprom_RunTests();
    Config_RunTests();

    ECU_RunTests();
}

int main(int argc, const char *argv[])
{
    UnityMain(argc, argv, RunAll);
}

/** @} */
