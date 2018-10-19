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
 * @file    buttons.c
 * @brief   Utilities for buttons processing - debouncing and stuff
 *
 * @addtogroup utils
 * @{
 */

#include "utils/assert.h"
#include "utils/buttons.h"

bool Bt_Debounce(bool signal, bt_debounce_t *debounce)
{
    ASSERT_NOT(debounce == NULL);

    /* wait until another pulse can be used */
    if (debounce->time != 0) {
        debounce->time >>= 1;
        return debounce->prev;
    }

    if (signal != debounce->prev) {
        debounce->prev = signal;
        debounce->time = BT_DEBOUNCE_TIME_DEF;
    }

    return signal;
}

bt_edge_t Bt_GetEdge(bool signal, bool *prev_state)
{
    ASSERT_NOT(prev_state == NULL);

    if (signal == *prev_state) {
        return BT_EDGE_NONE;
    }
    *prev_state = signal;

    if (signal) {
        return BT_EDGE_RISING;
    }
    return BT_EDGE_FALLING;
}

/** @} */
