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

static GHandle ghLabelSpeedUnit;

static GHandle ghLabelSpeed, ghLabelDistance, ghLabelRaceTime, ghLabelSpeedAvg, ghLabelDatetime, ghLabelGear;
static GHandle ghLabelPress1, ghLabelPress2, ghLabelPress3;
static GHandle ghBtRaceReset, ghBtSetArcade, ghBtSetAcceleration, ghBtSetLongDist, ghBtHorn;

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
    wi.customDraw = NULL;
    Gui_SetFont(GUI_FONT_NORM);
    wi.g.width = 120;
    wi.g.height = GUI_BUTTON_HEIGHT;
    wi.g.y = tabHeight - wi.g.height - GUI_MARGIN;

    wi.g.x = GUI_MARGIN;
    wi.text = "Arcade";
    ghBtSetArcade = gwinButtonCreate(0, &wi);

    wi.g.x += wi.g.width + 10;
    wi.text = "Acceler.";
    ghBtSetAcceleration = gwinButtonCreate(0, &wi);

    wi.g.x += wi.g.width + 10;
    wi.text = "Long Dist";
    ghBtSetLongDist = gwinButtonCreate(0, &wi);

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
    chsnprintf(buf, BUFSIZE, "%02d:%02d  %02d.%02d. %d", timp.tm_hour, timp.tm_min,
            timp.tm_mday, timp.tm_mon + 1, timp.tm_year+1900);
    Gui_LabelUpdate(ghLabelDatetime, buf);
}

#if 0

/**
 * Periodic screen values updating
 */
void Gui_RaceUpdate(void)
{
    //average speed color change
}

#include <gfx.h>

/* Generic stuff */
static GListener gl;
static font_t font_big, font_norm;

/* Race tab */

/* Diagnostic tab */
static GHandle ghPistonPos;
static GHandle ghBtVLeft1In, ghBtVLeft1Out, ghBtVLeft1Close;
static GHandle ghBtVLeft2In, ghBtVLeft2Out, ghBtVLeft2Close;
static GHandle ghBtVRight1In, ghBtVRight1Out, ghBtVRight1Close;
static GHandle ghBtVRight2In, ghBtVRight2Out, ghBtVRight2Close;
static GHandle ghLabelBrake, ghLabelThrottle, ghLabelShifting;
static GHandle ghLabelGear1, ghLabelGear2;
static GHandle ghLabelVoltage, ghLabelCurrent;
static GHandle ghLabelGpsFix, ghLabelGpsLat, ghLabelGpsLon, ghLabelGpsSatellites;
static GHandle ghLabelPres1, ghLabelPres2, ghLabelPres3;
static GHandle ghRegulatorSet;

/* Setup tab */
static GHandle ghBtTurnaroundInc, ghBtTurnaroundDec;
static GHandle ghBtSave, ghBtReload;
static GHandle ghLabelTurnaround, ghFillingSlider;
static GHandle ghContrastSlider, ghBacklightSlider, ghChScreenFlip;

/* Debug tab */
static GHandle ghConsole;

/* default screen with runtime information - speed, distance, time,.. */
static void gui_create_views(void)
{

        /* ======================== Race ============================ */

        /* ======================== Diagnostic ============================ */
        wi.g.parent = ghTabDiag;


        /* ======================== Config ============================ */

        /* ======================== Console ============================ */
        wi.g.parent = ghTabConsole;
        wi.g.x = 0;
        wi.g.y = 0;
        wi.g.width = tabWidth;
        wi.g.height = tabHeight;
        ghConsole = gwinConsoleCreate(0, &wi.g);

        gwinSetColor(ghConsole, Red);
        gwinSetBgColor(ghConsole, Blue);
        gwinShow(ghConsole);
        gwinClear(ghConsole);
}

/* Save configured values */
static void gui_config_save(void)
{
        conf.gui.backlight = gwinSliderGetPosition(ghBacklightSlider);
        conf.gui.contrast = gwinSliderGetPosition(ghContrastSlider);
        conf.gui.rotate = gdispGetOrientation() == GDISP_ROTATE_180;
        //conf.pneu.turnaround is already set
        conf.pneu.filling = gwinSliderGetPosition(ghFillingSlider);

        //TODO show result
        if (config_write() != 0)
                ;
        //TODO send data to device
}

/* Replace current config by saved values */
static void gui_config_reload(void)
{
        //TODO show result
        if (config_read() != 0)
                ;

        gdispSetBacklight(conf.gui.backlight);
        gdispSetContrast(conf.gui.contrast);
        if (conf.gui.rotate)
                gdispSetOrientation(GDISP_ROTATE_180);
        else
                gdispSetOrientation(GDISP_ROTATE_0);

        gwinSliderSetPosition(ghBacklightSlider, conf.gui.backlight);
        gwinSliderSetPosition(ghContrastSlider, conf.gui.contrast);
        gwinSliderSetPosition(ghFillingSlider, conf.pneu.filling);
        gwinSetText(ghLabelTurnaround, conf.pneu.turnaround_ms, TRUE);
}

static void gui_valves_set(enum e_valve valve, enum e_valve_state state)
{
        GHandle bt_in, bt_close, bt_out;

        switch (valve) {
                case VALVE_LEFT_1:
                        bt_in = ghBtVLeft1In;
                        bt_out = ghBtVLeft1Out;
                        bt_close = ghBtVLeft1Close;
                        break;
                case VALVE_RIGHT_1:
                        bt_in = ghBtVRight1In;
                        bt_out = ghBtVRight1Out;
                        bt_close = ghBtVRight1Close;
                        break;
                case VALVE_LEFT_2:
                        bt_in = ghBtVLeft2In;
                        bt_out = ghBtVLeft2Out;
                        bt_close = ghBtVLeft2Close;
                        break;
                case VALVE_RIGHT_2:
                        bt_in = ghBtVRight2In;
                        bt_out = ghBtVRight2Out;
                        bt_close = ghBtVRight2Close;
                        break;
        }

        switch (state) {
                case VALVE_IN:
                        gwinSetEnabled(bt_in, 0);
                        gwinSetEnabled(bt_out, 1);
                        gwinSetEnabled(bt_close, 1);
                        break;
                case VALVE_OUT:
                        gwinSetEnabled(bt_out, 0);
                        gwinSetEnabled(bt_in, 1);
                        gwinSetEnabled(bt_close, 1);
                        break;
                case VALVE_CLOSE:
                        gwinSetEnabled(bt_close, 0);
                        gwinSetEnabled(bt_out, 1);
                        gwinSetEnabled(bt_in, 1);
                        break;
        }

        //TODO send command to data
}

/* if inc is true, increase turnaround time, decrease otherwise */
static void gui_set_turnaround(uint8_t inc)
{
        if (inc)
                conf.pneu.turnaround_ms++;
        else
                conf.pneu.turnaround_ms--;

        gwinSetText(ghLabelTurnaround, conf.pneu.turnaround_ms, TRUE);
}

static void gui_race_start_stop(void)
{
        //prepnout text na stop pokud je stop,
        //ghBtRaceStart = gwinButtonCreate(0, &wi);

}

static void gui_race_reset(void)
{

}

/* Enter diagnostic mode, switch pneu controller to diag */
static void gui_diag_start(void)
{

}

/* Exit diagnostic mode */
static void gui_diag_stop(void)
{

}

/* redraw speed, distance, time, valves,... */
void gui_update(uint8_t speed, uint16_t distance, time_t time)
{

}

/* print data to console */
void gui_puts_console(char *string)
{
        gwinPutString(ghConsole, string);
        //show unreaded message present
        gwinTabsetSetTitle(ghTabConsole, "Console (!)", FALSE);
        //TODO font a zmenit barvu labelu tabu ze obsahuje nova data

}

void gui_check_events(void)
{
        GEvent* pe;
        static GHandle handle;

        while(1) {
                // Get an Event
                pe = geventEventWait(&gl, 1);
                if (pe == NULL)
                        return;
                handle = ((GEventGWinSlider *)pe)->gwin;

                switch(pe->type) {
                case GEVENT_GWIN_SLIDER:
                        if (handle == ghBacklightSlider) {
                                gdispSetBacklight(((GEventGWinSlider *)pe)->position);
                        }
                        else if (handle == ghContrastSlider) {
                                gdispSetContrast(((GEventGWinSlider *)pe)->position);
                        }
                        break;

                case GEVENT_GWIN_CHECKBOX:
                        if (handle == ghChScreenFlip) {
                                if(((GEventGWinCheckbox*)pe)->isChecked)
                                        gdispSetOrientation(GDISP_ROTATE_0);
                                else
                                        gdispSetOrientation(GDISP_ROTATE_180);
                        }
                        break;

                case GEVENT_GWIN_BUTTON:
                        /* Race tab */
                        if (handle == ghBtRaceStart)
                                gui_race_start_stop();
                        else if (handle == ghBtRaceReset)
                                gui_race_reset();

                        /* Diagnostic tab */
                        else if (handle == ghBtVLeft1In)
                                gui_valves_set(VALVE_LEFT_1, VALVE_IN);
                        else if (handle == ghBtVLeft1Out)
                                gui_valves_set(VALVE_LEFT_1, VALVE_OUT);
                        else if (handle == ghBtVLeft1Close)
                                gui_valves_set(VALVE_LEFT_1, VALVE_CLOSE);
                        else if (handle == ghBtVLeft2In)
                                gui_valves_set(VALVE_LEFT_2, VALVE_IN);
                        else if (handle == ghBtVLeft2Out)
                                gui_valves_set(VALVE_LEFT_2, VALVE_OUT);
                        else if (handle == ghBtVLeft2Close)
                                gui_valves_set(VALVE_LEFT_2, VALVE_CLOSE);
                        //
                        else if (handle == ghBtVRight1In)
                                gui_valves_set(VALVE_RIGHT_1, VALVE_IN);
                        else if (handle == ghBtVRight1Out)
                                gui_valves_set(VALVE_RIGHT_1, VALVE_OUT);
                        else if (handle == ghBtVRight1Close)
                                gui_valves_set(VALVE_RIGHT_1, VALVE_CLOSE);
                        else if (handle == ghBtVRight2In)
                                gui_valves_set(VALVE_RIGHT_2, VALVE_IN);
                        else if (handle == ghBtVRight2Out)
                                gui_valves_set(VALVE_RIGHT_2, VALVE_OUT);
                        else if (handle == ghBtVRight2Close)
                                gui_valves_set(VALVE_RIGHT_2, VALVE_CLOSE);

                        /* Config tab */
                        else if (handle == ghBtTurnaroundDec)
                                gui_set_turnaround(0);
                        else if (handle == ghBtTurnaroundInc)
                                gui_set_turnaround(1);
                        else if (handle == ghBtSave)
                                gui_config_save();
                        else if (handle == ghBtReload)
                                gui_config_reload();
                        break;

                case GEVENT_GWIN_TABSET:
                        if (handle == ghTabDiag)
                                gui_diag_start();
                        gui_diag_stop();
                        break;
                }
        }
}

void gui_setup(void)
{
        gfxInit();

        if (conf.gui.rotate)
                gdispSetOrientation(GDISP_ROTATE_180);

        gdispSetBacklight(conf.gui.backlight);
        gdispSetContrast(conf.gui.contrast);

        font_norm = gdispOpenFont("DejaVuSans24");
        gwinSetDefaultFont(font_norm);
        gwinSetDefaultStyle(&WhiteWidgetStyle, FALSE);
        gdispClear(White);

        //font for large numbers
        font_big = gdispOpenFont("DejaVuSans32");

        // listen for events
        geventListenerInit(&gl);
        gwinAttachListener(&gl);

        gui_create_views();
}
#endif

/** @} */
