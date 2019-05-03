/*
    OLED_C_click.c

 ------------------------------------------------------------------------------

  This file is part of mikroSDK.

  Copyright (c) 2017, MikroElektonika - http://www.mikroe.com

  All rights reserved.

 ------------------------------------------------------------------------------

- Global Library Prefix : OLEDC
- Version               : 1.1.0
- Date                  : Dec 2018.
- Developer             : Milos Vidojevic
    
---

- 1.0.0 : Module created (Milos Vidojevic)
- 1.1.0 : RTOS support implemented (Milos Vidojevic)

----------------------------------------------------------------------------- */

#include "click_oled_c.h"
#include "click_oled_c_hal.h"

/* ------------------------------------------------------------------- MACROS */

/*  
    Use this macro to enable partial screen update which slightly improves 
    display performances.

    !! NOT SUPPORTED IN CURRENT VERSION
*/
//#define _OLEDC_PARTIAL_SCREEN_UPDATE

//  OLED REMAMP SET

#define _OLEDC_RMP_INC_HOR              0x00
#define _OLEDC_RMP_INC_VER              0x01
#define _OLEDC_RMP_COLOR_NOR            0x00
#define _OLEDC_RMP_COLOR_REV            0x02
#define _OLEDC_RMP_SEQ_RGB              0x00
#define _OLEDC_RMP_SEQ_BGR              0x04
#define _OLEDC_RMP_SCAN_NOR             0x00
#define _OLEDC_RMP_SCAN_REV             0x10
#define _OLEDC_RMP_SPLIT_DISABLE        0x00
#define _OLEDC_RMP_SPLIT_ENABLE         0x20
#define _OLEDC_COLOR_65K                0x00
#define _OLEDC_COLOR_262K               0x80
#define _OLEDC_IMG_HEAD                 0x06

//  Device Properties

#define _OLEDC_SCRN_SIZE                (96 * 96)
#define _OLEDC_SCRN_X_MAX               95
#define _OLEDC_SCRN_Y_MAX               95
#define _OLEDC_SCRN_X_OFFSET            0x10
#define _OLEDC_SCRN_Y_OFFSET            0x00

//  SSD1355 Commands

#define _OLEDC_SET_COL_ADDRESS          0x15
#define _OLEDC_SET_ROW_ADDRESS          0x75
#define _OLEDC_WRITE_RAM                0x5C
#define _OLEDC_READ_RAM                 0x5D
#define _OLEDC_SET_REMAP                0xA0
#define _OLEDC_SET_START_LINE           0xA1
#define _OLEDC_SET_OFFSET               0xA2
#define _OLEDC_MODE_OFF                 0xA4
#define _OLEDC_MODE_ON                  0xA5
#define _OLEDC_MODE_NORMAL              0xA6
#define _OLEDC_MODE_INVERSE             0xA7
#define _OLEDC_FUNCTION                 0xAB
#define _OLEDC_SLEEP_ON                 0xAE
#define _OLEDC_SLEEP_OFF                0xAF
#define _OLEDC_NOP                      0xB0
#define _OLEDC_SET_RESET_PRECH          0xB1
#define _OLEDC_ENHANCEMENT              0xB2
#define _OLEDC_CLOCK_DIV                0xB3
#define _OLEDC_VSL                      0xB4
#define _OLEDC_GPIO                     0xB5
#define _OLEDC_SETSEC_PRECH             0xB6
#define _OLEDC_GREY_SCALE               0xB8
#define _OLEDC_LUT                      0xB9
#define _OLEDC_PRECH_VOL                0xBB
#define _OLEDC_VCOMH                    0xBE
#define _OLEDC_CONTRAST                 0xC1
#define _OLEDC_MASTER_CONTRAST          0xC7
#define _OLEDC_MUX_RATIO                0xCA
#define _OLEDC_COMMAND_LOCK             0xFD
#define _OLEDC_SCROLL_HOR               0x96
#define _OLEDC_START_MOV                0x9E
#define _OLEDC_STOP_MOV                 0x9F

/* ---------------------------------------------------------------- VARIABLES */

static uint8_t color_p[2];
static uint8_t bound_x[2];
static uint8_t bound_y[2];

static uint8_t frame_update;
static uint8_t frame_buffer[_OLEDC_SCRN_SIZE * 2];

/* --------------------------------------------- PRIVATE FUNCTION DEFINITIONS */

static void pixel(uint8_t x, uint8_t y);

static int update_x_bound(uint8_t x);

static int update_y_bound(uint8_t y);

static uint8_t get_font_first_char(const uint8_t* font);

static uint8_t get_font_last_char(const uint8_t* font);

static uint8_t get_font_height(const uint8_t* font);

static uint8_t get_font_bitmap(const uint8_t* font, uint8_t ch, uint8_t* map);

/* --------------------------------------------------------- PUBLIC FUNCTIONS */

#ifdef   __OLEDC_DRV_SPI__

void oledc_spiDriverInit(T_OLEDC_P gpioObj, T_OLEDC_P spiObj)
{    
    hal_spiMap((T_HAL_P)DRV_SPI_Open(
            DRV_SPI_INDEX_0,
            DRV_IO_INTENT_READWRITE | DRV_IO_INTENT_BLOCKING)
    );

    hal_gpio_csSet(1);
    hal_gpio_anSet(0);
    hal_gpio_pwmSet(1);
}

#endif
#ifdef   __OLEDC_DRV_I2C__

void oledc_i2cDriverInit(T_OLEDC_P gpioObj, T_OLEDC_P i2cObj, uint8_t slave)
{
    _slaveAddress = slave;
    hal_i2cMap( (T_HAL_P)i2cObj );
    hal_gpioMap( (T_HAL_P)gpioObj );

    // ... power ON
    // ... configure CHIP
}

#endif
#ifdef   __OLEDC_DRV_UART__

void oledc_uartDriverInit(T_OLEDC_P gpioObj, T_OLEDC_P uartObj)
{
    hal_uartMap( (T_HAL_P)uartObj );
    hal_gpioMap( (T_HAL_P)gpioObj );

    // ... power ON
    // ... configure CHIP
}

#endif

/* ----------------------------------------------------------- IMPLEMENTATION */

void oledc_enable( uint8_t state )
{
    hal_gpio_intSet(state);
}

void oledc_reset( void )
{
    hal_gpio_rstSet(1);
    vTaskDelay(1);
    hal_gpio_rstSet(0);
    vTaskDelay(1);
    hal_gpio_rstSet(1);
    vTaskDelay(40);
}

void oledc_command(uint8_t command, uint8_t *p_args, uint16_t n_args)
{
    hal_gpio_csSet(0);
    hal_gpio_pwmSet(0);
    hal_spiWrite(&command, 1);
    hal_gpio_pwmSet(1);
    hal_spiWrite(p_args, n_args);
    hal_gpio_csSet(1);
    hal_gpio_pwmSet(0);
}

void oledc_configure()
{
    //  Screen configuration.

    uint8_t lock                = 0x12;
    uint8_t cmd_lock            = 0xB1;
    uint8_t mux_ratio           = 95;
    uint8_t start_line          = 0x80;
    uint8_t offset              = 0x20;
    uint8_t vcom                = 0x05;
    uint8_t clock_divset        = 0xF1;
    uint8_t precharge           = 0x32;
    uint8_t precharge_2         = 0x01;
    uint8_t master_contrast     = 0xCF;
    uint8_t contrast[3] = { 0x8A, 0x51, 0x8A };
    uint8_t vsl[3]      = { 0xA0, 0xB5, 0x55 };
    uint8_t remap       = _OLEDC_RMP_INC_HOR | _OLEDC_RMP_COLOR_REV |
                          _OLEDC_RMP_SEQ_RGB | _OLEDC_RMP_SCAN_REV |
                          _OLEDC_RMP_SPLIT_ENABLE | _OLEDC_COLOR_65K;

    //  Unlock display and turn off. Necessary according to datasheet.

    oledc_command(_OLEDC_COMMAND_LOCK,    &lock,            1);
    oledc_command(_OLEDC_COMMAND_LOCK,    &cmd_lock,        1);
    oledc_command(_OLEDC_SLEEP_ON,        0,                0);

    //  Default Setup SSD1351 for 96 x 96 display size.

    oledc_command(_OLEDC_SET_REMAP,       &remap,           1);
    oledc_command(_OLEDC_MUX_RATIO,       &mux_ratio,       1);
    oledc_command(_OLEDC_SET_START_LINE,  &start_line,      1);
    oledc_command(_OLEDC_SET_OFFSET,      &offset,          1);
    oledc_command(_OLEDC_VCOMH,           &vcom,            1);
    oledc_command(_OLEDC_CLOCK_DIV,       &clock_divset,    1);
    oledc_command(_OLEDC_SET_RESET_PRECH, &precharge,       1);
    oledc_command(_OLEDC_SETSEC_PRECH,    &precharge_2,     1);
    oledc_command(_OLEDC_MASTER_CONTRAST, &master_contrast, 1);
    oledc_command(_OLEDC_CONTRAST,        contrast,         3);
    oledc_command(_OLEDC_VSL,             vsl,              3);

    //  Set normal mode and turn on display.

    oledc_command(_OLEDC_MODE_NORMAL,     0,                0);
    oledc_command(_OLEDC_SLEEP_OFF,       0,                0);

    //  Draw black screen as initial state.

    memset(frame_buffer, 0, _OLEDC_SCRN_SIZE * 2);

#ifdef _OLEDC_PARTIAL_SCREEN_UPDATE

    update_x_bound(0);
    update_x_bound(_OLEDC_SCRN_X_MAX);
    update_y_bound(0);
    update_y_bound(_OLEDC_SCRN_Y_MAX);

#endif

    //  Adjust RAM and local variables to fit display resolution.

    bound_x[0] += _OLEDC_SCRN_X_OFFSET; 
    bound_x[1] += _OLEDC_SCRN_X_MAX + _OLEDC_SCRN_X_OFFSET;
    bound_y[0] += _OLEDC_SCRN_Y_OFFSET;
    bound_y[1] += _OLEDC_SCRN_Y_MAX + _OLEDC_SCRN_Y_OFFSET;

    oledc_command(_OLEDC_SET_COL_ADDRESS, bound_x, 2);
    oledc_command(_OLEDC_SET_ROW_ADDRESS, bound_y, 2);

    frame_update = 1;
}

void oledc_set_pen_color(uint16_t rgb)
{
    color_p[0] = rgb >> 8;
    color_p[1] = (uint8_t)(rgb & 0x00FF);
}

int oledc_draw_field(uint8_t xs, uint8_t ys, uint8_t xf, uint8_t yf)
{
    uint8_t x;
    uint8_t y;

    if ((xf < xs) || (yf < ys))
    {
        return OLEDC_ERR;
    }
    
    if (update_x_bound(xs) || update_x_bound(xf))
    {
        return OLEDC_ERR;
    }

    if (update_y_bound(ys) || update_y_bound(yf))
    {
        return OLEDC_ERR;
    }

    for (y = ys; y <= yf; y++)
    {
        for (x = xs; x <= xf; x++)
        {
            pixel(x, y);
        }
    }

    //  Schedule screen update from task.

    frame_update = 1;

    return OLEDC_OK;
}

int oledc_draw_line(uint8_t xs, uint8_t ys, uint8_t xf, uint8_t yf)
{
    uint8_t x;
    uint8_t y;
    uint8_t dx;
    uint8_t dy;
    uint8_t swap;
    int8_t  step;
    int8_t  err;
    
    if (update_x_bound(xs) || update_x_bound(xf))
    {
        return OLEDC_ERR;
    }

    if (update_y_bound(ys) || update_y_bound(yf))
    {
        return OLEDC_ERR;
    }

    if (xs > xf) { dx = xs - xf; }
    else         { dx = xf - xs; }

    if (ys > yf) { dy = ys - yf; }
    else         { dy = yf - ys; }

    if (dy > dx)
    {
        uint8_t tmp;

        swap = 1;
        tmp = dx; dx = dy; dy = tmp;
        tmp = xs; xs = ys; ys = tmp;
        tmp = xf; xf = yf; yf = tmp;
    }
    else
    {
        swap = 0;
    }

    if (xs > xf) 
    {
        uint8_t tmp;

        tmp = xs; xs = xf; xf = tmp;
        tmp = ys; ys = yf; yf = tmp;
    }

    err = dx >> 1;

    if (yf > ys) { step = 1;  }
    else         { step = -1; }

    y = ys;
    
    for (x = xs; x <= xf; x++)
    {
        if (swap == 0) { pixel(x, y); }
        else           { pixel(y, x); }
      
        err -= dy;

        if (err < 0) 
        {
            y += step;
            err += dx;
        }
    }

    //  Schedule screen update from task.

    frame_update = 1;

    return OLEDC_OK;
}

int oledc_draw_polygon(uint8_t ncorners, uint8_t* x, uint8_t* y)
{
    uint8_t i;

    for (i = 0; i < ncorners - 1; i++)
    {
        if (oledc_draw_line(x[i], y[i], x[i + 1], y[i + 1]) != OLEDC_OK)
        {
            return OLEDC_ERR;
        }
    }

    return oledc_draw_line(x[0], y[0], x[ncorners - 1], y[ncorners - 1]);
}


int oledc_image(const uint8_t* img, uint8_t xs, uint8_t ys)
{   
    const uint8_t * p_img;
    uint8_t         x;
    uint8_t         y;
    uint8_t         bckp_color_p[2];
    
    //  p_img points to first pixel - skip 6 header bytes.

    p_img = &img[6];

    /*  
        Check bounds using image header 

        img[2] represents width
        img[4] represents height
    */

    if (update_x_bound(xs) || update_x_bound(xs + img[2] - 1))
    {
        return OLEDC_ERR;
    }

    if (update_y_bound(ys) || update_y_bound(ys + img[4] - 1))
    {
        return OLEDC_ERR;
    }

    //  Backup current pen color.

    bckp_color_p[0] = color_p[0];
    bckp_color_p[1] = color_p[1];

    //  Copy image to frame buffer.

    for (x = 0; x < img[2]; ++x)
    {
        for (y = 0; y < img[4]; ++y)
        {
            //  Update pen color

            color_p[0] = p_img[(((y * img[2]) + x) * 2) + 1];
            color_p[1] = p_img[((y * img[2]) + x) * 2];

            // Draw pixel inside frame buffer

            pixel(x + xs, y + ys);
        }
    }

    //  Restore pen color.

    color_p[0] = bckp_color_p[0];
    color_p[1] = bckp_color_p[1];

    //  Schedule screen update from task.

    frame_update = 1;

    return OLEDC_OK;
}

int oledc_draw_bitmap_c(const uint8_t *bitmap, 
            uint8_t xs, uint8_t ys, uint8_t xf, uint8_t yf)
{
    uint16_t x;
    uint16_t y;
    uint16_t c;
    uint8_t  mask;
    
    if (update_x_bound(xs) || update_x_bound(xf))
    {
        return OLEDC_ERR;
    }

    if (update_y_bound(ys) || update_y_bound(yf))
    {
        return OLEDC_ERR;
    }

    for (c = 0, y = ys; y <= yf; y++, c++)
    {
        mask = 1;

        for (x = xs; x <= xf; x++)
        {
            /*
                Mask overflow triger counter incrementation - meaning display 
                same horizontal line but use next bitmap byte - allow to map 
                image wider than 8 pixels.
            */

            if (!mask)
            {
                c++;
                mask = 1;
            }

            //  Draw pixel only if mask match bit from bitmap.

            if (mask & bitmap[c])
            {
                pixel(x, y);
            }

            mask <<= 1;
        }
    }
    
    //  Schedule screen update from task.

    frame_update = 1;

    return OLEDC_OK;
}


int oledc_draw_bitmap(uint8_t *bitmap, 
            uint8_t xs, uint8_t ys, uint8_t xf, uint8_t yf)
{
    uint16_t x;
    uint16_t y;
    uint16_t c;
    uint8_t  mask;
    
    if (update_x_bound(xs) || update_x_bound(xf))
    {
        return OLEDC_ERR;
    }

    if (update_y_bound(ys) || update_y_bound(yf))
    {
        return OLEDC_ERR;
    }

    for (c = 0, y = ys; y <= yf; y++, c++)
    {
        mask = 1;

        for (x = xs; x <= xf; x++)
        {
            /*
                Mask overflow triger counter incrementation - meaning display 
                same horizontal line but use next bitmap byte - allow to map 
                image wider than 8 pixels.
            */

            if (!mask)
            {
                c++;
                mask = 1;
            }

            //  Draw pixel only if mask match bit from bitmap.

            if (mask & bitmap[c])
            {
                pixel(x, y);
            }

            mask <<= 1;
        }
    }
    
    //  Schedule screen update from task.

    frame_update = 1;

    return OLEDC_OK;
}

int oledc_text(const uint8_t *font, unsigned char *text, uint8_t xs, uint8_t ys)
{
    uint16_t c;
    uint8_t xf;
    uint8_t yf;
    uint8_t first_ch;
    uint8_t last_ch;
    uint8_t height_ch;
    uint8_t width_ch;
    
    /**
        !!! WARNING : Possible buffer overflow on map variable. 
        !! TODO : Increase map buffer in case of large fonts. 
    */

    uint8_t map[128];

    first_ch = get_font_first_char(font);
    last_ch = get_font_last_char(font);

    //  Bottom row calculation

    yf = ys + get_font_height(font) - 1;

    //  Print only character defined by font look up table.

    for (c = 0; ((text[c] >= first_ch) && (text[c] <= last_ch)); c++)
    {
        width_ch = get_font_bitmap(font, text[c], map);
        
        //  Last column calculation

        xf = xs + width_ch - 1;

        if (oledc_draw_bitmap(map, xs, ys, xf, yf))
        {
            return OLEDC_ERR;
        }

        //  Update start point calculation for next character.

        xs = xf + 1;
    }

    return OLEDC_OK;
}

void oledc_task()
{
    uint16_t i;
    uint8_t cmd = _OLEDC_WRITE_RAM;

#ifdef _OLEDC_PARTIAL_SCREEN_UPDATE

    //  Adjust bounds according to offset.

    bound_x[0] += _OLEDC_SCRN_X_OFFSET; 
    bound_x[1] += _OLEDC_SCRN_X_OFFSET;
    bound_y[0] += _OLEDC_SCRN_Y_OFFSET;
    bound_y[1] += _OLEDC_SCRN_Y_OFFSET;

    oledc_command(_OLEDC_SET_COL_ADDRESS, bound_x, 2);
    oledc_command(_OLEDC_SET_ROW_ADDRESS, bound_y, 2);

#endif

    if (frame_update != 0)
    {
        hal_gpio_csSet(0);
        hal_gpio_pwmSet(0);
        hal_spiWrite(&cmd, 1);
        hal_gpio_pwmSet(1);
        
        
        /*
            ! IMPROVEMENT :

            For maximal performances populate whole SPI buffer on each loop. 
            Number of loops should be adjusted to be compatible with SPI buffer 
            size - (_OLEDC_SCRN_SIZE / SPI_BUF_SIZE).
        */

        for (i = 0; i < (_OLEDC_SCRN_SIZE * 2); i += 2)
        {
            hal_spiWrite(&frame_buffer[i], 2);
        }
        
        hal_gpio_csSet(1);
        hal_gpio_pwmSet(0);

        //  Reset bound variables.

        bound_x[0] = _OLEDC_SCRN_X_MAX;
        bound_y[0] = _OLEDC_SCRN_Y_MAX;
        bound_x[1] = 0;
        bound_y[1] = 0;

        frame_update = 0;
    }
}

/* ------------------------------------------ PRIVATE FUNCTION IMPLEMENTATION */

/*
    ! IMPROVEMENT :

    Replace pixel function with macro. 
*/
static void pixel(uint8_t x, uint8_t y)
{    
    frame_buffer[((y * 96 + x) * 2)] = color_p[0];
    frame_buffer[((y * 96 + x) * 2) + 1] = color_p[1];
}

static int update_x_bound(uint8_t x)
{
    if (x > _OLEDC_SCRN_X_MAX)
    {
        return 1;
    }
    else
    {
#if _OLEDC_PARTIAL_SCREEN_UPDATE

        if (x < bound_x[0]) { bound_x[0] = x; }
        if (x > bound_x[1]) { bound_x[1] = x; }

#endif
    }

    return 0;
}

static int update_y_bound(uint8_t y)
{
    if (y > _OLEDC_SCRN_Y_MAX)
    {
        return 1;
    }
    else
    {
#if _OLEDC_PARTIAL_SCREEN_UPDATE

        if (y < bound_y[0]) { bound_y[0] = y; }
        if (y > bound_y[1]) { bound_y[1] = y; }

#endif
    }
    
    return 0;    
}

static uint8_t get_font_first_char(const uint8_t* font)
{
    return font[2];
}

static uint8_t get_font_last_char(const uint8_t* font)
{
    return font[4];
}

static uint8_t get_font_height(const uint8_t* font)
{
    return font[6];
}

static uint8_t get_font_bitmap(const uint8_t* font, uint8_t ch, uint8_t* map)
{
    uint8_t  cnt;
    uint8_t  tmp_w;     //  Character Map Width.
    uint32_t tmp_o;     //  Character Map Offset.

    ch -= font[2];  

    tmp_o = ((font[8 + (ch * 4) + 3] << 16) | 
                    (font[8 + (ch * 4) + 2] << 8) | font[8 + (ch * 4) + 1]);
    tmp_w = ((font[8 + (ch * 4)] / 8) + 1) * font[6];

    for (cnt = 0; cnt < tmp_w; cnt++)
    {
        map[cnt] = font[tmp_o + cnt];
    }

    return font[8 + (ch * 4)];
}

/* ----------------------------------------------------------------------------

    OLED_C_click.c

  Copyright (c) 2017, MikroElektonika - http://www.mikroe.com

  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

3. All advertising materials mentioning features or use of this software
   must display the following acknowledgement:
   This product includes software developed by the MikroElektonika.

4. Neither the name of the MikroElektonika nor the
   names of its contributors may be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY MIKROELEKTRONIKA ''AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL MIKROELEKTRONIKA BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

----------------------------------------------------------------------------- */
