/*
    hvac.c

    | Global Library Prefix | **HVAC**              |
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

#include "../remote_hvac/module_hvac.h"
#include "../remote_hvac/module_thermostat.h"
#include "../remote_hvac/module_display.h"

/* ------------------------------------------------------------------- MACROS */
//                                                                     ------

#define HVAC_ACTIVE_THRESHOLD           0.5f
#define HVAC_INACTIVE_THRESHOLD         HVAC_ACTIVE_THRESHOLD + 1.0f

/* ---------------------------------------------------------------- CONSTANTS */
//

const uint8_t FAN_STATE_STRING[ 3 ][ 5 ] =
{
    "off", "low", "high"
};

/*
const uint8_t HVAC_STATE_STRING[ 2 ][ 9 ] =
{
    "active", "inactive"
};
*/

const uint8_t AIRCON_STATE_STRING[ 3 ][ 5 ] =
{
    "off", "heat", "cool"
};

/* ---------------------------------------------------------------- VARIABLES */
//                                                                  ---------

/**
    \biref HVAC Data

This structure holds the HVACHVAC's data.

    \note
This structure should be initialized by the HVAC_Initialize function.
    
*/
HVAC_DATA hvacData;

/* -------------------------------------------------------- PRIVATE FUNCTIONS */
//                                                          -----------------

static void _HVAC_Tasks ( void );

static void hvac_update_fan ( FAN_STATE fan );

static void hvac_update_aircon( AIRCON_STATE aircon );

static void hvac_update_hvac( HVAC_STATE hvac );

static void hvac_update_sensor( SENSOR_VALUE sensor );

static void hvac_update_target_t( float temp );

static int calulate_aircon_status ( void );

static int check_hvac_button ( void );

static int check_fan_button ( void );

/* --------------------------------------------------------- PUBLIC FUNCTIONS */
//                                                           ----------------

/**
    \brief HVAC Module Initalization
*/
void HVAC_Initialize ( void )
{
    //  Place the HVAC state machine and all fields in it's initial state.

    hvacData.state                = MODULE_STATE_INIT;
    hvacData.fan                  = FAN_OFF;
    hvacData.hvac                 = HVAC_INACTIVE;
    hvacData.aircon               = AIRCON_OFF;
    hvacData.target_temp          = 0.0f;

    hvacData.current.temperature  = 0.0f;
    hvacData.current.humidity     = 0.0f;
    hvacData.current.pressure     = 0.0f;
    
    //  Create OS Thread for THERMOSTAT Tasks.

    xTaskCreate( (TaskFunction_t) _HVAC_Tasks, "HVAC Tasks",
            HVAC_TASK_STACK_SIZE, NULL, HVAC_TASK_PRIORITY, NULL );
}

/**
    \brief HVAC Module Task
*/
void HVAC_Tasks ( void )
{
    switch ( hvacData.state )
    {
        /* 
            HVAC INIT state.

            Entered only after initializaion of the module.
            
            During this state HVAC waiting for sensor to send first queue which 
            will be taken as initial target value.
        */

        case MODULE_STATE_INIT:
        {
            SENSOR_VALUE        sval;   

            //  Wait for sensor to initialize.

            if ( xQueueReceive( qHVAC_Sensor, 
                        (SENSOR_VALUE *) &sval, RTOS_NO_BLOCKING ) )
            {
                hvacData.target_temp            = sval.temperature;
                hvacData.current.temperature    = sval.temperature;
                hvacData.current.humidity       = sval.humidity;
                hvacData.current.pressure       = sval.pressure;

                hvacData.state = MODULE_STATE_ACTIVE;
            }

            hvacData.state = MODULE_STATE_ACTIVE;

            break;
        }

        /*  
            HVAC ACTIVE state.

            Entered after init state. This state is devided to substates.
        */

        case MODULE_STATE_ACTIVE:
        {
            SENSOR_VALUE        sensorv;
            HVAC_STATE          hvacv;
            FAN_STATE           fanv;
            float               targetv;
            
            /* 
                FAN button press detection and status update. Note that button 
                is capable only to change FAN status in defined order.
            */

            if ( check_fan_button( ) )
            {
                if ( hvacData.hvac == HVAC_INACTIVE )
                {
                    hvacData.hvac = HVAC_ACTIVE;
                }

                //  Switch states in range 0 to 2.

                hvacData.fan = ( ++hvacData.fan % 3 );
                hvacData.fan_bkp = hvacData.fan;

                hvac_update_fan( hvacData.fan );
            }

            //  FAN status update received from CONN module.

            if ( xQueueReceive( qHVAC_Fan, (FAN_STATE *) &fanv, 
                        RTOS_NO_BLOCKING ) )
            {
                if ( hvacData.hvac == HVAC_INACTIVE )
                {
                    hvacData.hvac = HVAC_ACTIVE;
                }

                hvacData.fan = fanv;
                hvacData.fan_bkp = hvacData.fan;

                hvac_update_fan( hvacData.fan );
            }

            //  HVAC button press detection and status update.

            if ( check_hvac_button( ) )
            {
                if ( hvacData.hvac == HVAC_INACTIVE )
                {
                    hvacData.hvac = HVAC_ACTIVE;
                    
                    //  Restore last FAN state.

                    hvacData.fan = hvacData.fan_bkp; 
                    hvac_update_fan( hvacData.fan );

                    if ( calulate_aircon_status( ) )
                    {
                        hvac_update_aircon( hvacData.aircon );
                    }
                }
                else
                {
                    hvacData.hvac = HVAC_INACTIVE;

                    hvacData.fan = FAN_OFF;
                    hvacData.aircon = AIRCON_OFF;
                    hvac_update_fan( hvacData.fan );
                    hvac_update_aircon( hvacData.aircon );
                }
            }

            //  New measurements received from SENSOR module.

            if ( xQueueReceive( qHVAC_Sensor, (SENSOR_VALUE *) &sensorv, 
                        RTOS_NO_BLOCKING ) )
            {
                hvacData.current.temperature = sensorv.temperature;
                hvacData.current.humidity = sensorv.humidity;

                hvac_update_sensor( sensorv );

                if ( calulate_aircon_status( ) )
                {
                    hvac_update_aircon( hvacData.aircon );
                }
            }

            //  New target temperature received from THERMOSTAT or CONN module.

            if ( xQueueReceive( qHVAC_TargetT, (float *) &targetv, 
                        RTOS_NO_BLOCKING ) )
            {
                hvacData.target_temp = targetv;

                hvac_update_target_t( hvacData.target_temp );

                if ( calulate_aircon_status( ) )
                {
                    hvac_update_aircon( hvacData.aircon );
                }
            }

            break;
        }

        // HVAC other states.

        case MODULE_STATE_INACTIVE:
        case MODULE_STATE_PREACTIVE:
        case MODULE_STATE_POSTACTIVE:
        default:
        {
            //  TODO : Handle error.

            break;
        }
    }
}

float HVAC_GetTargetTemperature ( void )
{
    return hvacData.target_temp;
}

FAN_STATE HVAC_GetFanState ( void )
{
    return hvacData.fan;
}

HVAC_STATE HVAC_GetHvacState ( void )
{
    return hvacData.hvac;
}

AIRCON_STATE HVAC_GetAirconState ( void )
{
    return hvacData.aircon;
}

/* -------------------------------------------------------- PRIVATE FUNCTIONS */
//                                                          -----------------

static void _HVAC_Tasks ( void )
{
    for ( ; ; )
    {
        HVAC_Tasks( );
        vTaskDelay( HVAC_TASK_DELAY / portTICK_PERIOD_MS );
    }
}

static void hvac_update_fan ( FAN_STATE fan )
{
    if ( xQueueSend( qDISPLAY_Fan, &fan, RTOS_NO_BLOCKING ) )
    {
        //  TODO : Handle error.
    }

    if ( xQueueSend( qCONN_Fan, &fan, RTOS_NO_BLOCKING ) )
    {
        //  TODO : Handle error.
    }

    // Log new fan state.
    
    vLoggingPrintf( logFAN_PAYLOAD, jsonFAN_REFERENCE,  
            FAN_STATE_STRING[ fan ] );
}

static void hvac_update_aircon( AIRCON_STATE aircon )
{
    if ( xQueueSend( qDISPLAY_Aircon, &aircon, RTOS_NO_BLOCKING ) )
    {
        //  TODO : Handle error.
    }

    //  Forward sensor data to CONN module

    if ( xQueueSend( qCONN_Aircon, &aircon, RTOS_NO_BLOCKING ) )
    {
        //  TODO : Handle error.
    }

    // Log new fan state.
    
    vLoggingPrintf( logAIRCON_PAYLOAD, jsonAIRCON_REFERENCE,
            AIRCON_STATE_STRING[ aircon ] );
}

static void hvac_update_sensor( SENSOR_VALUE sensor )
{
    //  Forward sensor data to CONN module

    if ( xQueueSend( qCONN_Sensor, &sensor, RTOS_NO_BLOCKING ) )
    {
        //  TODO : Handle error.
    }

    //  Forward sensor data to DISPLAY module

    if ( xQueueSend( qDISPLAY_Sensor, &sensor, RTOS_NO_BLOCKING ) )
    {
        //  TODO : Handle error.
    }

    // Log new data received from the sensor.

    vLoggingPrintf( logSENSOR_PAYLOAD, jsonSENSOR_T_REFERENCE, 
            sensor.temperature, jsonSENSOR_H_REFERENCE, sensor.humidity );
}

static void hvac_update_target_t( float temp )
{
    //  Forward sensor data to CONN module

    if ( xQueueSend( qCONN_TargetT, &temp, RTOS_NO_BLOCKING ) )
    {
        //  TODO : Handle error.
    }

    //  Forward sensor data to DISPLAY module

    if ( xQueueSend( qDISPLAY_TargetT, &temp, RTOS_NO_BLOCKING ) )
    {
        //  TODO : Handle error.
    }

    // Log new data received from the sensor.

    vLoggingPrintf( logTARGET_PAYLOAD, jsonTARGET_T_REFERENCE, temp );
}

static int calulate_aircon_status ( void )
{
    if ( hvacData.hvac == HVAC_INACTIVE )
    {
        return 0;
    }

    switch ( hvacData.aircon )
    {
        //  Forced off state. No changes allowed.

        case AIRCON_OFF:
        {
            /* 
                Check ambient temperature and turn on heating / cooling in case 
                when temperature is out of bounds.
            */

            if ( ( hvacData.current.temperature + HVAC_INACTIVE_THRESHOLD ) 
                        < hvacData.target_temp )
            {
                hvacData.aircon = AIRCON_HEATING;

                return 1;
            }

            if ( ( hvacData.current.temperature - HVAC_INACTIVE_THRESHOLD ) 
                        > hvacData.target_temp )
            {
                hvacData.aircon = AIRCON_COOLING;

                return 1;
            }
        }

        //  Cooling active. 

        case AIRCON_COOLING:
        {
            /* 
                Check ambient temperature and turn of cooling in case when 
                temperature is below target + threshold.
            */

            if ( ( hvacData.current.temperature + HVAC_ACTIVE_THRESHOLD ) 
                        < hvacData.target_temp )
            {
                hvacData.aircon = AIRCON_OFF;

                /* 
                    One more recursive call just for the case when temperature 
                    rapidly change rises / falls so it might happens that 
                    airconditioner can switch directly from heat to cool status.
                */

                (void) calulate_aircon_status( );

                return 1;
            }

            break;
        }

        //  Heating active. 

        case AIRCON_HEATING:
        {
            /* 
                Check ambient temerature and turn of heating in case when 
                temperature is above target + threshold.
            */

            if ( ( hvacData.current.temperature - HVAC_ACTIVE_THRESHOLD ) 
                        > hvacData.target_temp )
            {
                hvacData.aircon = AIRCON_OFF;

                /* 
                    One more recursive call just for the case when temperature 
                    rapidly change rises / falls so it might happens that 
                    airconditioner can switch directly from heat to cool status.
                */

                (void) calulate_aircon_status( );

                return 1;
            }

            break;
        }

        default:
        {
            //  TODO : Handle error.

            break;
        }
    }

    return 0;
}

static int check_hvac_button ( void )
{
    static uint8_t btn1_press;

    //  Rising edge detection.

    if ( BOARDBTN_1StateGet( ) && !btn1_press )
    {
        btn1_press = 1;
    }

    //  Falling edge detection.

    if ( !BOARDBTN_1StateGet( ) && btn1_press )
    {
        btn1_press = 0;

        return 1;
    }

    return 0;
}

static int check_fan_button ( void )
{
    static uint8_t btn2_press;

    //  Rising edge detection.

    if ( BOARDBTN_2StateGet( ) && !btn2_press )
    {
        btn2_press = 1;
    }

    //  Falling edge detection.

    if ( !BOARDBTN_2StateGet( ) && btn2_press )
    {
        btn2_press = 0;

        return 1;
    }

    return 0;
}


/* -------------------------------------------------------------------------- */
/*
    hvac.c

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