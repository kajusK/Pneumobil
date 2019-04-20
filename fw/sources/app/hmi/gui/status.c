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
 * @file    hmi/gui/status.c
 * @brief   Car status screen
 *
 * @addtogroup gui
 * @{
 */

#include <gfx.h>
#include <chprintf.h>
#include "state.h"
#include "drivers/fatfs/sdc.h"

#include "gui/gui.h"
#include "gui/status.h"

#define BUFSIZE 32

static GHandle ghLabel5V, ghLabel12V, ghLabel24V;

static GHandle ghLabelHmi, ghLabelEcu, ghLabelPsu, ghLabelSdu, ghLabelDbg;
static GHandle ghLabelBat1, ghLabelBat2, ghLabelCurrent, ghLabelCharge;
static GHandle ghLabel5VCur, ghLabel5VVolt, ghLabel12VCur, ghLabel12VVolt;
static GHandle ghLabel24VVolt, ghLabel24VCur;

static GHandle ghLabelSdCard, ghLabelRfLink, ghLabelPresssure, ghLabelTemp;
static GHandle ghLabelThrottle, ghLabelBrake, ghLabelHorn;
static GHandle ghLabelPress1, ghLabelPress2, ghLabelPress3;

static GHandle ghLabelFront1In, ghLabelFront2In, ghLabelFront1Out, ghLabelFront2Out;
static GHandle ghLabelBack1In, ghLabelBack2In, ghLabelBack1Out, ghLabelBack2Out;
static GHandle ghLabelEndstopBack, ghLabelEndstopFront;
static GHandle ghPistonProgress;

static void Gui_StatusUpdateNode(GHandle label, state_node_t *node)
{
    char buf[BUFSIZE];
    char const *state;
    if (node->online) {
        state = "On";
    } else {
        state= "Off";
    }

    chsnprintf(buf, BUFSIZE, "%s, %4d mV, up %4d s", state, node->core_mv,
            node->uptime_s/1000);
    Gui_LabelUpdate(label, buf);
}

void Gui_StatusInit(GHandle ghTab)
{
    GWidgetInit wi;
    gCoord tabWidth;
    gCoord labelOffset;
    font_t font = gwinGetDefaultFont();

    tabWidth = gwinGetInnerWidth(ghTab);

    gwinWidgetClearInit(&wi);
    wi.g.show = gTrue;
    wi.g.parent = ghTab;

    /* ==================== Left side =================== */
    wi.g.x = GUI_MARGIN;
    wi.g.y = GUI_MARGIN;
    wi.g.height = gdispGetFontMetric(font, fontHeight) + GUI_LINES_SPACING;
    wi.g.width = tabWidth/2 - GUI_MARGIN;
    labelOffset = gdispGetStringWidth("HMI:", font) + GUI_OFFSET_SPACING;

    /* Modules Status */
    wi.text = "Online";
    ghLabelHmi = gwinLabelCreate(0, &wi);
    gwinLabelSetAttribute(ghLabelHmi, labelOffset, "HMI:");
    gwinRedraw(ghLabelHmi);

    wi.text = "Offline";
    wi.g.y += wi.g.height;
    ghLabelEcu = gwinLabelCreate(0, &wi);
    gwinLabelSetAttribute(ghLabelEcu, labelOffset, "ECU:");

    wi.text = "Offline";
    wi.g.y += wi.g.height;
    ghLabelPsu = gwinLabelCreate(0, &wi);
    gwinLabelSetAttribute(ghLabelPsu, labelOffset, "PSU:");

    wi.text = "Offline";
    wi.g.y += wi.g.height;
    ghLabelSdu = gwinLabelCreate(0, &wi);
    gwinLabelSetAttribute(ghLabelSdu, labelOffset, "SDU:");

    wi.text = "Offline";
    wi.g.y += wi.g.height;
    ghLabelDbg = gwinLabelCreate(0, &wi);
    gwinLabelSetAttribute(ghLabelDbg, labelOffset, "DBG:");

    /* battery info */
    wi.g.y += wi.g.height;
    labelOffset = gdispGetStringWidth("Battery 1:", font) + GUI_OFFSET_SPACING;

    wi.text = "0 mV";
    wi.g.y += wi.g.height;
    ghLabelBat1 = gwinLabelCreate(0, &wi);
    gwinLabelSetAttribute(ghLabelBat1, labelOffset, "Battery 1:");

    wi.text = "0 mV";
    wi.g.y += wi.g.height;
    ghLabelBat2 = gwinLabelCreate(0, &wi);
    gwinLabelSetAttribute(ghLabelBat2, labelOffset, "Battery 2:");

    wi.text = "0 mA";
    wi.g.y += wi.g.height;
    ghLabelCurrent = gwinLabelCreate(0, &wi);
    gwinLabelSetAttribute(ghLabelCurrent, labelOffset, "Current:");

    wi.text = "0 %";
    wi.g.y += wi.g.height;
    ghLabelCharge = gwinLabelCreate(0, &wi);
    gwinLabelSetAttribute(ghLabelCharge, labelOffset, "Charge:");

    /* PSU branches */
    wi.g.y += wi.g.height;
    wi.g.y += wi.g.height*2/3;
    wi.g.width = gdispGetStringWidth("12 V", font) + GUI_MARGIN;
    wi.text = "5 V";
    ghLabel5V = gwinLabelCreate(0, &wi);

    wi.g.y += 2*wi.g.height;
    wi.text = "12 V";
    ghLabel12V = gwinLabelCreate(0, &wi);

    wi.g.y += 2*wi.g.height;
    wi.text = "24 V";
    ghLabel24V = gwinLabelCreate(0, &wi);


    wi.g.y -= 4*wi.g.height + wi.g.height*2/3;
    wi.g.x += wi.g.width;
    wi.g.width = tabWidth - wi.g.width;
    wi.text = "0 mV";
    ghLabel5VVolt = gwinLabelCreate(0, &wi);

    wi.g.y += wi.g.height;
    wi.text = "0 mA";
    ghLabel5VCur = gwinLabelCreate(0, &wi);

    wi.g.y += wi.g.height;
    wi.text = "0 mV";
    ghLabel12VVolt = gwinLabelCreate(0, &wi);

    wi.g.y += wi.g.height;
    wi.text = "0 mA";
    ghLabel12VCur = gwinLabelCreate(0, &wi);

    wi.g.y += wi.g.height;
    wi.text = "0 mV";
    ghLabel24VVolt = gwinLabelCreate(0, &wi);

    wi.g.y += wi.g.height;
    wi.text = "0 mA";
    ghLabel24VCur = gwinLabelCreate(0, &wi);


    /* ==================== Right side =================== */
    /* Generic HMI info */
    wi.g.x = tabWidth/2;
    wi.g.y = GUI_MARGIN;
    wi.g.width = tabWidth / 2;
    labelOffset = gdispGetStringWidth("Temperature:", font) + GUI_OFFSET_SPACING;

    wi.text = "None";
    ghLabelSdCard = gwinLabelCreate(0, &wi);
    gwinLabelSetAttribute(ghLabelSdCard, labelOffset, "SD Card:");

    wi.g.y += wi.g.height;
    wi.text = "Off";
    ghLabelRfLink = gwinLabelCreate(0, &wi);
    gwinLabelSetAttribute(ghLabelRfLink, labelOffset, "RF Link:");

    wi.g.y += wi.g.height;
    wi.text = "0 hPa";
    ghLabelPresssure = gwinLabelCreate(0, &wi);
    gwinLabelSetAttribute(ghLabelPresssure, labelOffset, "Pressure:");

    wi.g.y += wi.g.height;
    wi.text = "x C";
    ghLabelTemp = gwinLabelCreate(0, &wi);
    gwinLabelSetAttribute(ghLabelTemp, labelOffset, "Temperature:");

    /* Inputs */
    wi.g.y += 2*wi.g.height;
    wi.text = "Throttle";
    ghLabelThrottle = gwinLabelCreate(0, &wi);

    wi.g.y += wi.g.height;
    wi.text = "Brake";
    ghLabelBrake = gwinLabelCreate(0, &wi);

    wi.g.y += wi.g.height;
    wi.text = "Horn";
    ghLabelHorn = gwinLabelCreate(0, &wi);

    /* Pneumatics pressures */
    labelOffset = gdispGetStringWidth("Pressure 1:", font) + GUI_OFFSET_SPACING;
    wi.g.y += 2*wi.g.height;
    wi.text = "0 hPa";
    ghLabelPress1 = gwinLabelCreate(0, &wi);
    gwinLabelSetAttribute(ghLabelPress1, labelOffset, "Pressure 1:");

    wi.g.y += wi.g.height;
    wi.text = "0 hPa";
    ghLabelPress2 = gwinLabelCreate(0, &wi);
    gwinLabelSetAttribute(ghLabelPress2, labelOffset, "Pressure 2:");

    wi.g.y += wi.g.height;
    wi.text = "0 hPa";
    ghLabelPress3 = gwinLabelCreate(0, &wi);
    gwinLabelSetAttribute(ghLabelPress3, labelOffset, "Pressure 3:");

    /* Pneumatic circuits */
    wi.g.width = gdispGetStringWidth("Out", font) + GUI_OFFSET_SPACING;
    wi.g.y += wi.g.height;
    wi.text = "In";
    ghLabelFront1In = gwinLabelCreate(0, &wi);

    wi.g.x += wi.g.width;
    wi.text = "Out";
    ghLabelFront1Out = gwinLabelCreate(0, &wi);

    wi.g.x = tabWidth - wi.g.width - GUI_MARGIN;
    ghLabelBack1Out = gwinLabelCreate(0, &wi);

    wi.g.x -= wi.g.width;
    wi.text = "In";
    ghLabelBack1In = gwinLabelCreate(0, &wi);


    wi.g.x = tabWidth/2;
    wi.g.y += wi.g.height;
    wi.text = "In";
    ghLabelFront2In = gwinLabelCreate(0, &wi);

    wi.g.x += wi.g.width;
    wi.text = "Out";
    ghLabelFront2Out = gwinLabelCreate(0, &wi);

    wi.g.x = tabWidth - wi.g.width - GUI_MARGIN;
    ghLabelBack2Out = gwinLabelCreate(0, &wi);

    wi.g.x -= wi.g.width;
    wi.text = "In";
    ghLabelBack2In = gwinLabelCreate(0, &wi);


    wi.g.width = gdispGetStringWidth("Endstop ", font);
    wi.g.x = tabWidth/2;
    wi.g.y += wi.g.height;
    wi.text = "Endstop";
    ghLabelEndstopFront = gwinLabelCreate(0, &wi);

    wi.g.x = tabWidth - wi.g.width - GUI_MARGIN;
    ghLabelEndstopBack = gwinLabelCreate(0, &wi);

    wi.g.y += wi.g.height;
    wi.g.x = tabWidth/2;
    wi.g.width = tabWidth/2 - GUI_MARGIN;
    wi.g.height += 5;
    wi.text = "Piston";
    ghPistonProgress = gwinProgressbarCreate(NULL, &wi);
}

void Gui_StatusUpdate(void)
{
    char buf[BUFSIZE];

    state_t *state = State_Get();

    Gui_StatusUpdateNode(ghLabelHmi, &state->node.hmi);
    Gui_StatusUpdateNode(ghLabelEcu, &state->node.ecu);
    Gui_StatusUpdateNode(ghLabelPsu, &state->node.psu);
    Gui_StatusUpdateNode(ghLabelSdu, &state->node.sdu);
    Gui_StatusUpdateNode(ghLabelDbg, &state->node.dbg);

    chsnprintf(buf, BUFSIZE, "%4d mV", state->psu.battery.bat1_mv);
    Gui_LabelUpdate(ghLabelBat1, buf);
    chsnprintf(buf, BUFSIZE, "%4d mV", state->psu.battery.bat2_mv);
    Gui_LabelUpdate(ghLabelBat2, buf);
    chsnprintf(buf, BUFSIZE, "%4d mA", state->psu.battery.cur_ma);
    Gui_LabelUpdate(ghLabelCurrent, buf);
    chsnprintf(buf, BUFSIZE, "%3d %", state->psu.battery.charge_pct);
    Gui_LabelUpdate(ghLabelCharge, buf);

    chsnprintf(buf, BUFSIZE, "%4d mV", state->psu.psu5_mv);
    Gui_LabelUpdate(ghLabel5VVolt, buf);
    chsnprintf(buf, BUFSIZE, "%4d mA", state->psu.psu5_ma);
    Gui_LabelUpdate(ghLabel5VCur, buf);
    chsnprintf(buf, BUFSIZE, "%4d mV", state->psu.psu12_mv);
    Gui_LabelUpdate(ghLabel12VVolt, buf);
    chsnprintf(buf, BUFSIZE, "%4d mA", state->psu.psu12_ma);
    Gui_LabelUpdate(ghLabel12VCur, buf);
    chsnprintf(buf, BUFSIZE, "%4d mV", state->psu.psu24_mv);
    Gui_LabelUpdate(ghLabel24VVolt, buf);
    chsnprintf(buf, BUFSIZE, "%4d mA", state->psu.psu24_ma);
    Gui_LabelUpdate(ghLabel24VCur, buf);

    gwinSetVisible(ghLabelThrottle, state->car.throttle);
    gwinSetVisible(ghLabelBrake, state->car.brake);
    gwinSetVisible(ghLabelHorn, state->car.horn);

    chsnprintf(buf, BUFSIZE, "%4d kpa", state->pneu.press1_kpa);
    Gui_LabelUpdate(ghLabelPress1, buf);
    chsnprintf(buf, BUFSIZE, "%4d kpa", state->pneu.press2_kpa);
    Gui_LabelUpdate(ghLabelPress2, buf);
    chsnprintf(buf, BUFSIZE, "%4d kpa", state->pneu.press3_kpa);
    Gui_LabelUpdate(ghLabelPress3, buf);

    gwinSetVisible(ghLabelBack1In, state->pneu.valve_back1 == VALVE_IN);
    gwinSetVisible(ghLabelBack1Out, state->pneu.valve_back1 == VALVE_OUT);
    gwinSetVisible(ghLabelBack2In, state->pneu.valve_back1 == VALVE_IN);
    gwinSetVisible(ghLabelBack2Out, state->pneu.valve_back1 == VALVE_OUT);
    gwinSetVisible(ghLabelFront1In, state->pneu.valve_front1 == VALVE_IN);
    gwinSetVisible(ghLabelFront1Out, state->pneu.valve_front1 == VALVE_OUT);
    gwinSetVisible(ghLabelFront2In, state->pneu.valve_front1 == VALVE_IN);
    gwinSetVisible(ghLabelFront2Out, state->pneu.valve_front1 == VALVE_OUT);

    gwinSetVisible(ghLabelEndstopBack, state->pneu.endstop_back);
    gwinSetVisible(ghLabelEndstopFront, state->pneu.endstop_front);

    gwinProgressbarSetPosition(ghPistonProgress, state->pneu.piston_pct);

    if (SDCd_IsReady()) {
        Gui_LabelUpdate(ghLabelSdCard, "Mounted");
    } else {
        Gui_LabelUpdate(ghLabelSdCard, "None");
    }
}

bool Gui_StatusProcessEvent(GEvent *ev)
{
    (void) ev;
    return false;
}

/** @} */
