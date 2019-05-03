/*
    display.c

    | Global Library Prefix | **DISPLAY**           |
    |:---------------------:|:---------------------:|
    | Version               | **1.0.0**             |
    | Date                  | **Feb 2019.**         |
    | Developer             | **Milos Vidojevic**   |
    
    ---
    
    **Version Info :**
    - **1.0.0** Module Created [Milos Vidojevic]

-----------------------------------------------------------------------------

  Copyright (c) 2017, MikroElektonika - http://www.mikroe.com

  All rights reserved.

----------------------------------------------------------------------------- */

#include <stdio.h>

#include "../remote_hvac/module_display.h"
#include "../../../mikroe/OLED_C/click_oled_c.h"

/* ------------------------------------------------------------------- MACROS */
//                                                                     ------

//  Value used during initialization 

#define INIT_VAL    (555.555f)

/* ---------------------------------------------------------------- VARIABLES */
//                                                                  ---------

/**
    \biref Logger Data

This structure holds the Logger's data.

    \note
This structure should be initialized by the LOGGER_Initialize function.
    
*/
DISPLAY_DATA displayData;

/* ------------------------------------------- PRIVATE FUNCTIONS DECLARATIONS */
//                                             ------------------------------

static void _DISPLAY_Tasks ( void );

static void display_update ( void );

static void display_intro ( void );

static void display_update_conn ( int conn );

static void display_update_fan ( void );

static void display_update_wave ( void );

static void display_update_sensor_values ( float temp, float hum );

static void display_update_target_temperature ( float temp );

static void display_update_target_humidity ( float hum );

/* --------------------------------------------------------- PUBLIC FUNCTIONS */
//                                                           ----------------

/**
    \brief Display Module Initalization
*/
void DISPLAY_Initialize ( void )
{
    /* 
        Place the Logger state machine and other fields in its initial state. 
    */

    displayData.state               = MODULE_STATE_INIT;

    oledc_spiDriverInit( NULL, NULL );
    xTaskCreate( ( TaskFunction_t ) _DISPLAY_Tasks, "Display Task",
            DISPLAY_TASK_STACK_SIZE, NULL, DISPLAY_TASK_PRIORITY, NULL );

}

/**
    \brief Display Module Task
*/
void DISPLAY_Tasks ( void )
{
    //  Check the logger current state.

    switch ( displayData.state )
    {
        //  Logger initial state.

        case MODULE_STATE_INIT:
        {   
            oledc_enable( 1 );
            oledc_reset( );
            oledc_configure( );

            //  Intro illustration

            display_intro();

            //  Initial display state

            display_update_conn( 0 );
            display_update_fan( );
            display_update_wave( );
            display_update_sensor_values( INIT_VAL, INIT_VAL );
            display_update_target_temperature( INIT_VAL );
            display_update_target_humidity( INIT_VAL );

            displayData.state                       = MODULE_STATE_PREACTIVE;

            break;
        }

        case MODULE_STATE_INACTIVE:
        {

            break;
        }

        case MODULE_STATE_PREACTIVE:
        {   

            
            displayData.state                       = MODULE_STATE_ACTIVE;

            break;
        }

        case MODULE_STATE_ACTIVE:
        {
            FAN_STATE       fan;
            HVAC_STATE      hvac;
            AIRCON_STATE    aircon;
            SENSOR_VALUE    sensor;
            int             conn;
            float           target_t;

            if ( xQueueReceive( qDISPLAY_Fan, (FAN_STATE *) &fan, 
                        RTOS_NO_BLOCKING ) )
            {
                display_update_wave( );
            }

            if ( xQueueReceive( qDISPLAY_Aircon, (AIRCON_STATE *) &aircon, 
                        RTOS_NO_BLOCKING ) )
            {
                display_update_wave( );
            }

            if ( xQueueReceive( qDISPLAY_Sensor, (SENSOR_VALUE *) &sensor, 
                        RTOS_NO_BLOCKING ) )
            {
                display_update_sensor_values( sensor.temperature, 
                        sensor.humidity );
            }

            if ( xQueueReceive( qDISPLAY_TargetT, (float *) &target_t, 
                        RTOS_NO_BLOCKING ) )
            {
                display_update_target_temperature( target_t );
            }

            if ( xQueueReceive( qDISPLAY_Conn, (int *) &conn, 
                        RTOS_NO_BLOCKING ) )
            {
                display_update_conn( conn );
            }

            display_update_fan( );
            display_update( );

            break;
        }
    
        case MODULE_STATE_POSTACTIVE:
        {
            //  Turn off display.

            displayData.state                       = MODULE_STATE_INACTIVE;

            break;
        }

        default:
        {
            /* 
                ! TODO
                
                Handle error in application's state machine. 
            */
            break;
        }
    }
}

void DISPLAY_ISR_Handler ( void )
{
    switch (displayData.state)
    {
        case MODULE_STATE_INACTIVE:
        {
            displayData.state = MODULE_STATE_PREACTIVE;

            break;
        }
        case MODULE_STATE_ACTIVE:
        {
            displayData.state = MODULE_STATE_POSTACTIVE;

            break;
        }
        case MODULE_STATE_INIT:
        case MODULE_STATE_PREACTIVE:
        case MODULE_STATE_POSTACTIVE:
        {
            //  Nothing to do here.

            break;
        }
        default :

        break;
    }
}

/* -------------------------------------------------------- PRIVATE FUNCTIONS */
//                                                          -----------------

static void _DISPLAY_Tasks ( void )
{
    for (;;)
    {
        DISPLAY_Tasks();
        vTaskDelay(DISPLAY_TASK_DELAY / portTICK_PERIOD_MS);
    }
}

static void display_update ( void )
{
    // Take semaphore and draw frame.

    if ( xSemaphoreTake( smphrSPI2, RTOS_NO_BLOCKING ) )
    {
        oledc_task( );
        xSemaphoreGive( smphrSPI2 );
    }    
}

static void display_intro ( void )
{
    // AWS logo preview

    oledc_image( AWS_LOGO_BMP, 0, 0 );
    display_update( );
    vTaskDelay( 1500 / portTICK_PERIOD_MS );

    // MikroE logo preview

    oledc_image( MIKROE_LOGO_BMP, 0, 0 );
    display_update( );
    vTaskDelay( 1500 / portTICK_PERIOD_MS );

    oledc_set_pen_color( UI_BACKGROUND_COLOR );
    oledc_draw_field( 0, 0, 95, 95 );

    //  Draw static icons (Temp and Hum)

    oledc_set_pen_color( UI_ACTIVE_COLOR );
    oledc_draw_bitmap_c(
                            UI_TEM_ICON,
                            UI_TEM_ICON_XOFF,
                            UI_TEM_ICON_YOFF,
                            UI_TEM_ICON_XOFF + UI_TEM_ICON_W - 1,
                            UI_TEM_ICON_YOFF + UI_TEM_ICON_H - 1
                        );
    oledc_draw_bitmap_c(
                            UI_HUM_ICON,
                            UI_HUM_ICON_XOFF, 
                            UI_HUM_ICON_YOFF, 
                            UI_HUM_ICON_XOFF + UI_HUM_ICON_W - 1,
                            UI_HUM_ICON_YOFF + UI_HUM_ICON_H - 1
                        );   
}

static void display_update_conn ( int conn )
{
    //  Erase current content

    oledc_set_pen_color( UI_BACKGROUND_COLOR );
    oledc_draw_field(
                        UI_CON_ICON_XOFF, 
                        UI_CON_ICON_YOFF, 
                        UI_CON_ICON_XOFF + UI_CON_ICON_BMP_W - 1, 
                        UI_CON_ICON_YOFF + UI_CON_ICON_BMP_H - 1
                    );

    switch (conn)
    {
        //  DISCONNECTED

        case 0 :        
        {
            //  Draw inactive sphere

            oledc_set_pen_color( UI_INACTIVE_COLOR );
            oledc_draw_bitmap_c(
                                    UI_CON_ICON,
                                    UI_CON_ICON_XOFF,
                                    UI_CON_ICON_YOFF,
                                    UI_CON_ICON_XOFF + UI_CON_ICON_W - 1,
                                    UI_CON_ICON_YOFF + UI_CON_ICON_H - 1
                                );

            break;
        }
        
        //  CONNECTED

        case 1 :           
        {
            //  Draw green sphere

            oledc_image(
                            UI_CON_ICON_BMP, 
                            UI_CON_ICON_XOFF, 
                            UI_CON_ICON_YOFF
                        );

            break;
        }
        default :

        break;
    }
}

static void display_update_fan ( void )
{
    static uint8_t      fan_pos;
    FAN_STATE           fan;

    fan = HVAC_GetFanState( );

    oledc_set_pen_color( UI_BACKGROUND_COLOR );
    oledc_draw_field(
                        UI_FAN_ICON_XOFF, 
                        UI_FAN_ICON_YOFF, 
                        UI_FAN_ICON_XOFF + UI_FAN_ICON_W - 1, 
                        UI_FAN_ICON_YOFF + UI_FAN_ICON_H - 1
                    );

    switch ( fan )
    {
        case FAN_OFF:
        {
            // Draw inactive fan

            oledc_set_pen_color( UI_INACTIVE_COLOR );
            oledc_draw_bitmap_c(
                                    UI_FAN_ICON[ fan_pos ],
                                    UI_FAN_ICON_XOFF, 
                                    UI_FAN_ICON_YOFF, 
                                    UI_FAN_ICON_XOFF + UI_FAN_ICON_W - 1,
                                    UI_FAN_ICON_YOFF + UI_FAN_ICON_H - 1
                                );
            break;
        }

        case FAN_LOW:
        case FAN_HIGH:
        {
            fan_pos = ++fan_pos % UI_FAN_ICON_FRAMES;

            oledc_set_pen_color( UI_ACTIVE_COLOR );
            oledc_draw_bitmap_c(
                                    UI_FAN_ICON[ fan_pos ],
                                    UI_FAN_ICON_XOFF, 
                                    UI_FAN_ICON_YOFF, 
                                    UI_FAN_ICON_XOFF + UI_FAN_ICON_W - 1,
                                    UI_FAN_ICON_YOFF + UI_FAN_ICON_H - 1
                                );
            break;
        }
        default:
        break;
    }
}

static void display_update_wave ( void )
{
    FAN_STATE       fan;
    AIRCON_STATE    aircon;

    fan = HVAC_GetFanState( );
    aircon = HVAC_GetAirconState( );

    oledc_set_pen_color( UI_BACKGROUND_COLOR );
    oledc_draw_field(
                        UI_WAV_ICON_XOFF, 
                        UI_WAV_ICON_YOFF, 
                        UI_WAV_ICON_XOFF + UI_WAV_ICON_W - 1, 
                        UI_WAV_ICON_YOFF + UI_WAV_ICON_H - 1
                    );

    switch ( aircon )
    {
        case AIRCON_OFF :       oledc_set_pen_color( UI_INACTIVE_COLOR ); break;
        case AIRCON_HEATING :   oledc_set_pen_color( UI_HEATING_COLOR );  break;
        case AIRCON_COOLING :   oledc_set_pen_color( UI_COOLING_COLOR );  break;
        default:
        break;
    }

    switch ( fan )
    {
        case FAN_LOW :
        {
            oledc_draw_bitmap_c(
                                    UI_WAV_ICON_LO,
                                    UI_WAV_ICON_XOFF, 
                                    UI_WAV_ICON_YOFF, 
                                    UI_WAV_ICON_XOFF + UI_WAV_ICON_W - 1, 
                                    UI_WAV_ICON_YOFF + UI_WAV_ICON_H - 1
                                );

            break;
        }
        case FAN_HIGH : 
        {
            oledc_draw_bitmap_c(
                                    UI_WAV_ICON_HI,
                                    UI_WAV_ICON_XOFF, 
                                    UI_WAV_ICON_YOFF, 
                                    UI_WAV_ICON_XOFF + UI_WAV_ICON_W - 1, 
                                    UI_WAV_ICON_YOFF + UI_WAV_ICON_H - 1
                                );

            break;
        }
        case FAN_OFF :      // Fan off no Wave drawing
        default:
        break;
    }
}

static void display_update_sensor_values ( float temp, float hum )
{
    uint8_t tmp_txt[16] = "n/a";

    // Display temperature.

    if (temp != INIT_VAL)
    {
        sprintf(tmp_txt, "%.1f", temp);
    }

    //  Erase current temperature content.

    oledc_set_pen_color( UI_BACKGROUND_COLOR );
    oledc_draw_field( 
                        UI_TEM_CUR_VAL_XOFF, 
                        UI_TEM_CUR_VAL_YOFF, 
                        UI_TEM_CUR_VAL_XOFF + UI_TEM_CUR_VAL_W - 1, 
                        UI_TEM_CUR_VAL_YOFF + UI_TEM_CUR_VAL_H - 1
                    );

    //  Write new temperature values.

    oledc_set_pen_color( UI_FONT_COLOR );
    oledc_text( 
                    UI_BIG_FONT, 
                    tmp_txt, 
                    UI_TEM_CUR_VAL_XOFF, 
                    UI_TEM_CUR_VAL_YOFF
                );

    // Display humidity.

    if (hum != INIT_VAL)
    {
        sprintf(tmp_txt, "%.1f", hum);
    }

    //  Erase current humidity content.

    oledc_set_pen_color( UI_BACKGROUND_COLOR );
    oledc_draw_field( 
                        UI_HUM_CUR_VAL_XOFF, 
                        UI_HUM_CUR_VAL_YOFF, 
                        UI_HUM_CUR_VAL_XOFF + UI_HUM_CUR_VAL_W - 1, 
                        UI_HUM_CUR_VAL_YOFF + UI_HUM_CUR_VAL_H - 1
                    );

    //  Write new humidity values.

    oledc_set_pen_color( UI_FONT_COLOR );
    oledc_text( 
                    UI_BIG_FONT, 
                    tmp_txt, 
                    UI_HUM_CUR_VAL_XOFF, 
                    UI_HUM_CUR_VAL_YOFF
                );
}

static void display_update_target_temperature ( float temp )
{
    uint8_t tmp_txt[16] = "n/a";

    if (temp != INIT_VAL)
    {
        sprintf(tmp_txt, "%.1f", temp);
    }

    //  Erase current content.

    oledc_set_pen_color( UI_BACKGROUND_COLOR );
    oledc_draw_field( 
                        UI_TEM_TAR_VAL_XOFF, 
                        UI_TEM_TAR_VAL_YOFF, 
                        UI_TEM_TAR_VAL_XOFF + UI_TEM_TAR_VAL_W - 1, 
                        UI_TEM_TAR_VAL_YOFF + UI_TEM_TAR_VAL_H - 1
                    );

    oledc_set_pen_color( UI_FONT_COLOR );
    oledc_text(
                    UI_SMALL_FONT, 
                    tmp_txt, 
                    UI_TEM_TAR_VAL_XOFF, 
                    UI_TEM_TAR_VAL_YOFF
                );
}

static void display_update_target_humidity ( float hum )
{
    uint8_t tmp_txt[16] = "n/a";

    if (hum != INIT_VAL)
    {
        sprintf(tmp_txt, "%.1f", hum);
    }

    //  Erase current content.

    oledc_set_pen_color( UI_BACKGROUND_COLOR );
    oledc_draw_field( 
                        UI_HUM_TAR_VAL_XOFF, 
                        UI_HUM_TAR_VAL_YOFF, 
                        UI_HUM_TAR_VAL_XOFF + UI_HUM_TAR_VAL_W - 1, 
                        UI_HUM_TAR_VAL_YOFF + UI_HUM_TAR_VAL_H - 1
                    );

    oledc_set_pen_color( UI_FONT_COLOR );
    oledc_text(
                    UI_SMALL_FONT, 
                    tmp_txt, 
                    UI_HUM_TAR_VAL_XOFF, 
                    UI_HUM_TAR_VAL_YOFF
                );
}

/* -------------------------------------------------------------------------- */
/*
    display.c

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