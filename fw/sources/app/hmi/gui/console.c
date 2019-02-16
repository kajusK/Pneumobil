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
 * @file    hmi/gui/console.c
 * @brief   Debug console
 *
 * @addtogroup gui
 * @{
 */

#include <gfx.h>
#include <string.h>
#include <utils/assert.h>
#include <drivers/rtc.h>
#include <modules/log.h>

#include "gui/gui.h"
#include "gui/tabs.h"
#include "gui/console.h"

#define CONSOLE_BLACK '0'
#define CONSOLE_RED '1'
#define CONSOLE_GREEN '2'
#define CONSOLE_YELLOW '3'
#define CONSOLE_BLUE '4'
#define CONSOLE_MAGENTA '5'
#define CONSOLE_CYAN '6'
#define CONSOLE_WHITE '7'

static GHandle ghConsole;
static GHandle ghTabConsole;

void Gui_ConsoleInit(GHandle ghTab)
{
    GWidgetInit     wi;

    gwinWidgetClearInit(&wi);
    wi.g.parent = ghTab;
    wi.g.x = 0;
    wi.g.y = 0;
    wi.g.width = gwinGetInnerWidth(ghTab);
    wi.g.height = gwinGetInnerHeight(ghTab);

    Gui_SetFont(GUI_FONT_TINY);
    ghConsole = gwinConsoleCreate(0, &wi.g);
    gwinShow(ghConsole);
    gwinClear(ghConsole);
    gwinConsoleSetBuffer(ghConsole, gTrue);
    Gui_SetFont(GUI_FONT_NORM);

    ghTabConsole = ghTab;
}

void Gui_ConsoleLogCb(const log_msg_t *log)
{
    struct tm timeinfo;
    char buf[32];
    char color;

    ASSERT_NOT(log == NULL || log->src >= LOG_SOURCE_COUNT ||
            log->severity >= LOG_SEVERITY_COUNT);

    RTCd_GetTime(&timeinfo);
    asctime_r(&timeinfo, buf);
    /* Remove trailing new line */
    buf[strlen(buf)-2] = '\0';

    switch (log->severity) {
        case LOG_SEVERITY_ERROR:
            color = CONSOLE_RED;
            break;
        case LOG_SEVERITY_WARNING:
            color = CONSOLE_YELLOW;
            break;
        default:
            color = 'C';
            break;
    }

    gwinPrintf(ghConsole, "%s \033b[%s] %s\033B \033%c<%s>\033C: %s\n\r",
            buf, Log_GetModuleStr(log->module), Log_GetSourceStr(log->src),
            color, Log_GetSeverityStr(log->severity), log->msg);

    if (log->severity > LOG_SEVERITY_INFO && Gui_TabsGetActive() != TAB_CONSOLE) {
        Gui_ConsoleSetUrgent(true);
    }
}

void Gui_ConsoleSetUrgent(bool urgent)
{
    if (urgent) {
        gwinTabsetSetTitle(ghTabConsole, "Console (!)", FALSE);
    } else {
        gwinTabsetSetTitle(ghTabConsole, "Console", FALSE);
    }
}

bool Gui_ConsoleProcessEvent(GEvent *ev)
{
    (void) ev;
    return false;
}

/** @} */
