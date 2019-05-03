/*
    hvac.h

-----------------------------------------------------------------------------
  
  Copyright (c) 2017, MikroElektonika - http://www.mikroe.com

  All rights reserved.

----------------------------------------------------------------------------- */
/**
    \file     hvac.h
    \brief    HVAC Module
    \defgroup HVAC
    \brief    HVAC Module
    \{

| Global Library Prefix | **HVAC**              |
|:---------------------:|:---------------------:|
| Version               | **1.0.0**             |
| Date                  | **Feb 2019.**         |
| Developer             | **Milos Vidojevic**   |

---

**Version Info :**
- **1.0.0** Module Created [Milos Vidojevic]

*/
/* -------------------------------------------------------------------------- */

#ifndef _HVAC_H
#define _HVAC_H

#include "../module_common.h"
#include "../remote_hvac/module_sensor.h"
#include "../remote_hvac/module_hvac.h"

extern const uint8_t FAN_STATE_STRING[3][5];

extern const uint8_t HVAC_STATE_STRING[2][9];

extern const uint8_t AIRCON_STATE_STRING[3][5];

/* -------------------------------------------------------------------- TYPES */

typedef enum
{
    FAN_OFF             = 0,
    FAN_LOW,
    FAN_HIGH

} FAN_STATE;

typedef enum 
{
    HVAC_INACTIVE       = 0,
    HVAC_ACTIVE

} HVAC_STATE;

typedef enum
{
    AIRCON_OFF          = 0,
    AIRCON_HEATING,
    AIRCON_COOLING

} AIRCON_STATE;

typedef struct 
{
    MODULE_STATE        state;

    HVAC_STATE          hvac;
    FAN_STATE           fan;
    FAN_STATE           fan_bkp;
    AIRCON_STATE        aircon;
    SENSOR_VALUE        current;
    float               target_temp;

} HVAC_DATA;

#ifdef __cplusplus
extern "C" {
#endif

/* ---------------------------------------------------------------- FUNCTIONS */

/**
    \brief HVAC Initialization Routine.

This function initializes the HVAC module.

*/
void HVAC_Initialize ( void );

/**
    \brief HVAC Tasks Function

This routine is the HVAC module tasks function. 

*/
void HVAC_Tasks ( void );

float HVAC_GetTargetTemperature ( void );

FAN_STATE HVAC_GetFanState ( void );

HVAC_STATE HVAC_GetHvacState ( void );

AIRCON_STATE HVAC_GetAirconState ( void );

#ifdef __cplusplus
}
#endif
#endif

/// \}
/* -------------------------------------------------------------------------- */
/*
    hvac.h

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