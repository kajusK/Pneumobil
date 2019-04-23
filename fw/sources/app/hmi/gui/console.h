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
 * @file    hmi/gui/console.h
 * @brief   Car console screen
 *
 * @addtogroup gui
 * @{
 */

#ifndef __INCLUDE_HMI_GUI_CONSOLE_H
#define __INCLUDE_HMI_GUI_CONSOLE_H

#include <types.h>
#include <gfx.h>
#include <modules/log.h>

/**
 * Draw the console screen
 */
extern void Gui_ConsoleInit(GHandle ghTab);

/**
 * Callback for log module, will print received messages
 *
 * @param [in] log      Pointer to generated log message
 */
extern void Gui_ConsoleLogCb(const log_msg_t *log);

/**
 * Called on switching to this tab
 */
extern void Gui_ConsoleOnSelect(void);

/**
 * Update data on this screen
 */
extern void Gui_ConsoleUpdate(void);

/**
 * Process events for this and nested windows
 *
 * @param [in] ev       Event to be processed
 * @return True if event processed (found related object)
 */
extern bool Gui_ConsoleProcessEvent(GEvent *ev);

#endif

/** @} */
