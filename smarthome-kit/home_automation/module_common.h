/*
    module_common.h

-----------------------------------------------------------------------------
  
  Copyright (c) 2017, MikroElektonika - http://www.mikroe.com

  All rights reserved.

----------------------------------------------------------------------------- */
/**
    \file     module_common.h
    \brief    Modules Common
    \defgroup MODULE
    \brief    Module Common
    \{

| Global Library Prefix | **MODULE**            |
|:---------------------:|:---------------------:|
| Version               | **1.0.0**             |
| Date                  | **Feb 2019.**         |
| Developer             | **Milos Vidojevic**   |

---

**Version Info :**
- **1.0.0** Module Created [Milos Vidojevic]

*/
/* -------------------------------------------------------------------------- */

#ifndef _MODULE_COMMON_H_
#define _MODULE_COMMON_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "system_config.h"
#include "system_definitions.h"

#include "semphr.h"
#include "queue.h"

#include "aws_logging_task.h"

/* ------------------------------------------------------------------- MACROS */

#define RTOS_NO_BLOCKING            0

#define SENSOR_TASK_STACK_SIZE      256
#define THERMOSTAT_TASK_STACK_SIZE  256
#define HVAC_TASK_STACK_SIZE        2048
#define DISPLAY_TASK_STACK_SIZE     20480
#define CONNECTOR_TASK_STACK_SIZE   3072

#define SENSOR_TASK_PRIORITY        1
#define THERMOSTAT_TASK_PRIORITY    1
#define HVAC_TASK_PRIORITY          1
#define DISPLAY_TASK_PRIORITY       1
#define CONNECTOR_TASK_PRIORITY     2

#define SENSOR_TASK_DELAY           5000
#define THERMOSTAT_TASK_DELAY       1
#define HVAC_TASK_DELAY             1
#define DISPLAY_TASK_DELAY          1
#define CONNECTOR_TASK_DELAY        1

#define jsonFAN_REFERENCE           ("FAN")
#define jsonAIRCON_REFERENCE        ("AIRCON")
#define jsonSENSOR_T_REFERENCE      ("SENSOR_T")
#define jsonSENSOR_H_REFERENCE      ("SENSOR_H")
#define jsonTARGET_T_REFERENCE      ("TARGET_T")
    
#define mqttSENSOR_PAYLOAD          ("{\"%s\":\"%.1f\", \"%s\":\"%.1f\"}")
#define mqttFAN_REPORTED_PAYLOAD    ("{ \"state\":{\"reported\":{\"%s\":\"%s\"}}}")
#define mqttFAN_DESIRED_PAYLOAD     ("{ \"state\":{\"desired\":{\"%s\":\"%s\"}}}")
#define mqttAIRCON_REPORTED_PAYLOAD ("{ \"state\":{\"reported\":{\"%s\":\"%s\"}}}")
#define mqttAIRCON_DESIRED_PAYLOAD  ("{ \"state\":{\"desired\":{\"%s\":\"%s\"}}}")
#define mqttTARGET_REPORTED_PAYLOAD ("{ \"state\":{\"reported\":{\"%s\":\"%.1f\"}}}")
#define mqttTARGET_DESIRED_PAYLOAD  ("{ \"state\":{\"desired\":{\"%s\":\"%.1f\"}}}")

#define logSENSOR_PAYLOAD           ("<\"%s\":\"%.1f\", \"%s\":\"%.1f\">\r\n")
#define logFAN_PAYLOAD              ("<\"%s\":\"%s\">\r\n")
#define logAIRCON_PAYLOAD           ("<\"%s\":\"%s\">\r\n")
#define logTARGET_PAYLOAD           ("<\"%s\":\"%.1f\">\r\n")

/* -------------------------------------------------------------------- TYPES */

typedef enum 
{
    MODULE_OK                   = 0,
    MODULE_ERROR
    
} MODULE_RETURN;

/** 
    \enum MODULE_COMMON_STATES
    \brief Module common states

This enumeration defines the valid module states. These states determine the 
behavior of the module at various times.

*/
typedef enum
{
    MODULE_STATE_INIT           = 0,
    MODULE_STATE_INACTIVE,
    MODULE_STATE_PREACTIVE,
    MODULE_STATE_ACTIVE,
    MODULE_STATE_POSTACTIVE

} MODULE_STATE;

/* ----------------------------------------------------------- RTOS VARIABLES */

extern SemaphoreHandle_t    smphrSPI1;
extern SemaphoreHandle_t    smphrSPI2;

extern QueueHandle_t        qHVAC_Fan;
extern QueueHandle_t        qHVAC_Sensor;
extern QueueHandle_t        qHVAC_TargetT;

extern QueueHandle_t        qCONN_Fan;
extern QueueHandle_t        qCONN_Aircon;
extern QueueHandle_t        qCONN_Sensor;
extern QueueHandle_t        qCONN_TargetT;
extern QueueHandle_t        qCONN_ShadowReported;

extern QueueHandle_t        qDISPLAY_Fan;
extern QueueHandle_t        qDISPLAY_Aircon;
extern QueueHandle_t        qDISPLAY_Sensor;
extern QueueHandle_t        qDISPLAY_TargetT;
extern QueueHandle_t        qDISPLAY_Conn;

#ifdef __cplusplus
extern "C" {
#endif

/**
    \brief Module Common Initalization
*/
void MODULES_Initialize ( void );

#ifdef __cplusplus
}
#endif
#endif

/// \} 
/* -------------------------------------------------------------------------- */
/*
    module_common.h

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
