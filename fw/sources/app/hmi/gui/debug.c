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

#include <modules/comm/comm.h>

#include "state.h"
#include "gui/gui.h"
#include "gui/debug.h"

#define RADIO_GROUP_F1 1
#define RADIO_GROUP_F2 2
#define RADIO_GROUP_B1 3
#define RADIO_GROUP_B2 4
#define RADIO_GROUP_MOVE 5

typedef struct {
    bool out1;
    bool out2;
    bool horn;
    bool brake;
    bool dual;
    valve_state_t front1;
    valve_state_t front2;
    valve_state_t back1;
    valve_state_t back2;
} gui_debug_t;

static GHandle ghLabelValveF1, ghLabelValveF2, ghLabelValveB1, ghLabelValveB2;
static GHandle ghRadioF1In, ghRadioF1Out, ghRadioF1Close;
static GHandle ghRadioF2In, ghRadioF2Out, ghRadioF2Close;
static GHandle ghRadioB1In, ghRadioB1Out, ghRadioB1Close;
static GHandle ghRadioB2In, ghRadioB2Out, ghRadioB2Close;

static GHandle ghLabelValves, ghLabelOutputs;
static GHandle ghChboxDual, ghChboxHorn, ghChboxBrake, ghChboxOut1, ghChboxOut2;
static GHandle ghChboxEnable;
static GHandle ghRadioFront, ghRadioBack, ghRadioClose;

static gui_debug_t guii_debug;

static void Guii_DebugSetValues(void)
{
    Comm_SendEcuDebug(guii_debug.front1, guii_debug.front2, guii_debug.back1,
            guii_debug.back2, guii_debug.horn, guii_debug.brake,
            guii_debug.out1, guii_debug.out2);
}

static void Guii_DebugValveDraw(GWidgetInit *wi, const char *name, GHandle *label,
        GHandle *in, GHandle *out, GHandle *close, uint8_t group)
{
    gCoord width, height;

    width = wi->g.width;
    height = wi->g.height;

    wi->text = name;
    *label = gwinLabelCreate(0, wi);

    wi->customDraw = gwinRadioDraw_Button;
    wi->g.y += wi->g.height + GUI_MARGIN;

    wi->g.width = GUI_BUTTON_WIDTH;
    wi->g.height = GUI_BUTTON_HEIGHT;
    wi->text = "In";
    *in = gwinRadioCreate(0, wi, group);

    wi->g.x += GUI_BUTTON_WIDTH + GUI_OFFSET_SPACING;
    wi->text = "Out";
    *out = gwinRadioCreate(0, wi, group);

    wi->g.x += GUI_BUTTON_WIDTH + GUI_OFFSET_SPACING;
    wi->text = "Close";
    *close = gwinRadioCreate(0, wi, group);

    wi->g.x -= GUI_BUTTON_WIDTH*2 + GUI_OFFSET_SPACING*2;
    wi->g.y += wi->g.height + GUI_OFFSET_SPACING;
    wi->g.width = width;
    wi->g.height = height;
    wi->customDraw = NULL;
}

void Gui_DebugInit(GHandle ghTab)
{
    GWidgetInit wi;
    gCoord tabWidth, tabHeight;
    font_t font;

    font = gwinGetDefaultFont();
    tabWidth = gwinGetInnerWidth(ghTab);
    tabHeight = gwinGetInnerHeight(ghTab);

    gwinWidgetClearInit(&wi);
    wi.g.parent = ghTab;
    wi.g.show = gTrue;
    wi.g.height = gdispGetFontMetric(font, fontHeight);
    wi.g.width = (tabWidth - 2*GUI_MARGIN)/2;

    /* Valves control */
    wi.g.x = GUI_MARGIN;
    wi.g.y = GUI_MARGIN;
    Guii_DebugValveDraw(&wi, "Front 1:", &ghLabelValveF1, &ghRadioF1In,
            &ghRadioF1Out, &ghRadioF1Close, RADIO_GROUP_F1);

    Guii_DebugValveDraw(&wi, "Front 2:", &ghLabelValveF2, &ghRadioF2In,
            &ghRadioF2Out, &ghRadioF2Close, RADIO_GROUP_F2);

    Guii_DebugValveDraw(&wi, "Back 1:", &ghLabelValveB1, &ghRadioB1In,
            &ghRadioB1Out, &ghRadioB1Close, RADIO_GROUP_B1);

    Guii_DebugValveDraw(&wi, "Back 2:", &ghLabelValveB2, &ghRadioB2In,
            &ghRadioB2Out, &ghRadioB2Close, RADIO_GROUP_B2);

    wi.g.y = GUI_MARGIN;
    wi.g.x = wi.g.width;
    wi.text = "Valve control:";
    ghLabelValves = gwinLabelCreate(0, &wi);

    wi.g.y += wi.g.height + GUI_MARGIN;
    wi.g.width = GUI_BUTTON_WIDTH;
    wi.g.height = GUI_BUTTON_HEIGHT;
    wi.customDraw = gwinCheckboxDraw_Button;
    wi.text = "Dual";
    ghChboxDual = gwinCheckboxCreate(0, &wi);

    wi.g.y += GUI_BUTTON_HEIGHT + GUI_OFFSET_SPACING;
    wi.customDraw = gwinRadioDraw_Button;
    wi.text = "Back";
    ghRadioBack = gwinRadioCreate(0, &wi, RADIO_GROUP_MOVE);

    wi.g.x += GUI_BUTTON_WIDTH + GUI_OFFSET_SPACING;
    wi.text = "Front";
    ghRadioFront = gwinRadioCreate(0, &wi, RADIO_GROUP_MOVE);

    wi.g.x += GUI_BUTTON_WIDTH + GUI_OFFSET_SPACING;
    wi.text = "Close";
    ghRadioClose = gwinRadioCreate(0, &wi, RADIO_GROUP_MOVE);

    /* Outputs control */
    wi.customDraw = NULL;
    wi.g.x = (tabWidth - 2*GUI_MARGIN)/2;
    wi.g.y += wi.g.height + GUI_OFFSET_SPACING;
    wi.text = "Outputs control:";
    wi.g.height = gdispGetFontMetric(font, fontHeight);
    ghLabelOutputs = gwinLabelCreate(0, &wi);

    wi.customDraw = gwinCheckboxDraw_Button;
    wi.g.y += wi.g.height + GUI_MARGIN;
    wi.g.width = GUI_BUTTON_WIDTH;
    wi.g.height = GUI_BUTTON_HEIGHT;
    wi.text = "Horn";
    ghChboxHorn = gwinCheckboxCreate(0, &wi);

    wi.g.x += wi.g.width + GUI_OFFSET_SPACING;
    wi.text = "Brake";
    ghChboxBrake = gwinCheckboxCreate(0, &wi);


    wi.g.x = (tabWidth - 2*GUI_MARGIN)/2;
    wi.g.y += wi.g.height + GUI_OFFSET_SPACING;

    wi.text = "Out1";
    ghChboxOut1 = gwinCheckboxCreate(0, &wi);

    wi.g.x += wi.g.width + GUI_OFFSET_SPACING;
    wi.text = "Out2";
    ghChboxOut2 = gwinCheckboxCreate(0, &wi);

    /* Mode control */
    wi.g.x = tabWidth - wi.g.width - GUI_MARGIN;
    wi.g.y = tabHeight - wi.g.height - GUI_MARGIN;
    wi.text = "Enable";
    ghChboxEnable = gwinCheckboxCreate(0, &wi);
}

void Gui_DebugUpdate(void)
{
    gwinCheckboxCheck(ghChboxEnable, State_GetRaceMode() == RACE_MODE_DEBUG);
}

bool Gui_DebugProcessEvent(GEvent *ev)
{
    GHandle handle;
    handle = ((GEventGWin *)ev)->gwin;

    switch (ev->type) {
        case GEVENT_GWIN_CHECKBOX:
            if (handle == ghChboxEnable) {
                if (gwinCheckboxIsChecked(ghChboxEnable)) {
                    State_SetRaceMode(RACE_MODE_DEBUG);
                } else {
                    State_SetRaceMode(RACE_MODE_ARCADE);
                }
            } else if (handle == ghChboxDual) {
                guii_debug.dual = gwinCheckboxIsChecked(ghChboxDual);
            } else if (handle == ghChboxHorn) {
                guii_debug.horn = gwinCheckboxIsChecked(ghChboxHorn);
                Guii_DebugSetValues();
            } else if (handle == ghChboxBrake) {
                guii_debug.brake = gwinCheckboxIsChecked(ghChboxBrake);
                Guii_DebugSetValues();
            } else if (handle == ghChboxOut1) {
                guii_debug.out1 = gwinCheckboxIsChecked(ghChboxOut1);
                Guii_DebugSetValues();
            } else if (handle == ghChboxOut2) {
                guii_debug.out2 = gwinCheckboxIsChecked(ghChboxOut2);
                Guii_DebugSetValues();
            } else {
                return false;
            }
            break;

        case GEVENT_GWIN_RADIO:
            /* Separate valve control */
            if (handle == ghRadioF1In) {
                guii_debug.front1 = VALVE_IN;
                Guii_DebugSetValues();
            } else if (handle == ghRadioF1Out) {
                guii_debug.front1 = VALVE_OUT;
                Guii_DebugSetValues();
            } else if (handle == ghRadioF1Close) {
                guii_debug.front1 = VALVE_CLOSED;
                Guii_DebugSetValues();

            } else if (handle == ghRadioF2In) {
                guii_debug.front2 = VALVE_IN;
                Guii_DebugSetValues();
            } else if (handle == ghRadioF2Out) {
                guii_debug.front2 = VALVE_OUT;
                Guii_DebugSetValues();
            } else if (handle == ghRadioF2Close) {
                guii_debug.front2 = VALVE_CLOSED;
                Guii_DebugSetValues();

            } else if (handle == ghRadioB1In) {
                guii_debug.back1 = VALVE_IN;
                Guii_DebugSetValues();
            } else if (handle == ghRadioB1Out) {
                guii_debug.back1 = VALVE_OUT;
                Guii_DebugSetValues();
            } else if (handle == ghRadioB1Close) {
                guii_debug.back1 = VALVE_CLOSED;
                Guii_DebugSetValues();

            } else if (handle == ghRadioB2In) {
                guii_debug.back2 = VALVE_IN;
                Guii_DebugSetValues();
            } else if (handle == ghRadioB2Out) {
                guii_debug.back2 = VALVE_OUT;
                Guii_DebugSetValues();
            } else if (handle == ghRadioB2Close) {
                guii_debug.back2 = VALVE_CLOSED;
                Guii_DebugSetValues();

            /* common valves control */
            } else if (handle == ghRadioFront) {
                guii_debug.front1 = VALVE_OUT;
                guii_debug.back1 = VALVE_IN;
                if (gwinCheckboxIsChecked(ghChboxDual)) {
                    guii_debug.front2 = VALVE_OUT;
                    guii_debug.back2 = VALVE_IN;
                } else {
                    guii_debug.front2 = VALVE_CLOSED;
                    guii_debug.back2 = VALVE_CLOSED;
                }
                Guii_DebugSetValues();
            } else if (handle == ghRadioBack) {
                guii_debug.front1 = VALVE_IN;
                guii_debug.back1 = VALVE_OUT;
                if (gwinCheckboxIsChecked(ghChboxDual)) {
                    guii_debug.front2 = VALVE_IN;
                    guii_debug.back2 = VALVE_OUT;
                } else {
                    guii_debug.front2 = VALVE_CLOSED;
                    guii_debug.back2 = VALVE_CLOSED;
                }
                Guii_DebugSetValues();
            } else if (handle == ghRadioClose) {
                guii_debug.front1 = VALVE_CLOSED;
                guii_debug.front2 = VALVE_CLOSED;
                guii_debug.back1 = VALVE_CLOSED;
                guii_debug.back2 = VALVE_CLOSED;
                Guii_DebugSetValues();
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
