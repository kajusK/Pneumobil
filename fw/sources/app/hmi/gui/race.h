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
 * @file    hmi/gui/race.h
 * @brief   Car race screen
 *
 * @addtogroup gui
 * @{
 */

#ifndef __HMI_GUI_RACE_H
#define __HMI_GUI_RACE_H

#include <types.h>
#include <gfx.h>

/**
 * Draw the race screen
 */
extern void Gui_RaceInit(GHandle ghTab);

/**
 * Called on switching to this tab
 */
extern void Gui_RaceOnSelect(void);

/**
 * Update values on Race screen
 */
extern void Gui_RaceUpdate(void);

/**
 * Process events for this and nested windows
 *
 * @param [in] ev       Event to be processed
 * @return True if event processed (found related object)
 */
extern bool Gui_RaceProcessEvent(GEvent *ev);

#endif

/** @} */
