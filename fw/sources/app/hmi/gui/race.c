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
 * @file    hmi/gui/race.c
 * @brief   Race screen
 *
 * @addtogroup gui
 * @{
 */

#include <time.h>
#include <string.h>
#include <gfx.h>
#include <chprintf.h>
#include "state.h"
#include "drivers/rtc.h"

#include "gui/gui.h"
#include "gui/race.h"

#define BUFSIZE 32
#define RADIO_GROUP_MODE 0

static GHandle ghLabelSpeedUnit;

static GHandle ghLabelSpeed, ghLabelDistance, ghLabelRaceTime, ghLabelSpeedAvg, ghLabelDatetime, ghLabelGear;
static GHandle ghLabelPress1, ghLabelPress2, ghLabelPress3;
static GHandle ghBtRaceReset, ghRadioSetArcade, ghRadioSetAcceleration, ghRadioSetLongDist, ghBtHorn;

void Gui_RaceInit(GHandle ghTab)
{
    GWidgetInit wi;
    gCoord tabWidth, tabHeight, freeHeight;
    font_t font;

    tabWidth = gwinGetInnerWidth(ghTab);
    tabHeight = gwinGetInnerHeight(ghTab);
    freeHeight = tabHeight - 2*GUI_MARGIN - GUI_BUTTON_HEIGHT;

    gwinWidgetClearInit(&wi);
    wi.g.parent = ghTab;
    wi.g.show = gTrue;

    /** Labels */
    Gui_SetFont(GUI_FONT_BIG);
    font = gwinGetDefaultFont();
    wi.g.height = gdispGetFontMetric(font, fontHeight);
    wi.g.width = gdispGetStringWidth("00.0", font);
    wi.g.x = tabWidth/2 - wi.g.width/2;
    wi.g.y = freeHeight/2 - wi.g.height/2;
    wi.customDraw = gwinLabelDrawJustifiedRight;
    wi.text = "00.0";
    ghLabelSpeed = gwinLabelCreate(0, &wi);

    Gui_SetFont(GUI_FONT_MIDDLE);
    font = gwinGetDefaultFont();

    wi.g.x = wi.g.x + wi.g.width + GUI_MARGIN;
    wi.g.y += wi.g.height;
    wi.g.width = gdispGetStringWidth("km/h:", font);
    wi.g.height = gdispGetFontMetric(font, fontHeight);
    wi.g.y -= wi.g.height;
    wi.text = "km/h";
    ghLabelSpeedUnit = gwinLabelCreate(0, &wi);


    wi.g.x = GUI_MARGIN;
    wi.g.y = freeHeight/2 - wi.g.height;
    wi.g.width = gdispGetStringWidth("00000 m", font);
    wi.text = "00000 m";
    ghLabelDistance = gwinLabelCreate(0, &wi);

    wi.g.width = gdispGetStringWidth("00.0 km/h", font);
    wi.g.x = tabWidth - wi.g.width - GUI_MARGIN;
    wi.text = "00.0 km/h";
    ghLabelSpeedAvg = gwinLabelCreate(0, &wi);

    wi.g.width = gdispGetStringWidth("00:00", font);
    wi.g.x = tabWidth/2-wi.g.width/2;
    wi.g.y = GUI_MARGIN;
    wi.text = "00:00";
    ghLabelRaceTime = gwinLabelCreate(0, &wi);


    Gui_SetFont(GUI_FONT_NORM);
    font = gwinGetDefaultFont();
    wi.g.height = gdispGetFontMetric(font, fontHeight);

    wi.g.x = GUI_MARGIN;
    wi.g.y = GUI_MARGIN;
    wi.g.width = tabWidth/2 - 2*GUI_MARGIN;
    wi.g.width = gdispGetStringWidth("12:00  31.12. 2099", font) + 5;
    wi.text = "12:00  31.12. 1970";
    ghLabelDatetime = gwinLabelCreate(0, &wi);

    wi.g.width = 150;
    wi.g.x = tabWidth - GUI_MARGIN - wi.g.width;
    wi.g.y = GUI_MARGIN;
    wi.text = "N";
    ghLabelGear = gwinLabelCreate(0, &wi);
    gwinLabelSetAttribute(ghLabelGear, 100, "Gear:");

    /* Pressures */
    Gui_SetFont(GUI_FONT_MIDDLE);
    font = gwinGetDefaultFont();
    wi.g.height = gdispGetFontMetric(font, fontHeight);

    wi.g.width = gdispGetStringWidth("1234 kPa", font);
    wi.g.x = GUI_MARGIN;
    wi.g.y = freeHeight - wi.g.height;
    wi.text = "0000 kPa";
    ghLabelPress1 = gwinLabelCreate(0, &wi);

    wi.g.x = tabWidth/2 - wi.g.width/2;
    ghLabelPress2 = gwinLabelCreate(0, &wi);

    wi.g.x = tabWidth - wi.g.width - GUI_MARGIN;
    ghLabelPress3 = gwinLabelCreate(0, &wi);

    /* buttons */
    Gui_SetFont(GUI_FONT_NORM);
    wi.g.width = 120;
    wi.g.height = GUI_BUTTON_HEIGHT;
    wi.g.y = tabHeight - wi.g.height - GUI_MARGIN;
    wi.customDraw = gwinRadioDraw_Button;

    wi.g.x = GUI_MARGIN;
    wi.text = "Arcade";
    ghRadioSetArcade = gwinRadioCreate(0, &wi, RADIO_GROUP_MODE);

    wi.g.x += wi.g.width + 10;
    wi.text = "Acceler.";
    ghRadioSetAcceleration = gwinRadioCreate(0, &wi, RADIO_GROUP_MODE);

    wi.g.x += wi.g.width + 10;
    wi.text = "Long Dist";
    ghRadioSetLongDist = gwinRadioCreate(0, &wi, RADIO_GROUP_MODE);

    wi.customDraw = NULL;
    wi.g.x = tabWidth/2 + 10;
    wi.text = "(Re)Start";
    ghBtRaceReset = gwinButtonCreate(0, &wi);

    wi.g.x = tabWidth - GUI_MARGIN - wi.g.width;
    wi.text = "Horn";
    ghBtHorn = gwinButtonCreate(0, &wi);
}

void Gui_RaceUpdate(void)
{
    char buf[BUFSIZE];
    struct tm timp;

    state_t *state = State_Get();

    chsnprintf(buf, BUFSIZE, "%4d kpa", state->pneu.press1_kpa);
    Gui_LabelUpdate(ghLabelPress1, buf);
    chsnprintf(buf, BUFSIZE, "%4d kpa", state->pneu.press2_kpa);
    Gui_LabelUpdate(ghLabelPress2, buf);
    chsnprintf(buf, BUFSIZE, "%4d kpa", state->pneu.press3_kpa);
    Gui_LabelUpdate(ghLabelPress3, buf);

    chsnprintf(buf, BUFSIZE, "%3.1f", state->car.speed_kmh);
    Gui_LabelUpdate(ghLabelSpeed, buf);
    chsnprintf(buf, BUFSIZE, "%3.1f km/h", state->car.speed_avg_kmh);
    Gui_LabelUpdate(ghLabelSpeedAvg, buf);
    chsnprintf(buf, BUFSIZE, "%5d m", state->car.distance_m);
    Gui_LabelUpdate(ghLabelDistance, buf);

    if (state->car.gear == 0) {
        Gui_LabelUpdate(ghLabelGear, "N");
    } else {
        chsnprintf(buf, BUFSIZE, "%d", state->car.gear);
        Gui_LabelUpdate(ghLabelGear, buf);
    }

    RTCd_GetTime(&timp);
    chsnprintf(buf, BUFSIZE, "%02d:%02d  %02d.%02d.%d", timp.tm_hour, timp.tm_min,
            timp.tm_mday, timp.tm_mon + 1, timp.tm_year+1900);
    Gui_LabelUpdate(ghLabelDatetime, buf);

    if (state->car.mode == RACE_MODE_ARCADE) {
        gwinRadioPress(ghRadioSetArcade);
    } else if (state->car.mode == RACE_MODE_ACCELERATION) {
        gwinRadioPress(ghRadioSetAcceleration);
    } else if (state->car.mode == RACE_MODE_LONG_DISTANCE) {
        gwinRadioPress(ghRadioSetLongDist);
    }
}

bool Gui_RaceProcessEvent(GEvent *ev)
{
    GHandle handle;
    handle = ((GEventGWin *)ev)->gwin;

    switch (ev->type) {
        case GEVENT_GWIN_RADIO:
            if (((GEventGWinRadio *)ev)->group != RADIO_GROUP_MODE) {
                return false;
            }

            if (handle == ghRadioSetArcade) {
                State_SetRaceMode(RACE_MODE_ARCADE);
            } else if (handle == ghRadioSetAcceleration) {
                State_SetRaceMode(RACE_MODE_ACCELERATION);
            } else if (handle == ghRadioSetLongDist) {
                State_SetRaceMode(RACE_MODE_LONG_DISTANCE);
            } else {
                return false;
            }
            break;

        case GEVENT_GWIN_BUTTON:
            if (handle == ghBtHorn) {
                //TODO
            } else if (handle == ghBtRaceReset) {
                State_SetRaceMode(State_GetRaceMode());
            } else {
                return false;
            }
            break;

        default:
            return false;
            break;
    }
    return true;
}

/** @} */
