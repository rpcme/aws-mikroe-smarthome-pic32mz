/*
    OLED_C_click.h

 ------------------------------------------------------------------------------

  This file is part of mikroSDK.
  
  Copyright (c) 2017, MikroElektonika - http://www.mikroe.com

  All rights reserved.

----------------------------------------------------------------------------- */

/**
    \file     OLED_C_click.h
    \brief    OLED_C Click Library
    \defgroup OLEDC
    \brief    OLED_C Click Driver
    \{

- Global Library Prefix : OLEDC
- Version               : 1.1.0
- Date                  : Dec 2018.
- Developer             : Milos Vidojevic

---

- 1.0.0 : Module created (Milos Vidojevic)
- 1.1.0 : RTOS support implemented (Milos Vidojevic)

---

    \note

Driver carries additional graphic functions.

*/
/* -------------------------------------------------------------------------- */

#ifndef _OLEDC_H_
#define _OLEDC_H_

#include <stdint.h>

#define __OLEDC_DRV_SPI__           ///< \macro __OLEDC_DRV_SPI__  \brief SPI driver selector.
// #define __OLEDC_DRV_I2C__        ///< \macro __OLEDC_DRV_I2C__  \brief I2C driver selector.
// #define __OLEDC_DRV_UART__       ///< \macro __OLEDC_DRV_UART__ \brief UART driver selector.

#define T_OLEDC_P const uint8_t*    ///< \macro T_OLEDC_P \brief Driver Abstract type.

#define OLEDC_ERR       1           ///< \macro OLEDC_ERR \brief Return value error.
#define OLEDC_OK        0           ///< \macro OLEDC_OK  \brief Return value OK.

#ifdef __cplusplus
extern "C"{
#endif
                                                                                                                             
#ifdef   __OLEDC_DRV_SPI__
void oledc_spiDriverInit(T_OLEDC_P gpioObj, T_OLEDC_P spiObj);
#endif
#ifdef   __OLEDC_DRV_I2C__
void oledc_i2cDriverInit(T_OLEDC_P gpioObj, T_OLEDC_P i2cObj, uint8_t slave);
#endif
#ifdef   __OLEDC_DRV_UART__
void oledc_uartDriverInit(T_OLEDC_P gpioObj, T_OLEDC_P uartObj);
#endif

/**
 * \brief OLED C Initialization
 *
 * Function initializes function pointers and perform setup of the SSD1351
 * must be called first
 */
void oledc_configure();

/**
 * \brief OLED C Hardware Reset
 *
 * Performs hardware reset of the click board. Init function calling executin
 * of this function.
 */
void oledc_reset();

/**
 * \brief OLED C Enable
 *
 * \param[in] state ( 1 ON / 0 OFF )
 *
 * Enable or disable the display usinf EN pin on the clickboard.
 */
void oledc_enable(uint8_t state);

/**
 * \brief OLED C Command
 *
 * \param[in] command   valid command
 * \param[in] args      pointer to command argument
 * \param[in] args_len  arguments size in bytes
 *
 * Funcion executes provided command.
 */
void oledc_command(uint8_t command, uint8_t *args, uint16_t args_len);

/**
 * \brief OLED C Fill Scren
 *
 * \param[in] color     RGB color
 *
 * Fills whole screen with provided color.
 */
//void oledc_fill_screen(uint16_t color );

/**
 * \brief OLED C Task
 *
 * Funcion sending content of frame buffer to the device.
 *
 * \note 
 * Function should be placed inside infinite loop in case of bare metal apps.
 */
void oledc_task();

/**
 * \brief OLED C Set Pen Color
 *
 * \param[in] rgb color in RGB565 format
 *
 * Funcion will set pen color which will be used with all funcions except draw
 * image funcion.
 */
void oledc_set_pen_color(uint16_t rgb);

/**
 * \brief OLED C Draw Rectangle
 *
 * \param[in] xs   column offset from the left border of the screen
 * \param[in] ys   row offset from the top border of the screen
 * \param[in] xf   column end offset also counted from the left border
 * \param[in] yf   row offset also counted from the top border
 *
 * ```
 * 
 *     (xs,ys)
 *        +------------+
 *        |xxxxxxxxxxxx|
 *        |xxxxxxxxxxxx|
 *        +------------+
 *                  (xf,yf)
 * 
 * ```
 * 
 * Function draws field with backgroud color configured using set pen function.
 */
int oledc_draw_field(uint8_t xs, uint8_t ys, uint8_t xf, uint8_t yf);


/**
 * \brief OLED C Draw Line
 *
 * \param[in] xs   column offset from the left border of the screen
 * \param[in] ys   row offset from the top border of the screen
 * \param[in] xf   column end offset also counted from the left border
 * \param[in] yf   row offset also counted from the top border
 *
 * ```
 * 
 *     (xs,ys)
 *        +
 *         \
 *          \
 *           +
 *        (xf,yf)
 * 
 * ```
 * 
 * Function draws field with backgroud color configured using set pen function.
 */
int oledc_draw_line(uint8_t xs, uint8_t ys, uint8_t xf, uint8_t yf);

/**
 * \brief OLED C Draw Polygon
 *
 * \param[in] ncorners   number of polygon corners
 * \param[in] x array of x coordinates
 * \param[in] y array of y coordinates
 *
 * ```
 * 
 *   (x[0],y[0])      (x[3],y[3])
 *        +----------------+
 *         \               |
 *          \              |
 *           +-------------+
 *      (x[1],y[1])   (x[2],y[2])
 * 
 * ```
 * 
 * Function draws polygon.
 */
int oledc_draw_polygon(uint8_t ncorners, uint8_t* x, uint8_t* y);

/**
 * \brief OLED C Draw BMP Image
 *
 * \param[in] img  pointer to bmp image
 * \param[in] xs   column offset from the left border of the screen
 * \param[in] ys   row offset from the top border of the screen
 *
 * Function draws image on the screen. Provided image must be in .bmp format
 * with 6 bytes header size.
 */
int oledc_image(const uint8_t* img, uint8_t xs, uint8_t ys);

int oledc_draw_bitmap(uint8_t *bitmap, 
            uint8_t xs, uint8_t ys, uint8_t xf, uint8_t yf);

int oledc_draw_bitmap_c(const uint8_t *bitmap, 
            uint8_t xs, uint8_t ys, uint8_t xf, uint8_t yf);

/**
 * \brief OLED C Draw Text
 *
 * \param[in] font font look up table
 * \param[in] text text string
 * \param[in] xs   column offset from the left border of the screen
 * \param[in] ys   row offset from the top border of the screen
 *
 * Function writes text on the screen.
 */
int oledc_text(const uint8_t *font, unsigned char *text, uint8_t xs, uint8_t ys);

#ifdef __cplusplus
} // extern "C"
#endif
#endif

/// \}
/* ----------------------------------------------------------------------------

    OLED_C_click.h

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
