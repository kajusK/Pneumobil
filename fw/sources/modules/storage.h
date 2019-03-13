/*
    BUT pneumobil - Copyright (C) 2018 Jakub Kaderka.

    This file is part of BUT pneumobil ECU.

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
 * @file    storage.h
 * @brief   Storing config data to eeprom
 *
 * @addtogroup modules
 * @{
 */

#ifndef __MODULES_STORAGE_H
#define __MODULES_STORAGE_H

#include <types.h>

/** Store config items after defined delay since last change */
#define STORAGE_SAVE_DELAY_S    5

typedef enum {
    STORAGE_UPDATE_UINT = 0x01,
    STORAGE_UPDATE_BOOL = 0x02,
    STORAGE_UPDATE_FLOAT = 0x04
} storage_update_t;

/**
 * Reset all storage items to default values
 */
extern void Storage_Reset(void);

/**
 * Load all items from storage
 *
 * @return False in case eeprom doesn't contain valid data or data are corrupted
 */
extern bool Storage_LoadAll(void);

/**
 * Update given item in storage
 *
 * Data will be stored defined save delay after last update command
 *
 * @param [in] update       Which item should be updated in storage
 */
extern void Storage_Update(storage_update_t update);

/**
 * Initialize storage module
 */
extern void Storage_Init(void);

#endif

/** @} */
