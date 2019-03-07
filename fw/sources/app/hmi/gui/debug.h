/*
    BUT pneumobil - Copyright (C) 2019 Jakub Kaderka.

    This file is part of BUT pneumobil ECU.

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
 * @file    hmi/gui/debug.h
 * @brief   Car debug screen
 *
 * @addtogroup gui
 * @{
 */

#ifndef __INCLUDE_HMI_GUI_DEBUG_H
#define __INCLUDE_HMI_GUI_DEBUG_H

#include <types.h>
#include <gfx.h>

/**
 * Draw the debug screen
 */
extern void Gui_DebugInit(GHandle ghTab);

/**
 * Update data on this screen
 */
extern void Gui_DebugUpdate(void);

/**
 * Process events for this and nested windows
 *
 * @param [in] ev       Event to be processed
 * @return True if event processed (found related object)
 */
extern bool Gui_DebugProcessEvent(GEvent *ev);

#endif

/** @} */
