/*
    sensor.h

-----------------------------------------------------------------------------
  
  Copyright (c) 2017, MikroElektonika - http://www.mikroe.com

  All rights reserved.

----------------------------------------------------------------------------- */
/**
    \file     sensor.h
    \brief    SENSOR Module
    \defgroup SENSOR
    \brief    SENSOR Module
    \{

| Global Library Prefix | **SENSOR**            |
|:---------------------:|:---------------------:|
| Version               | **1.0.0**             |
| Date                  | **Feb 2019.**         |
| Developer             | **Milos Vidojevic**   |

---

**Version Info :**
- **1.0.0** Module Created [Milos Vidojevic]

*/
/* -------------------------------------------------------------------------- */

#ifndef _SENSOR_H
#define _SENSOR_H

#include "../module_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------- TYPES */

typedef struct
{ 
    float               temperature;
    float               humidity;
    float               pressure;

} SENSOR_VALUE;

/**
    \struct SENSOR_DATA 
    \brief Sensor Data

This structure holds the sensor data including measurements and module status.

*/
typedef struct
{
    MODULE_STATE        state;
    SENSOR_VALUE        value;

} SENSOR_DATA;

/* ---------------------------------------------------------------- FUNCTIONS */

/**
    \brief Sensor Initialization Routine.

This function initializes the Sensor module.

*/
void SENSOR_Initialize ( void );

/**
    \brief Sensor Tasks Function

This routine is the Sensor module tasks function. 

*/
void SENSOR_Tasks ( void );

/**

*/
void SENSOR_GetLastMerasurement ( SENSOR_VALUE *value );

#ifdef __cplusplus
}
#endif
#endif

/// \}
/* -------------------------------------------------------------------------- */
/*
    sensor.h

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
