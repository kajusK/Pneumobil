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
 * @file    hmi/gui/tabs.h
 * @brief   Car tabs screen
 *
 * @addtogroup gui
 * @{
 */

#ifndef __INCLUDE_HMI_GUI_TABS_H
#define __INCLUDE_HMI_GUI_TABS_H

#include <types.h>
#include <gfx.h>

typedef enum {
    TAB_RACE,
    TAB_STATUS,
    TAB_SETUP,
    TAB_DEBUG,
    TAB_CONSOLE,
    TAB_UNKNOWN,
} gui_tab_t;

/**
 * Draw the tabs screen
 */
extern void Gui_TabsInit(void);

/**
 * Get currently active tab
 *
 * @return Current tab
 */
extern gui_tab_t Gui_TabsGetActive(void);

/**
 * Process events for tabs and nested windows
 *
 * @param [in] ev       Event to be processed
 * @return True if event processed (found related object)
 */
extern bool Gui_TabsProcessEvent(GEvent *ev);

#endif

/** @} */
