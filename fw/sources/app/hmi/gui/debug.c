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
 * @file    hmi/gui/debug.c
 * @brief   Debug screen
 *
 * @addtogroup gui
 * @{
 */

#include <gfx.h>
#include "gui/debug.h"

void Gui_DebugInit(GHandle ghTab)
{
#if 0
    GWidgetInit     wi;

    gwinWidgetClearInit(&wi);
    wi.g.parent = ghTab;
    wi.g.show = gTrue;
    wi.g.x = 0;
    wi.g.y = 0;
    wi.g.width = gwinGetInnerWidth(ghTab);
    wi.g.height = gwinGetInnerHeight(ghTab);

        // inputs
        wi.g.width = 290; wi.g.height = LABEL_HEIGHT;
        wi.g.x = 10; wi.g.y = 10;
        wi.text = "Off";
        ghLabelBrake = gwinLabelCreate(0, &wi);
        gwinLabelSetAttribute(ghLabelBrake, 120, "Brake: ");
        wi.g.y += wi.g.height + 5;
        ghLabelThrottle = gwinLabelCreate(0, &wi);
        gwinLabelSetAttribute(ghLabelThrottle, 120, "Throttle: ");
        wi.g.y += wi.g.height + 5;
        ghLabelShifting = gwinLabelCreate(0, &wi);
        gwinLabelSetAttribute(ghLabelShifting, 120, "Shifting: ");

        // gearbox
        wi.g.y += wi.g.height + 5;
        ghLabelGear1 = gwinLabelCreate(0, &wi);
        gwinLabelSetAttribute(ghLabelGear1, 120, "Gear 1: ");
        wi.g.y += wi.g.height + 5;
        ghLabelGear2 = gwinLabelCreate(0, &wi);
        gwinLabelSetAttribute(ghLabelGear2, 120, "Gear 2: ");

        // current, voltage
        wi.g.y += wi.g.height + 5;
        wi.text = "N/A";
        ghLabelCurrent = gwinLabelCreate(0, &wi);
        gwinLabelSetAttribute(ghLabelCurrent, 120, "Current: ");
        wi.g.y += wi.g.height + 5;
        ghLabelVoltage = gwinLabelCreate(0, &wi);
        gwinLabelSetAttribute(ghLabelVoltage, 120, "Voltage: ");

        // gps
        wi.g.y = 10; wi.g.x = 350;
        ghLabelGpsFix = gwinLabelCreate(0, &wi);
        gwinLabelSetAttribute(ghLabelGpsFix, 150, "GPS fix: ");
        wi.g.y += wi.g.height + 5;
        ghLabelGpsLat = gwinLabelCreate(0, &wi);
        gwinLabelSetAttribute(ghLabelGpsLat, 150, "Latitude: ");
        wi.g.y += wi.g.height + 5;
        ghLabelGpsLon = gwinLabelCreate(0, &wi);
        gwinLabelSetAttribute(ghLabelGpsLon, 150, "Longitude: ");
        wi.g.y += wi.g.height + 5;
        ghLabelGpsSatellites = gwinLabelCreate(0, &wi);
        gwinLabelSetAttribute(ghLabelGpsSatellites, 150, "Satellites: ");

        // pressures
        wi.g.y += wi.g.height + 5;
        ghLabelPres1 = gwinLabelCreate(0, &wi);
        gwinLabelSetAttribute(ghLabelPres1, 150, "Press 1: ");
        wi.g.y += wi.g.height + 5;
        ghLabelPres2 = gwinLabelCreate(0, &wi);
        gwinLabelSetAttribute(ghLabelPres2, 150, "Press 2: ");
        wi.g.y += wi.g.height + 5;
        ghLabelPres3 = gwinLabelCreate(0, &wi);
        gwinLabelSetAttribute(ghLabelPres3, 150, "Press 3: ");

        // pressure regulator control
        wi.g.width = 700; wi.g.height = 40;
        wi.g.x = tabwidth/2 - wi.g.width/2;
        wi.g.y = tabheight - 2*(BUTTON_HEIGHT+5) - 2*wi.g.height - 30;
        wi.text = "Regulator pressure";
        ghRegulatorSet = gwinSliderCreate(0, &wi);
        gwinSliderSetPosition(ghRegulatorSet, 0);


        // Valves Control
        wi.g.y += wi.g.height + 20;
        wi.text = "Piston position";
        ghPistonPos = gwinProgressbarCreate(0, &wi);
        gwinProgressbarSetPosition(ghPistonPos, 50);

        wi.g.x = 10; wi.g.y += wi.g.height + 5;
        wi.g.width = 100; wi.g.height = BUTTON_HEIGHT;

        //left valve 1
        wi.text = "In";
        ghBtVLeft1In = gwinButtonCreate(0, &wi);
        wi.g.x += wi.g.width + 5; wi.text = "Out";
        ghBtVLeft1Out = gwinButtonCreate(0, &wi);
        wi.g.x += wi.g.width + 5; wi.text = "Close";
        ghBtVLeft1Close = gwinButtonCreate(0, &wi);

        //right valve 1
        wi.g.x = tabwidth - 3*(wi.g.width + 5);
        wi.text = "In";
        ghBtVRight1In = gwinButtonCreate(0, &wi);
        wi.g.x += wi.g.width + 5; wi.text = "Out";
        ghBtVRight1Out = gwinButtonCreate(0, &wi);
        wi.g.x += wi.g.width + 5; wi.text = "Close";
        ghBtVRight1Close = gwinButtonCreate(0, &wi);

        //left valve 2
        wi.g.x = 10; wi.g.y += wi.g.height + 10;
        wi.text = "In";
        ghBtVLeft2In = gwinButtonCreate(0, &wi);
        wi.g.x += wi.g.width + 5; wi.text = "Out";
        ghBtVLeft2Out = gwinButtonCreate(0, &wi);
        wi.g.x += wi.g.width + 5; wi.text = "Close";
        ghBtVLeft2Close = gwinButtonCreate(0, &wi);

        //right valve 2
        wi.g.x = tabwidth - 3*(wi.g.width + 5);
        wi.text = "In";
        ghBtVRight2In = gwinButtonCreate(0, &wi);
        wi.g.x += wi.g.width + 5; wi.text = "Out";
        ghBtVRight2Out = gwinButtonCreate(0, &wi);
        wi.g.x += wi.g.width + 5; wi.text = "Close";
        ghBtVRight2Close = gwinButtonCreate(0, &wi);

        //make closed buttons gray
        gwinSetEnabled(ghBtVLeft1Close, 0);
        gwinSetEnabled(ghBtVLeft2Close, 0);
        gwinSetEnabled(ghBtVRight1Close, 0);
        gwinSetEnabled(ghBtVRight2Close, 0);
#endif
}

bool Gui_DebugProcessEvent(GEvent *ev)
{
    return false;
}

/** @} */
