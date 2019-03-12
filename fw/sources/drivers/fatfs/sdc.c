/*
    BUT pneumobil - Copyright (C) 2018 Jakub Kaderka.

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
 * @file    sdc.c
 * @brief   SD card driver for ChibiOS hal
 *
 * @addtogroup drivers
 * @{
 */

#include <hal.h>
#include "ff.h"
#include "modules/log.h"

#include "drivers/fatfs/sdc.h"

#define POOLING_DEBOUNCE_CNT 10
#define POOLING_INTERVAL_MS    10

/** Stack and stuff for thread */
THD_WORKING_AREA(sdcdi_thread_area, 1024);

/** True if card is mounted and ready to be used */
static bool sdcdi_ready;

/** Card monitor timer */
static virtual_timer_t sdcdi_timer;

/** Debounce counter */
static unsigned sdcdi_debounce_cnt;

/** Card event sources */
static event_source_t sdcdi_inserted_event;
static event_source_t sdcdi_removed_event;

/** External callback that will be called on card insertion */
static sdcd_cb_t sdcdi_insert_cb = NULL;

static FATFS SDC_FS;

/**
 * Callback for sd card insertion timer
 *
 * @param [in] p    Pointer to the @p BaseBlockDevice object
 */
static void SDCdi_timer_cb(void *p)
{
    BaseBlockDevice *bbdp = p;

    chSysLockFromISR();
    if (sdcdi_debounce_cnt > 0) {
        if (blkIsInserted(bbdp)) {
            if (--sdcdi_debounce_cnt == 0) {
                chEvtBroadcastI(&sdcdi_inserted_event);
            }
        }
        else {
            sdcdi_debounce_cnt = POOLING_DEBOUNCE_CNT;
        }
    }
    else {
        if (!blkIsInserted(bbdp)) {
            sdcdi_debounce_cnt = POOLING_DEBOUNCE_CNT;
            chEvtBroadcastI(&sdcdi_removed_event);
        }
    }
    chVTSetI(&sdcdi_timer, TIME_MS2I(POOLING_INTERVAL_MS), SDCdi_timer_cb, bbdp);
    chSysUnlockFromISR();
}

/*
 * Card insertion event.
 *
 * @param [in] id       Unused
 */
static void SDCdi_InsertHandler(eventid_t id)
{
    FRESULT err;

    (void)id;

    Log_Info(LOG_SOURCE_DRIVER, "SD card inserted");
    if (sdcConnect(&SDCD1)) {
        Log_Error(LOG_SOURCE_DRIVER, "Unable to connect to SD card");
        return;
    }

    err = f_mount(&SDC_FS, "/", 1);
    if (err != FR_OK) {
        Log_Error(LOG_SOURCE_DRIVER, "Unable to mount SD card");
        sdcDisconnect(&SDCD1);
        return;
    }
    sdcdi_ready = TRUE;
    if (sdcdi_insert_cb != NULL) {
        sdcdi_insert_cb();
    }
}

/*
 * Card removal event.
 *
 * @param [in] id       Unused
 */
static void SDCdi_RemoveHandler(eventid_t id)
{
    (void)id;

    Log_Info(LOG_SOURCE_DRIVER, "SD card removed");
    sdcDisconnect(&SDCD1);
    sdcdi_ready = FALSE;
}

static THD_FUNCTION(SDCd_Thread, arg)
{
    (void) arg;
    static const evhandler_t evhndl[] = {
        SDCdi_InsertHandler,
        SDCdi_RemoveHandler,
    };
    event_listener_t el0, el1;

    chEvtRegister(&sdcdi_inserted_event, &el0, 0);
    chEvtRegister(&sdcdi_removed_event, &el1, 1);

    while (1) {
        chEvtDispatch(evhndl, chEvtWaitOneTimeout(ALL_EVENTS, TIME_MS2I(500)));
    }
}

bool SDCd_AddInsertCallback(sdcd_cb_t cb)
{
    if (sdcdi_insert_cb != NULL) {
        return false;
    }
    sdcdi_insert_cb = cb;
    return true;
}

bool SDCd_IsReady(void)
{
    return sdcdi_ready;
}

void SDCd_Init(void)
{

    sdcStart(&SDCD1, NULL);
    /*
     * Activates the card insertion monitor.
     */

    chEvtObjectInit(&sdcdi_inserted_event);
    chEvtObjectInit(&sdcdi_removed_event);
    chSysLock();
    sdcdi_debounce_cnt = POOLING_DEBOUNCE_CNT;
    chVTSetI(&sdcdi_timer, TIME_MS2I(POOLING_INTERVAL_MS), SDCdi_timer_cb, &SDCD1);
    chSysUnlock();

    (void) chThdCreateStatic(sdcdi_thread_area, sizeof(sdcdi_thread_area),
                    NORMALPRIO - 2, SDCd_Thread, NULL);
}
/** @} */

