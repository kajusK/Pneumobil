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
 * @file    hmi/ugfx/board_SSD1963.h
 * @brief   SSD1963 screen driver code for uGFX, talk to lcd using fsmc
 *
 * @addtogroup app-hmi
 * @{
 */
#ifndef _GDISP_LLD_BOARD_H
#define _GDISP_LLD_BOARD_H

/* Using FSMC A18 as RS */
#define GDISP_REG              (*((volatile uint16_t *) 0x60000000)) /* RS = 0 */
#define GDISP_RAM              (*((volatile uint16_t *) 0x60080000)) /* RS = 1 */

static const LCD_Parameters DisplayTimings[] = {
    {
        800, 480,                       // Panel width and height
        2, 2, 41,                       // Horizontal Timings (back porch, front porch, pulse)
        CALC_PERIOD(800,2,2,41),        // Total Horizontal Period (calculated from above line)
        2, 2, 10,                       // Vertical Timings (back porch, front porch, pulse)
        CALC_PERIOD(480,2,2,10),        // Total Vertical Period (calculated from above line)
        CALC_FPR(800,480,2,2,41,2,2,10,60ULL),  // FPR - the 60ULL is the frames per second. Note the ULL!
        LCD_PANEL_DATA_WIDTH_24BIT | LCD_PANEL_ENABLE_DITHERING,
        FALSE,                          // Flip horizontally
        FALSE                           // Flip vertically
    },
};

static GFXINLINE void init_board(GDisplay *g)
{
    g->board = 0;

    switch(g->controllerdisplay) {
        case 0:
            #if defined(STM32F1XX) || defined(STM32F3XX)
                    rccEnableAHB(RCC_AHBENR_FSMCEN, 0);
            #elif defined(STM32F4XX) || defined(STM32F2XX)
                    rccEnableAHB3(RCC_AHB3ENR_FSMCEN, 0);
            #else
                    #error "FSMC not implemented for this device"
            #endif

            /* FSMC timing */
            FSMC_Bank1->BTCR[1] = (FSMC_BTR1_ADDSET_1 | FSMC_BTR1_ADDSET_3) \
                            | (FSMC_BTR1_DATAST_1 | FSMC_BTR1_DATAST_3) \
                            | (FSMC_BTR1_BUSTURN_1 | FSMC_BTR1_BUSTURN_3) ;

            FSMC_Bank1->BTCR[0] = FSMC_BCR1_MWID_0 | FSMC_BCR1_WREN | FSMC_BCR1_MBKEN;
            break;
    }
}

static GFXINLINE void post_init_board(GDisplay *g)
{
    (void) g;
    /* FSMC delay reduced as the controller now runs at full speed */
    FSMC_Bank1->BTCR[0+1] = FSMC_BTR1_ADDSET_0 | FSMC_BTR1_DATAST_2 | FSMC_BTR1_BUSTURN_0 ;
    FSMC_Bank1->BTCR[0] = FSMC_BCR1_MWID_0 | FSMC_BCR1_WREN | FSMC_BCR1_MBKEN;
}

static GFXINLINE void setpin_reset(GDisplay *g, bool_t state)
{
    (void) g;
    palWriteLine(LINE_LCD_REST, !state);
}

static GFXINLINE void acquire_bus(GDisplay *g)
{
    (void) g;
}

static GFXINLINE void release_bus(GDisplay *g)
{
    (void) g;
}

static GFXINLINE void write_index(GDisplay *g, uint16_t index)
{
    (void) g;
    GDISP_REG = index;
}

static GFXINLINE void write_data(GDisplay *g, uint16_t data)
{
    (void) g;
    GDISP_RAM = data;
}

static GFXINLINE void setreadmode(GDisplay *g)
{
    (void) g;
}

static GFXINLINE void setwritemode(GDisplay *g)
{
    (void) g;
}

static GFXINLINE uint16_t read_data(GDisplay *g)
{
    (void) g;
    return GDISP_RAM;
}

#endif
