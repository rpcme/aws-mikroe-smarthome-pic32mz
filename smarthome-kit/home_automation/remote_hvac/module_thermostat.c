/*
    thermostat.c

-----------------------------------------------------------------------------

  Copyright (c) 2017, MikroElektonika - http://www.mikroe.com

  All rights reserved.

----------------------------------------------------------------------------- */

#include "module_thermostat.h"
#include "module_hvac.h"
#include "../../../mikroe/Rotary/click_rotary.h"

/* ------------------------------------------------------------------- MACROS */
//                                                                     ------

#define THERMOSTAT_RESOLUTION      0.2f

/* ---------------------------------------------------------------- CONSTANTS */
//                                                                  ---------

/*
    Lookup tables for rotary ring negative values.
*/
static const uint16_t NEG_VALUES_TABLE[16] =
{
    0x0001, 0x8001, 0xC001, 0xE001, 0xF001, 0xF801, 0xFC01, 0xFE01, 
    0xFF01, 0xFF81, 0xFFC1, 0xFFE1, 0xFFF1, 0xFFF9, 0xFFFE, 0xFFFF
};

/*
    Lookup tables for rotary ring positive values.
*/
static const uint16_t POS_VALUES_TABLE[16] =
{
    0x0001, 0x0003, 0x0007, 0x000F, 0x001F, 0x003F, 0x007F, 0x00FF, 
    0x01FF, 0x03FF, 0x07FF, 0x0FFF, 0x1FFF, 0x3FFF, 0x7FFF, 0xFFFF
};

/* ---------------------------------------------------------------- VARIABLES */
//                                                                  ---------

/**
    \biref Thermostat Data

This structure holds the Thermostat's data.

    \note
This structure should be initialized by the THERMOSTAT_Initialize function.
    
*/
THERMOSTAT_DATA thermostatData;

/* -------------------------------------------------------- PRIVATE FUNCTIONS */
//                                                          -----------------

static void _THERMOSTAT_Tasks ( void );

/* --------------------------------------------------------- PUBLIC FUNCTIONS */
//                                                           ----------------

/**
    \brief Thermostat Module Initalization
*/
void THERMOSTAT_Initialize ( void )
{
    //  Place the Thermostat state machine and all fields in it's initial state.

    thermostatData.state          = MODULE_STATE_INIT;
    thermostatData.value.rotation = ROTARY_UNKNOWN_MOVEMENT;
    thermostatData.value.position = 0;

    rotary_spiDriverInit( NULL, NULL );
    
    // Create OS Thread for THERMOSTAT Tasks.

    xTaskCreate( ( TaskFunction_t ) _THERMOSTAT_Tasks, "Thermostat Task",
            THERMOSTAT_TASK_STACK_SIZE, NULL, THERMOSTAT_TASK_PRIORITY, NULL );
}

/**
    \brief Thermostat Module Task
*/
void THERMOSTAT_Tasks ( void )
{
    switch ( thermostatData.state )
    {
        /* 
            Thermostat initial state.
            Entered only after initializaion of the module.
        */

        case MODULE_STATE_INIT:
        {   
            //  Put module to default INACTIVE state.

            thermostatData.state = MODULE_STATE_INACTIVE;
    
            break;
        }

        /*
            Thermostat INACTIVE state.

            This state is enered after module INIT state or can be entered by 
            pressing the Rotary stick while module is in ACTIVE state. 
            
            During this state LED ring is turned off.

            This state can be only left by pressing the Rotary stick.
        */

        case MODULE_STATE_INACTIVE:
        {
            //  Nothing to do here.
             
            break;
        }

        /*
            Thermostat transition state from INACTIVE to ACTIVE state. 

            Initial value which will be shown on ring is always 1.
            
            SPI bus is taken by Rotary click and thermostat module to avoid 
            glitches on OLED in case of SPI driver interrupt mode usage.

            SPI polling mode is not tested. 
        */

        case MODULE_STATE_PREACTIVE:
        {
            //  Take SPI semaphore.

            if ( xSemaphoreTake( smphrSPI1, 10 ) )
            {
                //  Turn on LED ring with default value and activate module.

                rotary_enable( );
                rotary_write_ledring( 1 );

                thermostatData.target   = HVAC_GetTargetTemperature( );
                thermostatData.state    = MODULE_STATE_ACTIVE;
            }

            break;
        }
    
        /*
            Thermostate ACTIVE state.

            Durnig this state module observes encoder lines and depends on it 
            updates LED ring value. During this state it is necessary to avoid 
            execution of any other operation 

            This state can be only left by pressing the Rotary stick.
        */

        case MODULE_STATE_ACTIVE:
        {   
            //  Read encoder rotation.

            thermostatData.value.rotation = rotary_read_encoder( );
            
            //  Increment or decrement position depends on rotation.

            switch ( thermostatData.value.rotation )
            {
                case ROTARY_CW_DIRECTION :
                {
                    ++thermostatData.value.position;
                    thermostatData.target += THERMOSTAT_RESOLUTION;

                    
                break;
                }
                case ROTARY_CCW_DIRECTION :
                {
                    --thermostatData.value.position;
                    thermostatData.target -= THERMOSTAT_RESOLUTION;
                    
                break;
                }
                case ROTARY_NO_MOVEMENT :
                case ROTARY_UNKNOWN_MOVEMENT :
        
                    //  Nothing to do here.

                break;
                default:

                    //  TODO : Handle error.
                
                break;
            }

            //  Draw position on LED ring.

            if ( thermostatData.value.position < 0 )
            {
                uint8_t lut;

                lut = ( ( -thermostatData.value.position ) / 2 ) % 15;
                rotary_write_ledring( NEG_VALUES_TABLE[ lut ] );
            }
            else
            {
                uint8_t lut;

                lut = ( thermostatData.value.position / 2 ) % 15;
                rotary_write_ledring( POS_VALUES_TABLE[ lut ] );
            }

            break;
        }

        /*
            Thermostat transition state from ACTIVE to INACTIVE state. 

            Durnig this state thermostat module send informations to Logger and 
            HVAC modules using queues and also gives SPI mutex semaphore to 
            display module. 

            LED ring will be turned off and module will be ready for INACTIVE 
            state.
        */

        case MODULE_STATE_POSTACTIVE:
        {
            
            //  Turn off Led Ring and reset position.

            rotary_write_ledring( 0 );
            rotary_disable( );

            //  Add data to queue

            if ( xQueueSend( qHVAC_TargetT, &thermostatData.target, 
                        RTOS_NO_BLOCKING ) )
            {
                //  TODO : Handle error.
            }

            //  Give semaphore back to sensor

            if ( xSemaphoreGive( smphrSPI1 ) )
            {
                //  TODO : Handle error.
            }

            thermostatData.value.position = 0;
            thermostatData.state = MODULE_STATE_INACTIVE;
    
            break;
        }
    
        default:
        {
            //  TODO : Handle error in application's state machine. 

            break;
        }
    }
}

void THERMOSTAT_ISR_Handler ( void )
{
    switch ( thermostatData.state )
    {
        case MODULE_STATE_INACTIVE:
        {
            thermostatData.state = MODULE_STATE_PREACTIVE;

            break;
        }
        case MODULE_STATE_ACTIVE:
        {
            thermostatData.state = MODULE_STATE_POSTACTIVE;

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

static void _THERMOSTAT_Tasks ( void )
{
    for ( ; ; )
    {
        THERMOSTAT_Tasks( );
        vTaskDelay( THERMOSTAT_TASK_DELAY / portTICK_PERIOD_MS );
    }
}

/* -------------------------------------------------------------------------- */
/*
    thermostat.c

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