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
#include <chprintf.h>

#include <modules/config.h>
#include <modules/comm/comm.h>
#include <drivers/rtc.h>
#include <utils/assert.h>

#include "state.h"
#include "gui/gui.h"
#include "gui/setup.h"

#define BUFSIZE 32
#define SLIDER_HEIGHT 40

typedef struct {
    uint8_t filling_pct;
    uint16_t deadtime_ms;
} setup_param_t;


/* Gui elements */
static GHandle ghLabelTimeDay, ghLabelTimeMon, ghLabelTimeYear;
static GHandle ghLabelTimeHour, ghLabelTimeMin;
static GHandle ghBtTimeDayInc, ghBtTimeDayDec, ghBtTimeMonInc, ghBtTimeMonDec;
static GHandle ghBtTimeHourInc, ghBtTimeHourDec, ghBtTimeMinInc, ghBtTimeMinDec;
static GHandle ghBtTimeYearInc, ghBtTimeYearDec;
static GHandle ghFrameTime, ghBtTimeSave, ghBtTimeCancel;
static GHandle ghSliderBacklight, ghSliderContrast, ghBtLcdSet;
static GHandle ghFrameLcd, ghBtLcdSave, ghBtLcdCancel, ghChboxLcdRotate;

static GHandle ghLabelDatetime;
static GHandle ghBtTimeSet;


static GHandle ghBtArcadeFilDec, ghBtArcadeFilInc, ghBtArcadeDeadInc, ghBtArcadeDeadDec;
static GHandle ghSliderArcadeFil, ghSliderArcadeDead;
static GHandle ghLabelArcadeFil, ghLabelArcadeDead, ghLabelArcade;

static GHandle ghBtAccelFilDec, ghBtAccelFilInc, ghBtAccelDeadInc, ghBtAccelDeadDec;
static GHandle ghSliderAccelFil, ghSliderAccelDead;
static GHandle ghLabelAccelFil, ghLabelAccelDead, ghLabelAccel;

static GHandle ghBtLongFilDec, ghBtLongFilInc, ghBtLongDeadInc, ghBtLongDeadDec;
static GHandle ghSliderLongFil, ghSliderLongDead;
static GHandle ghLabelLongFil, ghLabelLongDead, ghLabelLong;

static GHandle ghBtParamSave, ghBtParamReload;

/* Mode parameters */
static setup_param_t guii_arcade;
static setup_param_t guii_acceleration;
static setup_param_t guii_long_dist;

/* Time setup data */
struct tm guii_tm;

static gCoord Guii_SetupGenUpDownBtn(GHandle parent, GHandle *up, GHandle *down,
        GHandle *label, const char *text, gCoord x, gCoord y)
{
    GWidgetInit wi;
    font_t font = gwinGetDefaultFont();

    gwinWidgetClearInit(&wi);
    wi.g.show = gTrue;
    wi.g.parent = parent;

    wi.g.height = gdispGetFontMetric(font, fontHeight);
    wi.g.width = gdispGetStringWidth(text, font) + GUI_OFFSET_SPACING;
    wi.g.x = x;
    wi.g.y = y + GUI_MARGIN + wi.g.height*3/2;

    wi.text = text;
    *label = gwinLabelCreate(0, &wi);

    wi.g.x += wi.g.width;
    wi.g.y = y;
    wi.g.height = 2*wi.g.height;
    wi.g.width = gdispGetStringWidth("+", font)*2;
    wi.text = "+";
    *up = gwinButtonCreate(0, &wi);
    wi.g.y += wi.g.height + GUI_MARGIN;
    wi.text = "-";
    *down = gwinButtonCreate(0, &wi);

    return wi.g.x += wi.g.width + GUI_OFFSET_SPACING;
}

/**
 * Create frame for setting the current time
 *
 * @param ghTab     Tab to draw frame to
 */
static void Guii_SetupTimeInit(GHandle ghTab)
{
    GWidgetInit wi;
    gCoord x, y;

    gwinWidgetClearInit(&wi);
    wi.g.show = gFalse;
    wi.g.parent = ghTab;

    wi.g.width = 530;
    wi.g.height = 200;
    wi.g.x = gwinGetInnerWidth(ghTab)/2 - wi.g.width/2;
    wi.g.y = gwinGetInnerHeight(ghTab)/2 - wi.g.height/2;
    wi.text = "Time setup";
    ghFrameTime = gwinFrameCreate(0, &wi, 0);

    x = GUI_MARGIN;
    y = GUI_MARGIN;
    wi.g.show = gTrue;

    x = Guii_SetupGenUpDownBtn(ghFrameTime, &ghBtTimeDayInc, &ghBtTimeDayDec,
            &ghLabelTimeDay, "31", x, y);
    x = Guii_SetupGenUpDownBtn(ghFrameTime, &ghBtTimeMonInc, &ghBtTimeMonDec,
            &ghLabelTimeMon, "12", x, y);
    x = Guii_SetupGenUpDownBtn(ghFrameTime, &ghBtTimeYearInc, &ghBtTimeYearDec,
            &ghLabelTimeYear, "2018", x, y);

    x += GUI_OFFSET_SPACING*2;
    x = Guii_SetupGenUpDownBtn(ghFrameTime, &ghBtTimeHourInc, &ghBtTimeHourDec,
            &ghLabelTimeHour, "11", x, y);
    x = Guii_SetupGenUpDownBtn(ghFrameTime, &ghBtTimeMinInc, &ghBtTimeMinDec,
            &ghLabelTimeMin, "59", x, y);

    wi.g.parent = ghFrameTime;
    wi.g.x = gwinGetInnerWidth(ghFrameTime) - GUI_BUTTON_WIDTH - GUI_MARGIN;
    wi.g.y = gwinGetInnerHeight(ghFrameTime) - GUI_BUTTON_HEIGHT - GUI_MARGIN;
    wi.g.width = GUI_BUTTON_WIDTH;
    wi.g.height = GUI_BUTTON_HEIGHT;
    wi.text = "Save";
    ghBtTimeSave = gwinButtonCreate(0, &wi);

    wi.g.x -= wi.g.width + GUI_OFFSET_SPACING;
    wi.text = "Cancel";
    ghBtTimeCancel = gwinButtonCreate(0, &wi);
}

static void Guii_SetupLcdInit(GHandle ghTab)
{
    GWidgetInit wi;

    gwinWidgetClearInit(&wi);
    wi.g.show = gFalse;
    wi.g.parent = ghTab;

    wi.g.width = 530;
    wi.g.height = 200;
    wi.g.x = gwinGetInnerWidth(ghTab)/2 - wi.g.width/2;
    wi.g.y = gwinGetInnerHeight(ghTab)/2 - wi.g.height/2;
    wi.text = "LCD setup";
    ghFrameLcd = gwinFrameCreate(0, &wi, 0);

    wi.g.parent = ghFrameLcd;
    wi.g.show = gTrue;
    wi.g.x = GUI_MARGIN;
    wi.g.y = GUI_MARGIN;
    wi.g.width = wi.g.width - GUI_MARGIN*2;
    wi.g.height = SLIDER_HEIGHT;
    wi.text = "Backlight";
    ghSliderBacklight = gwinSliderCreate(0, &wi);

    wi.g.y += wi.g.height + GUI_MARGIN;
    wi.text = "Contrast";
    ghSliderContrast = gwinSliderCreate(0, &wi);

    wi.g.y = gwinGetInnerHeight(ghFrameTime) - GUI_BUTTON_HEIGHT - GUI_MARGIN;
    wi.g.width = GUI_BUTTON_WIDTH;
    wi.g.height = GUI_BUTTON_HEIGHT;

    wi.g.x = GUI_MARGIN;
    wi.text = "Rotate";
    wi.customDraw = gwinCheckboxDraw_Button;
    ghChboxLcdRotate = gwinCheckboxCreate(0, &wi);

    wi.customDraw = NULL;
    wi.g.x = gwinGetInnerWidth(ghFrameTime) - GUI_BUTTON_WIDTH - GUI_MARGIN;
    wi.text = "Save";
    ghBtLcdSave = gwinButtonCreate(0, &wi);

    wi.g.x -= wi.g.width + GUI_OFFSET_SPACING;
    wi.text = "Cancel";
    ghBtLcdCancel = gwinButtonCreate(0, &wi);
}

static void Guii_SetupGenSlide(GWidgetInit *wi, const char *str, GHandle *label,
        GHandle *up, GHandle *down, GHandle *slider, gCoord width)
{
    font_t font = gwinGetDefaultFont();

    wi->g.height = gdispGetFontMetric(font, fontHeight);
    wi->g.x += GUI_SUB_OFFSET;
    wi->g.y += SLIDER_HEIGHT / 2 - wi->g.height/2;
    wi->g.width = width;
    wi->text = str;
    *label = gwinLabelCreate(0, wi);

    wi->g.x += width + GUI_OFFSET_SPACING;
    wi->g.y -= SLIDER_HEIGHT / 2 - wi->g.height/2;
    wi->g.height = SLIDER_HEIGHT;

    wi->g.width = gdispGetStringWidth("-", font) + 40;
    wi->text = "-";
    *down = gwinButtonCreate(0, wi);

    wi->g.x += wi->g.width + GUI_OFFSET_SPACING;
    wi->g.width = 400;
    wi->text = "0";
    *slider = gwinSliderCreate(0, wi);
    gwinSliderSendExtendedEvents(*slider, GSLIDER_EVENT_MOVE);

    wi->g.x += wi->g.width + GUI_OFFSET_SPACING;
    wi->g.width = gdispGetStringWidth("-", font) + 40;
    wi->text = "+";
    *up = gwinButtonCreate(0, wi);

    wi->g.y += wi->g.height + GUI_MARGIN;
    wi->g.x = GUI_MARGIN;
}

static void Guii_SetupParamsLoad(void)
{
    Comm_ReadPneuParams(RACE_MODE_ARCADE, &guii_arcade.filling_pct,
            &guii_arcade.deadtime_ms);
    Comm_ReadPneuParams(RACE_MODE_ACCELERATION, &guii_acceleration.filling_pct,
            &guii_acceleration.deadtime_ms);
    Comm_ReadPneuParams(RACE_MODE_LONG_DISTANCE, &guii_long_dist.filling_pct,
            &guii_long_dist.deadtime_ms);

    gwinSliderSetPosition(ghSliderArcadeFil, guii_arcade.filling_pct);
    gwinSliderSetPosition(ghSliderArcadeDead, guii_arcade.deadtime_ms);
    gwinSliderSetPosition(ghSliderAccelFil, guii_acceleration.filling_pct);
    gwinSliderSetPosition(ghSliderAccelDead, guii_acceleration.deadtime_ms);
    gwinSliderSetPosition(ghSliderLongFil, guii_long_dist.filling_pct);
    gwinSliderSetPosition(ghSliderLongDead, guii_long_dist.deadtime_ms);

    gwinRedraw(ghSliderArcadeFil);
    gwinRedraw(ghSliderArcadeDead);
    gwinRedraw(ghSliderAccelFil);
    gwinRedraw(ghSliderAccelDead);
    gwinRedraw(ghSliderLongFil);
    gwinRedraw(ghSliderLongDead);
}

static void Guii_SetupParamsSave(void)
{
    Comm_SendPneuParams(RACE_MODE_ARCADE, guii_arcade.filling_pct,
            guii_arcade.deadtime_ms);
    Comm_SendPneuParams(RACE_MODE_ACCELERATION, guii_acceleration.filling_pct,
            guii_acceleration.deadtime_ms);
    Comm_SendPneuParams(RACE_MODE_LONG_DISTANCE, guii_long_dist.filling_pct,
            guii_long_dist.deadtime_ms);
}

static void Guii_SetupTimeLoad(void)
{
    RTCd_GetTime(&guii_tm);
}

static void Guii_SetupTimeSave(void)
{
    RTCd_SetTime(&guii_tm);
}

static void Guii_SetupLcdLoad(void)
{
    gwinSliderSetPosition(ghSliderBacklight,
            Config_GetUint(CONFIG_UINT_BACKLIGHT));
    gwinSliderSetPosition(ghSliderContrast,
            Config_GetUint(CONFIG_UINT_CONTRAST));
    gwinCheckboxCheck(ghChboxLcdRotate, Config_GetBool(CONFIG_BOOL_LCD_ROTATE));
}

static void Guii_SetupLcdSave(void)
{
    Config_SetUint(CONFIG_UINT_BACKLIGHT, gwinSliderGetPosition(ghSliderBacklight));
    Config_SetUint(CONFIG_UINT_CONTRAST, gwinSliderGetPosition(ghSliderContrast));
    Config_SetBool(CONFIG_BOOL_LCD_ROTATE, gwinCheckboxIsChecked(ghChboxLcdRotate));
}

/**
 * Check if the data from guii_tm are correct and if not, fix it
 *
 * This way tm items can be incremented/decremented from button handler and
 * oveflows are all handled in one place without huge blocks of code in handler
 */
static void Guii_SetupTimeFix(void)
{
    if (guii_tm.tm_hour > 23) {
        guii_tm.tm_hour = 0;
    } else if (guii_tm.tm_hour < 0) {
        guii_tm.tm_hour = 23;
    }

    if (guii_tm.tm_min > 59) {
        guii_tm.tm_min = 0;
    } else if (guii_tm.tm_min < 0) {
        guii_tm.tm_min = 59;
    }

    if (guii_tm.tm_mon > 11) {
        guii_tm.tm_mon = 0;
    } else if (guii_tm.tm_mon < 0) {
        guii_tm.tm_mon = 11;
    }

    if (guii_tm.tm_mday > 31) {
        guii_tm.tm_mday = 1;
    } else if (guii_tm.tm_mday <= 0) {
        guii_tm.tm_mday = 31;
    }

    if (guii_tm.tm_year <= 0) {
        guii_tm.tm_year = 1;
    }
}

/**
 * Update given filling value, check ranges
 *
 * @param [in,out] fil      Pointer to filling
 * @param inc               Increment if true, else decrement
 * @param [in,out] slider   Pointer to slider object (set position, redraw)
 */
static void Guii_SetupFilChange(uint8_t *fil, bool inc, GHandle *slider)
{
    ASSERT_NOT(fil == NULL || slider == NULL);

    if (inc) {
        if (*fil < 100) {
            *fil += 1;
        }
    } else {
        if (*fil > 0) {
            *fil -= 1;
        }
    }

    gwinSliderSetPosition(*slider, *fil);
    gwinRedraw(*slider);
}

/**
 * Update given deadtime value, check ranges
 *
 * @param [in,out] time_ms  Pointer to deadtime
 * @param inc               Increment if true, else decrement
 * @param [in,out] slider   Pointer to slider object (set position, redraw)
 */
static void Guii_SetupDeadChange(uint16_t *time_ms, bool inc, GHandle *slider)
{
    uint16_t max;

    ASSERT_NOT(time_ms == NULL || slider == NULL);

    if (*slider == ghSliderArcadeDead) {
        max = Config_GetUint(CONFIG_UINT_ARCADE_DEADTIME_MAX_MS);
    } else if (*slider == ghSliderAccelDead) {
        max = Config_GetUint(CONFIG_UINT_ACCELERATION_DEADTIME_MAX_MS);
    } else if (*slider == ghSliderLongDead) {
        max = Config_GetUint(CONFIG_UINT_LONG_DIST_DEADTIME_MAX_MS);
    }

    if (inc) {
        *time_ms += max/100;
        if (*time_ms > max) {
            *time_ms = max;
        }
    } else {
        if (*time_ms >= max/100) {
            *time_ms -= max/100;
        } else {
            *time_ms = 0;
        }
    }

    gwinSliderSetPosition(*slider, *time_ms);
    gwinRedraw(*slider);
}

void Gui_SetupInit(GHandle ghTab)
{
    GWidgetInit wi;
    gCoord tabWidth, width;
    font_t font;

    tabWidth = gwinGetInnerWidth(ghTab);
    font = gwinGetDefaultFont();

    gwinWidgetClearInit(&wi);
    wi.g.parent = ghTab;
    wi.g.show = gTrue;

    /* Time configuration */
    wi.g.width = gdispGetStringWidth("12:00  31.12. 2099", font) + GUI_MARGIN;
    wi.g.height = gdispGetFontMetric(font, fontHeight);
    wi.g.x = GUI_MARGIN;
    wi.g.y = GUI_MARGIN + wi.g.height/2;
    wi.text = "12:00  31.12. 1970";
    ghLabelDatetime = gwinLabelCreate(0, &wi);

    wi.g.x += wi.g.width + GUI_OFFSET_SPACING;
    wi.g.y = GUI_MARGIN;
    wi.g.width = GUI_BUTTON_WIDTH;
    wi.g.height = GUI_BUTTON_HEIGHT;
    wi.text = "Set";
    ghBtTimeSet = gwinButtonCreate(0, &wi);

    /* Lcd config */
    wi.g.x += wi.g.width + GUI_OFFSET_SPACING*2;
    wi.text = "LCD";
    ghBtLcdSet = gwinButtonCreate(0, &wi);

    /* Arcade parameters */
    wi.g.y += wi.g.height;
    wi.g.x = GUI_MARGIN;
    wi.g.height = gdispGetFontMetric(font, fontHeight);
    width = gdispGetStringWidth("Deadtime:", font) + GUI_MARGIN;

    wi.g.width = (tabWidth - 2*GUI_MARGIN)/2;
    wi.text = "Arcade";
    ghLabelArcade = gwinLabelCreate(0, &wi);
    wi.g.y += wi.g.height;
    Guii_SetupGenSlide(&wi, "Filling:", &ghLabelArcadeFil,
        &ghBtArcadeFilInc, &ghBtArcadeFilDec, &ghSliderArcadeFil, width);
    Guii_SetupGenSlide(&wi, "Deadtime:", &ghLabelArcadeDead,
        &ghBtArcadeDeadInc, &ghBtArcadeDeadDec, &ghSliderArcadeDead, width);

    wi.g.width = (tabWidth - 2*GUI_MARGIN)/2;
    wi.text = "Acceleration";
    ghLabelAccel = gwinLabelCreate(0, &wi);
    wi.g.y += wi.g.height;
    Guii_SetupGenSlide(&wi, "Filling:", &ghLabelAccelFil,
        &ghBtAccelFilInc, &ghBtAccelFilDec, &ghSliderAccelFil, width);
    Guii_SetupGenSlide(&wi, "Deadtime:", &ghLabelAccelDead,
        &ghBtAccelDeadInc, &ghBtAccelDeadDec, &ghSliderAccelDead, width);

    wi.g.width = (tabWidth - 2*GUI_MARGIN)/2;
    wi.text = "Long Distance";
    ghLabelLong = gwinLabelCreate(0, &wi);
    wi.g.y += wi.g.height;
    Guii_SetupGenSlide(&wi, "Filling:", &ghLabelLongFil,
        &ghBtLongFilInc, &ghBtLongFilDec, &ghSliderLongFil, width);
    Guii_SetupGenSlide(&wi, "Deadtime:", &ghLabelLongDead,
        &ghBtLongDeadInc, &ghBtLongDeadDec, &ghSliderLongDead, width);

    wi.g.width = GUI_BUTTON_WIDTH;
    wi.g.height = GUI_BUTTON_HEIGHT;
    wi.g.y = GUI_MARGIN;
    wi.g.x = tabWidth - GUI_BUTTON_WIDTH - GUI_MARGIN;
    wi.text = "Save";
    ghBtParamSave = gwinButtonCreate(0, &wi);

    wi.g.x -= GUI_BUTTON_WIDTH + GUI_OFFSET_SPACING;
    wi.text = "Reload";
    ghBtParamReload = gwinButtonCreate(0, &wi);

    /* Called last to get frame in foreground */
    Guii_SetupTimeInit(ghTab);
    Guii_SetupLcdInit(ghTab);

    /* Slider ranges */
    gwinSliderSetRange(ghSliderArcadeDead, 0,
            Config_GetUint(CONFIG_UINT_ARCADE_DEADTIME_MAX_MS));
    gwinSliderSetRange(ghSliderAccelDead, 0,
            Config_GetUint(CONFIG_UINT_ACCELERATION_DEADTIME_MAX_MS));
    gwinSliderSetRange(ghSliderLongDead, 0,
            Config_GetUint(CONFIG_UINT_LONG_DIST_DEADTIME_MAX_MS));
}

void Gui_SetupOnSelect(void)
{
    Guii_SetupParamsLoad();
}

void Gui_SetupUpdate(void)
{
    char buf[BUFSIZE];
    struct tm timp;

    RTCd_GetTime(&timp);
    chsnprintf(buf, BUFSIZE, "%02d:%02d  %02d.%02d.%d", timp.tm_hour, timp.tm_min,
            timp.tm_mday, timp.tm_mon + 1, timp.tm_year+1900);
    Gui_LabelUpdate(ghLabelDatetime, buf);

    chsnprintf(buf, BUFSIZE, "%d", guii_arcade.filling_pct);
    Gui_LabelUpdate(ghSliderArcadeFil, buf);
    chsnprintf(buf, BUFSIZE, "%d", guii_acceleration.filling_pct);
    Gui_LabelUpdate(ghSliderAccelFil, buf);
    chsnprintf(buf, BUFSIZE, "%d", guii_long_dist.filling_pct);
    Gui_LabelUpdate(ghSliderLongFil, buf);

    chsnprintf(buf, BUFSIZE, "%d", guii_arcade.deadtime_ms);
    Gui_LabelUpdate(ghSliderArcadeDead, buf);
    chsnprintf(buf, BUFSIZE, "%d", guii_acceleration.deadtime_ms);
    Gui_LabelUpdate(ghSliderAccelDead, buf);
    chsnprintf(buf, BUFSIZE, "%d", guii_long_dist.deadtime_ms);
    Gui_LabelUpdate(ghSliderLongDead, buf);

    if (gwinGetVisible(ghFrameTime)) {
        chsnprintf(buf, BUFSIZE, "%02d", guii_tm.tm_hour);
        Gui_LabelUpdate(ghLabelTimeHour, buf);
        chsnprintf(buf, BUFSIZE, "%02d", guii_tm.tm_min);
        Gui_LabelUpdate(ghLabelTimeMin, buf);
        chsnprintf(buf, BUFSIZE, "%02d", guii_tm.tm_mday);
        Gui_LabelUpdate(ghLabelTimeDay, buf);
        chsnprintf(buf, BUFSIZE, "%02d", guii_tm.tm_mon + 1);
        Gui_LabelUpdate(ghLabelTimeMon, buf);
        chsnprintf(buf, BUFSIZE, "%04d", guii_tm.tm_year+1900);
        Gui_LabelUpdate(ghLabelTimeYear, buf);
    }
}

bool Gui_SetupProcessEvent(GEvent *ev)
{
    GHandle handle;
    handle = ((GEventGWin *)ev)->gwin;

    switch (ev->type) {
        case GEVENT_GWIN_CHECKBOX:
            if (handle == ghChboxLcdRotate) {
                if (gwinCheckboxIsChecked(ghChboxLcdRotate)) {
                     gdispSetOrientation(GDISP_ROTATE_180);
                } else {
                     gdispSetOrientation(GDISP_ROTATE_0);
                }
            }
            break;
        case GEVENT_GWIN_BUTTON:
            /* Mode saving/loading */
            if (handle == ghBtParamSave) {
                Guii_SetupParamsSave();
            } else if (handle == ghBtParamReload) {
                Guii_SetupParamsLoad();
            /* Time saving/loading */
            } else if (handle == ghBtTimeSet) {
                Guii_SetupTimeLoad();
                gwinSetVisible(ghFrameTime, true);
            } else if (handle == ghBtTimeCancel) {
                gwinSetVisible(ghFrameTime, false);
            } else if (handle == ghBtTimeSave) {
                Guii_SetupTimeSave();
                gwinSetVisible(ghFrameTime, false);
            /* Gui saving/loading */
            } else if (handle == ghBtLcdSet) {
                Guii_SetupLcdLoad();
                gwinSetVisible(ghFrameLcd, true);
            } else if (handle == ghBtLcdCancel) {
                gwinSetVisible(ghFrameLcd, false);
            } else if (handle == ghBtLcdSave) {
                Guii_SetupLcdSave();
                gwinSetVisible(ghFrameLcd, false);
            /* Time inc/dec buttons */
            } else if (handle == ghBtTimeDayDec) {
                guii_tm.tm_mday--;
                Guii_SetupTimeFix();
            } else if (handle == ghBtTimeDayInc) {
                guii_tm.tm_mday++;
                Guii_SetupTimeFix();
            } else if (handle == ghBtTimeMonDec) {
                guii_tm.tm_mon--;
                Guii_SetupTimeFix();
            } else if (handle == ghBtTimeMonInc) {
                guii_tm.tm_mon++;
                Guii_SetupTimeFix();
            } else if (handle == ghBtTimeYearDec) {
                guii_tm.tm_year--;
                Guii_SetupTimeFix();
            } else if (handle == ghBtTimeYearInc) {
                guii_tm.tm_year++;
                Guii_SetupTimeFix();
            } else if (handle == ghBtTimeHourDec) {
                guii_tm.tm_hour--;
                Guii_SetupTimeFix();
            } else if (handle == ghBtTimeHourInc) {
                guii_tm.tm_hour++;
                Guii_SetupTimeFix();
            } else if (handle == ghBtTimeMinDec) {
                guii_tm.tm_min--;
                Guii_SetupTimeFix();
            } else if (handle == ghBtTimeMinInc) {
                guii_tm.tm_min++;
                Guii_SetupTimeFix();

            /* Race parameters */
            } else if (handle == ghBtArcadeFilDec) {
                Guii_SetupFilChange(&guii_arcade.filling_pct, false, &ghSliderArcadeFil);
            } else if (handle == ghBtArcadeFilInc) {
                Guii_SetupFilChange(&guii_arcade.filling_pct, true, &ghSliderArcadeFil);
            } else if (handle == ghBtArcadeDeadDec) {
                Guii_SetupDeadChange(&guii_arcade.deadtime_ms, false, &ghSliderArcadeDead);
            } else if (handle == ghBtArcadeDeadInc) {
                Guii_SetupDeadChange(&guii_arcade.deadtime_ms, true, &ghSliderArcadeDead);

            } else if (handle == ghBtAccelFilDec) {
                Guii_SetupFilChange(&guii_acceleration.filling_pct, false, &ghSliderAccelFil);
            } else if (handle == ghBtAccelFilInc) {
                Guii_SetupFilChange(&guii_acceleration.filling_pct, true, &ghSliderAccelFil);
            } else if (handle == ghBtAccelDeadDec) {
                Guii_SetupDeadChange(&guii_acceleration.deadtime_ms, false, &ghSliderAccelDead);
            } else if (handle == ghBtAccelDeadInc) {
                Guii_SetupDeadChange(&guii_acceleration.deadtime_ms, true, &ghSliderAccelDead);

            } else if (handle == ghBtLongFilDec) {
                Guii_SetupFilChange(&guii_long_dist.filling_pct, false, &ghSliderLongFil);
            } else if (handle == ghBtLongFilInc) {
                Guii_SetupFilChange(&guii_long_dist.filling_pct, true, &ghSliderLongFil);
            } else if (handle == ghBtLongDeadDec) {
                Guii_SetupDeadChange(&guii_long_dist.deadtime_ms, false, &ghSliderLongDead);
            } else if (handle == ghBtLongDeadInc) {
                Guii_SetupDeadChange(&guii_long_dist.deadtime_ms, true, &ghSliderLongDead);
            } else {
                return false;
            }
            break;

        case GEVENT_GWIN_SLIDER:
            if (handle == ghSliderArcadeFil) {
                guii_arcade.filling_pct = ((GEventGWinSlider *)ev)->position;
            } else if (handle == ghSliderArcadeDead) {
                guii_arcade.deadtime_ms = ((GEventGWinSlider *)ev)->position;
            } else if (handle == ghSliderAccelFil) {
                guii_acceleration.filling_pct = ((GEventGWinSlider *)ev)->position;
            } else if (handle == ghSliderAccelDead) {
                guii_acceleration.deadtime_ms = ((GEventGWinSlider *)ev)->position;
            } else if (handle == ghSliderLongFil) {
                guii_long_dist.filling_pct = ((GEventGWinSlider *)ev)->position;
            } else if (handle == ghSliderLongDead) {
                guii_long_dist.deadtime_ms = ((GEventGWinSlider *)ev)->position;
            } else if (handle == ghSliderBacklight) {
                gdispSetBacklight(((GEventGWinSlider *)ev)->position);
            } else if (handle == ghSliderContrast) {
                gdispSetContrast(((GEventGWinSlider *)ev)->position);
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
