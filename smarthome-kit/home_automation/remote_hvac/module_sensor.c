/*
    sensor.c

-----------------------------------------------------------------------------

  Copyright (c) 2017, MikroElektonika - http://www.mikroe.com

  All rights reserved.

----------------------------------------------------------------------------- */

#include "module_sensor.h"
#include "../../../mikroe/Weather/click_weather.h"

/* ---------------------------------------------------------------- VARIABLES */
//                                                                  ---------

/**
    \biref Sensor Data

This structure holds the Sensor's data.

    \note
This structure should be initialized by the SENSOR_Initialize function.
    
*/
SENSOR_DATA sensorData;

/* -------------------------------------------------------- PRIVATE FUNCTIONS */
//                                                          -----------------

static void _SENSOR_Tasks ( void );

static int read_sensor_measurement ( SENSOR_VALUE *data );

/* --------------------------------------------------------- PUBLIC FUNCTIONS */
//                                                           ----------------

/**
    \brief Senosor Module Initalization
*/
void SENSOR_Initialize ( void )
{
    /* 
        Place the Sensor state machine, measurement fields initial state.
        Also initializes click driver and creates Sensor Task.
    */

    sensorData.state             = MODULE_STATE_INIT;
    sensorData.value.temperature = 0.0f;
    sensorData.value.humidity    = 0.0f;
    sensorData.value.pressure    = 0.0f;

    weather_spiDriverInit( NULL, NULL );
    
    //  Create OS Thread for SENSOR Tasks.

    xTaskCreate( (TaskFunction_t) _SENSOR_Tasks, "Sensor Task",
                SENSOR_TASK_STACK_SIZE, NULL, SENSOR_TASK_PRIORITY, NULL );
}

/**
    \brief Sensor Module Task
*/
void SENSOR_Tasks ( void )
{
    switch ( sensorData.state )
    {
        /* 
            Sensor INIT state.

            Entered only after initialization of the module.

            During this state module reads calibration parameters and 
            configures the weather click board.
        */

        case MODULE_STATE_INIT:
        {   
            /*
                Weather click driver initialization, configuration and read 
                of calibration data.
            */         

            weather_setStandbyTime( _WEATHER_STANDBY_TIME_1_MS );
            weather_setFilterCoefficient( _WEATHER_FILTER_COEFF_16 );
            weather_setOversamplingTemperature( _WEATHER_OVERSAMP_2X );
            weather_setOversamplingHumidity( _WEATHER_OVERSAMP_1X );
            weather_setOversamplingPressure( _WEATHER_OVERSAMP_16X );
            weather_setOversamplingMode( _WEATHER_NORMAL_MODE );
            weather_readCalibrationParams( );

            sensorData.state = MODULE_STATE_PREACTIVE;
    
            break;
        }
        
        /*
            Sensor PREACTIVE State. 

            During this state sensor perform single measurement and send data to 
            HVAC using appropriate queue. This very first measurement will be 
            used as initial HVAC target value.
        */
        
        case MODULE_STATE_PREACTIVE:
        {
            if ( read_sensor_measurement( &sensorData.value ) == MODULE_OK )
            {
                //  Send measurement to HVAC module 

                if ( xQueueSend( qHVAC_Sensor, &sensorData.value, 
                            RTOS_NO_BLOCKING ) )
                {
                    //  TODO : Handle error.
                }

                //  Send measurement to HVAC module as initial target values

                if ( xQueueSend( qHVAC_TargetT, &sensorData.value, 
                            RTOS_NO_BLOCKING ) )
                {
                    //  TODO : Handle error.
                }

                sensorData.state = MODULE_STATE_ACTIVE;
            }
            
            break;
        }

        /*
            Sensor ACTIVE State. 

            During this state sensor perform measurements and sends data to 
            HVAC modules using appropriate queue.

        */

        case MODULE_STATE_ACTIVE:
        {

            if ( read_sensor_measurement( &sensorData.value ) == MODULE_OK )
            {
                //  Send measurement to HVAC module

                if ( xQueueSend( qHVAC_Sensor, &sensorData.value, 
                            RTOS_NO_BLOCKING ) )
                {
                    //  TODO : Handle error.
                }
            }
    
            break;
        }

        case MODULE_STATE_INACTIVE:
        case MODULE_STATE_POSTACTIVE:
        default:
        {
            //  TODO : Handle error.

            break;
        }
    }
}

void SENSOR_GetLastMerasurement ( SENSOR_VALUE *value )
{
    memcpy( value, &sensorData.value, sizeof( SENSOR_VALUE ) );
}

/* -------------------------------------------------------- PRIVATE FUNCTIONS */
//                                                          -----------------

static void _SENSOR_Tasks ( void )
{
    for ( ; ; )
    {
        SENSOR_Tasks( );
        vTaskDelay( SENSOR_TASK_DELAY / portTICK_PERIOD_MS );
    }
}

static int read_sensor_measurement ( SENSOR_VALUE *data )
{
    int retval = MODULE_ERROR;

    /*
        Take semaphore, read Weather click measurement data, give back 
        semaphore and update HVAC sensor queue.
    */

    if ( xSemaphoreTake( smphrSPI1, RTOS_NO_BLOCKING ) )
    {        
        weather_getWeather( &data->temperature, &data->humidity, &data->pressure );
        xSemaphoreGive( smphrSPI1 );

        retval = MODULE_OK;
    }

    return retval;
}

/* -------------------------------------------------------------------------- */
/*
    sensor.c

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
   must display the following acknowledgment:
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