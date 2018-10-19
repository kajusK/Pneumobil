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
 * @file    buttons.h
 * @brief   Utilities for buttons processing - debouncing and stuff
 *
 * @addtogroup utils
 * @{
 */

#ifndef __UTILS_BUTTONS_H
#define __UTILS_BUTTONS_H

#include <types.h>

/** Initial state of debouncing timer after reset */
#define BT_DEBOUNCE_TIME_DEF 0x0f

/** Debouncig structure */
typedef struct {
    uint8_t time;
    bool prev;
} bt_debounce_t;

/**
 * Type for edge detection state
 */
typedef enum {
    BT_EDGE_NONE,
    BT_EDGE_RISING,
    BT_EDGE_FALLING
} bt_edge_t;

/**
 * Debounce signal
 *
 * @param [in] signal Input signal
 * @param [in] debounce Debounce structure for given signal
 *
 * @return Debounced signal state
 */
extern bool Bt_Debounce(bool signal, bt_debounce_t *debounce);

/**
 * Detect edge on signal
 *
 * @param [in] signal Signal input
 * @param [in] prev_state Previous state of the signal
 *
 * @return BT_EDGE_NONE, BT_EDGE_RISING, BT_EDGE_FALLING
 */
extern bt_edge_t Bt_GetEdge(bool signal, bool *prev_state);

#endif

/** @} */
