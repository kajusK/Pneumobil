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
 * @file    hmi/gui/setup.c
 * @brief   Setup screen
 *
 * @addtogroup gui
 * @{
 */

#include <gfx.h>
#include "gui/setup.h"

void Gui_SetupInit(GHandle ghTab)
{
#if 0
    GWidgetInit     wi;

    gwinWidgetClearInit(&wi);
    wi.g.show = gTrue;
    wi.g.parent = ghTab;
    wi.g.x = 0;
    wi.g.y = 0;
    wi.g.width = gwinGetInnerWidth(ghTab);
    wi.g.height = gwinGetInnerHeight(ghTab);

        //filling percent
        wi.g.parent = ghTabConfig;
        wi.g.width = 500; wi.g.height = 50;
        wi.g.x = tabwidth/2 - wi.g.width/2; wi.g.y = 20;
        wi.text = "Filling %";
        ghFillingSlider = gwinSliderCreate(0, &wi);
        gwinSliderSetPosition(ghFillingSlider, conf.pneu.filling);

        //turnaround time
        wi.g.width = 600;
        wi.g.height = LABEL_HEIGHT;
        wi.g.x = 10; wi.g.y = 100;
        wi.text = conf.pneu.turnaround_ms;
        ghLabelTurnaround = gwinLabelCreate(0, &wi);
        gwinLabelSetAttribute(ghLabelTurnaround, 300, "Turnaround time (ms):");

        wi.g.width = BUTTON_HEIGHT; wi.g.height = BUTTON_HEIGHT;
        wi.g.x = 450;
        wi.text = "-";
        ghBtTurnaroundDec = gwinButtonCreate(0, &wi);

        wi.g.x += wi.g.width + 10; wi.text = "+";
        ghBtTurnaroundInc = gwinButtonCreate(0, &wi);

        //backlight
        wi.g.width = 500; wi.g.height = 50;
        wi.g.x = tabwidth/2 - wi.g.width/2; wi.g.y = 280;
        wi.text = "Backlight";
        ghBacklightSlider = gwinSliderCreate(0, &wi);
        gwinSliderSetPosition(ghBacklightSlider, conf.gui.backlight);

        //screen flip
        wi.g.y += wi.g.height + 10;
        wi.g.x = 10;
        wi.g.width = 200; wi.g.height = BUTTON_HEIGHT;
        wi.text = "Screen flip: ";
        ghChScreenFlip = gwinCheckboxCreate(0, &wi);
        if (conf.gui.rotate)
                gwinCheckboxCheck(ghChScreenFlip, 1);

        //control buttons
        wi.g.width = BUTTON_WIDTH; wi.g.height = BUTTON_HEIGHT;
        wi.g.x = tabwidth-BUTTON_WIDTH-10;
        wi.g.y = tabheight - BUTTON_HEIGHT - 10;
        wi.text = "Save";
        ghBtSave = gwinButtonCreate(0, &wi);

        wi.g.x = 10;
        wi.text = "Reload";
        ghBtReload = gwinButtonCreate(0, &wi);

// ginputCalibrateMouse()
// gdispCloseFont(font);
// font_t font = gdispOpenFont("DejaVuSans32_aa");
#endif

}

/** @} */
