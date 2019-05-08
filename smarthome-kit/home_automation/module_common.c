/*
    module_common.c

    | Global Library Prefix | **MODULE**            |
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

#include "module_common.h"

#include "remote_hvac/module_hvac.h"
#include "remote_hvac/module_sensor.h"
#include "remote_hvac/module_thermostat.h"
#include "remote_hvac/module_display.h"
#include "../home_automation/aws_home_automation_demo.h"

/* ---------------------------------------------------------------- VARIABLES */
//                                                                  ---------

SemaphoreHandle_t    smphrSPI1;                 // THERMOSTAT / SENSOR
SemaphoreHandle_t    smphrSPI2;                 // CONN / DISPLAY

QueueHandle_t        qHVAC_Fan;                 // CONN             -> HVAC
QueueHandle_t        qHVAC_Sensor;              // SENSOR           -> HVAC
QueueHandle_t        qHVAC_TargetT;             // THERMOSTAT/CONN  -> HVAC
    
QueueHandle_t        qCONN_Fan;                 // HVAC             -> CONN
QueueHandle_t        qCONN_Aircon;              // HVAC             -> CONN (desired)
QueueHandle_t        qCONN_Sensor;              // HVAC             -> CONN (desired)
QueueHandle_t        qCONN_TargetT;             // HVAC             -> CONN (desired)
QueueHandle_t        qCONN_ShadowReported;      // HVAC             -> CONN (reported)

QueueHandle_t        qDISPLAY_Fan;              // HVAC             -> DISPLAY
QueueHandle_t        qDISPLAY_Aircon;           // HVAC             -> DISPLAY
QueueHandle_t        qDISPLAY_Sensor;           // HVAC             -> DISPLAY
QueueHandle_t        qDISPLAY_TargetT;          // HVAC             -> DISPLAY
QueueHandle_t        qDISPLAY_Conn;             // CONN             -> DISPLAY

/* --------------------------------------------------------- PUBLIC FUNCTIONS */
//                                                           ----------------

void MODULES_Initialize ( void )
{
    smphrSPI1           = xSemaphoreCreateMutex( );
    smphrSPI2           = xSemaphoreCreateMutex( );
    
    qHVAC_Fan           = xQueueCreate( 4, sizeof( FAN_STATE ) );
    qHVAC_Sensor        = xQueueCreate( 4, sizeof( SENSOR_VALUE ) );
    qHVAC_TargetT       = xQueueCreate( 4, sizeof( float ) );

    qCONN_Fan           = xQueueCreate( 4, sizeof( FAN_STATE ) );
    qCONN_Aircon        = xQueueCreate( 4, sizeof( AIRCON_STATE ) );
    qCONN_Sensor        = xQueueCreate( 4, sizeof( SENSOR_VALUE ) );
    qCONN_TargetT       = xQueueCreate( 4, sizeof( float ) );
    qCONN_ShadowReported= xQueueCreate( 4, sizeof( xShadowProperties ) );

    qDISPLAY_Fan        = xQueueCreate( 4, sizeof( FAN_STATE ) );
    qDISPLAY_Aircon     = xQueueCreate( 4, sizeof( AIRCON_STATE ) );
    qDISPLAY_Sensor     = xQueueCreate( 4, sizeof( SENSOR_VALUE ) );
    qDISPLAY_Conn       = xQueueCreate( 4, sizeof( int ) );
    qDISPLAY_TargetT    = xQueueCreate( 4, sizeof( float ) );
}


/* -------------------------------------------------------------------------- */
/*
    module_common.c

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