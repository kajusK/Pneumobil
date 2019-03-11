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
 * @file    hmi/gui/tabs.c
 * @brief   Drawing GUI tabs in main window
 *
 * @addtogroup gui
 * @{
 */

#include <gfx.h>

#include "gui/race.h"
#include "gui/status.h"
#include "gui/setup.h"
#include "gui/debug.h"
#include "gui/console.h"

#include "gui/tabs.h"

static GHandle ghTabset;
static GHandle ghTabRace, ghTabStatus, ghTabSetup, ghTabDebug, ghTabConsole;
static GHandle ghLabelMode, ghLabelBattery;

void Gui_TabsInit(void)
{
    GWidgetInit wi;

    gwinWidgetClearInit(&wi);
    wi.g.show = gTrue;
    wi.g.width = gdispGetWidth();
    wi.g.height = gdispGetHeight();
    wi.g.x = 0;
    wi.g.y = 0;

    ghTabset = gwinTabsetCreate(0, &wi, 0);
    ghTabRace = gwinTabsetAddTab(ghTabset, "Race", FALSE);
    ghTabStatus = gwinTabsetAddTab(ghTabset, "Status", FALSE);
    ghTabSetup = gwinTabsetAddTab(ghTabset, "Setup", FALSE);
    ghTabDebug = gwinTabsetAddTab(ghTabset, "Debug", FALSE);
    ghTabConsole = gwinTabsetAddTab(ghTabset, "Console", FALSE);

    Gui_RaceInit(ghTabRace);
    Gui_StatusInit(ghTabStatus);
    Gui_SetupInit(ghTabSetup);
    Gui_DebugInit(ghTabDebug);
    Gui_ConsoleInit(ghTabConsole);

    /*
    wi.g.width = 100;
    wi.g.height = 20;
    wi.g.y = 0;
    wi.g.x = gdispGetWidth() - wi.g.width;
    wi.text = "Unknown";
    ghLabelMode = gwinLabelCreate(0, &wi);

    wi.g.y = 0;
    wi.g.x -= wi.g.width;
    wi.text = "0 mA, 0%";
    ghLabelBattery = gwinLabelCreate(0, &wi);
    */
}

gui_tab_t Gui_TabsGetActive(void)
{
    if (gwinGetVisible(ghTabRace)) {
        return TAB_RACE;
    }
    if (gwinGetVisible(ghTabStatus)) {
        return TAB_STATUS;
    }
    if (gwinGetVisible(ghTabSetup)) {
        return TAB_SETUP;
    }
    if (gwinGetVisible(ghTabDebug)) {
        return TAB_DEBUG;
    }
    if (gwinGetVisible(ghTabConsole)) {
        return TAB_CONSOLE;
    }

    return TAB_UNKNOWN;
}

bool Gui_TabsUpdate(void)
{
    switch (Gui_TabsGetActive()) {
        case TAB_RACE:
            Gui_RaceUpdate();
            break;
        case TAB_STATUS:
            Gui_StatusUpdate();
            break;
        case TAB_SETUP:
            Gui_SetupUpdate();
            break;
        case TAB_DEBUG:
            Gui_DebugUpdate();
            break;
        case TAB_CONSOLE:
            Gui_ConsoleUpdate();
            break;
        default:
            return false;
            break;
    }
    return true;
}

bool Gui_TabsProcessEvent(GEvent *ev)
{
    if (ev->type == GEVENT_GWIN_TABSET) {
        if (((GEventGWinTabset *)ev)->ghPage == ghTabConsole) {
            Gui_ConsoleSetUrgent(false);
        }
        return true;
    }

    switch (Gui_TabsGetActive()) {
        case TAB_RACE:
            return Gui_RaceProcessEvent(ev);
        case TAB_STATUS:
            return Gui_StatusProcessEvent(ev);
        case TAB_SETUP:
            return Gui_SetupProcessEvent(ev);
        case TAB_DEBUG:
            return Gui_DebugProcessEvent(ev);
        case TAB_CONSOLE:
            return Gui_ConsoleProcessEvent(ev);
        default:
            return false;
    }
}

/** @} */
