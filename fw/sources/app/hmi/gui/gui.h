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
 * @file    hmi/gui/gui.h
 * @brief   Generic GUI setup
 *
 * @addtogroup gui
 * @{
 */

#ifndef __INCLUDE_HMI_GUI_GUI_H
#define __INCLUDE_HMI_GUI_GUI_H

#include <types.h>
#include <gfx.h>

/** Margin for placing items near edges */
#define GUI_MARGIN 5
/** Button dimensions */
#define GUI_BUTTON_WIDTH 100
#define GUI_BUTTON_HEIGHT 50
/** Space between lines */
#define GUI_LINES_SPACING 2
/** Space between attribute and label */
#define GUI_OFFSET_SPACING 10
/** Offset for indentation */
#define GUI_SUB_OFFSET 15

typedef enum {
    GUI_FONT_TINY,
    GUI_FONT_NORM,
    GUI_FONT_MIDDLE,
    GUI_FONT_BIG
} gui_font_t;

/**
 * Change default font
 *
 * @param [in] font Font to be selected
 */
extern void Gui_SetFont(gui_font_t font);

/**
 * Update label text if different from current value
 *
 * @param [in] label        Label object
 * @param [in] buf          Text buffer
 */
extern void Gui_LabelUpdate(GHandle label, const char *buf);

/**
 * Update values on current gui screen
 */
extern void Gui_Update(void);

/**
 * @brief Initialize the gui subsystem
 */
extern void Gui_Init(void);

#endif

/** @} */
