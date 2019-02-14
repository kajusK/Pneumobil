/*
    BUT pneumobil - Copyright (C) 2019 Jakub Kaderka.

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
 * @file    hmi/gui/gui.c
 * @brief   Generic GUI setup
 *
 * @addtogroup gui
 * @{
 */

#include <gfx.h>
#include <string.h>
#include <modules/config.h>
#include <modules/log.h>
#include <utils/assert.h>
#include "gui/tabs.h"
#include "gui/console.h"

#include "gui/gui.h"

static font_t guii_big_font;
static font_t guii_middle_font;
static font_t guii_norm_font;
static font_t guii_tiny_font;

/**
 * @brief Loading touchscreen calibration data for uGFX lib
 *
 * @param [in] instance     Unused
 * @param [out] data        Calibration data buffer
 * @param [in] sz           Size of calibration buffer
 *
 * @return True if suceeded
 */
gBool LoadMouseCalibration(unsigned instance, void* data, gMemSize sz)
{
    (void) instance;
    //Note: If you want to use the container as a pop-up or top-level window element, you might want to take a look at the frame widget instead.

    float calib[6];
    ASSERT(sz == sizeof(calib));

    calib[0] = Config_GetFloat(CONFIG_FLOAT_TOUCH_AX);
    calib[1] = Config_GetFloat(CONFIG_FLOAT_TOUCH_BX);
    calib[2] = Config_GetFloat(CONFIG_FLOAT_TOUCH_CX);
    calib[3] = Config_GetFloat(CONFIG_FLOAT_TOUCH_AY);
    calib[4] = Config_GetFloat(CONFIG_FLOAT_TOUCH_BY);
    calib[5] = Config_GetFloat(CONFIG_FLOAT_TOUCH_CY);

    if (calib[0] == 0 && calib[1] == 0) {
        return false;
    }

    memcpy(data, (void*)calib, sz);
    return true;
}

/**
 * @brief Saving touchscreen calibration data for uGFX lib
 *
 * @param [in] instance     Unused
 * @param [in] data         Calibration data buffer
 * @param [in] sz           Size of calibration buffer
 *
 * @return True if suceeded
 */
gBool SaveMouseCalibration(unsigned instance, const void* data, gMemSize sz)
{
    (void) instance;

    float calib[6];
    ASSERT(sz == sizeof(calib));
    memcpy((void *)calib, data, sz);

    Config_SetFloat(CONFIG_FLOAT_TOUCH_AX, calib[0]);
    Config_SetFloat(CONFIG_FLOAT_TOUCH_BX, calib[1]);
    Config_SetFloat(CONFIG_FLOAT_TOUCH_CX, calib[2]);
    Config_SetFloat(CONFIG_FLOAT_TOUCH_AY, calib[3]);
    Config_SetFloat(CONFIG_FLOAT_TOUCH_BY, calib[4]);
    Config_SetFloat(CONFIG_FLOAT_TOUCH_CY, calib[5]);

    return true;
}

void Gui_SetFont(gui_font_t font)
{
    switch (font) {
        case GUI_FONT_BIG:
            gwinSetDefaultFont(guii_big_font);
            break;
        case GUI_FONT_MIDDLE:
            gwinSetDefaultFont(guii_middle_font);
            break;
        case GUI_FONT_TINY:
            gwinSetDefaultFont(guii_tiny_font);
            break;
        case GUI_FONT_NORM:
        default:
            gwinSetDefaultFont(guii_norm_font);
            break;
    }
}

void Gui_LabelUpdate(GHandle label, const char *buf)
{
    /* No need to change, text is the same */
    if (strcmp(gwinGetText(label), buf) == 0) {
        return;
    }

    gwinSetText(label, buf, TRUE);
}

void Gui_Init(void)
{
    log_severity_t severity[LOG_SOURCE_COUNT];

    gfxInit();

    if (Config_GetBool(CONFIG_BOOL_LCD_ROTATE)) {
         gdispSetOrientation(GDISP_ROTATE_180);
    }

    gdispSetBacklight(Config_GetUint(CONFIG_UINT_BACKLIGHT));
    gdispSetContrast(Config_GetUint(CONFIG_UINT_CONTRAST));

    gwinSetDefaultStyle(&BlackWidgetStyle, FALSE);

    guii_tiny_font = gdispOpenFont("DejaVuSans12");
    guii_norm_font = gdispOpenFont("DejaVuSans24");
    guii_middle_font = gdispOpenFont("DejaVuSans32");
    guii_big_font = gdispScaleFont(gdispOpenFont("LargeNumbers"), 6, 9);
    Gui_SetFont(GUI_FONT_NORM);

    // listen for events
//    geventListenerInit(&gl);
//    gwinAttachListener(&gl);

    Gui_TabsInit();

    for (int i = 0; i < LOG_SOURCE_COUNT; i++) {
        severity[i] = LOG_SEVERITY_INFO;
    }
    Log_Subscribe(Gui_ConsoleLogCb, severity);
}

/** @} */
