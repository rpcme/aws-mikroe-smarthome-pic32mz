/*
    Rotary_click.h

-----------------------------------------------------------------------------

  This file is part of mikroSDK.
  
  Copyright (c) 2019, MikroElektonika - http://www.mikroe.com

  All rights reserved.

----------------------------------------------------------------------------- */

/**
    \file     Rotary_click.h
    \brief    Rotary Click Library
    \defgroup ROTARY
    \brief    Rotary Click Driver
    \{

- Global Library Prefix : ROTARY
- Version               : 1.1.0
- Date                  : Jan 2019.
- Developer             : Milos Vidojevic

---

- 1.0.0 : Module Created (Nenad Filipovic)
- 1.1.0 : RTOS support implemented (Milos Vidojevic)

*/
/* -------------------------------------------------------------------------- */

#include <stdint.h>

#ifndef _ROTARY_H_
#define _ROTARY_H_

#define __ROTARY_DRV_SPI__          ///< \macro __ROTARY_DRV_SPI__  \brief SPI driver selector
// #define __ROTARY_DRV_I2C__       ///< \macro __ROTARY_DRV_I2C__  \brief I2C driver selector
// #define __ROTARY_DRV_UART__      ///< \macro __ROTARY_DRV_UART__ \brief UART driver selector

#define T_ROTARY_P const uint8_t*   ///< \macro T_ROTARY_P \brief Driver Abstract type

/** 
    \enum T_rotary_direction
    \brief Rotation Direction
*/
typedef enum
{
    ROTARY_NO_MOVEMENT      = 0,
    ROTARY_CW_DIRECTION,
    ROTARY_CCW_DIRECTION,
    ROTARY_UNKNOWN_MOVEMENT

} T_rotary_direction;

#ifdef __cplusplus
extern "C"{
#endif

#ifdef   __ROTARY_DRV_SPI__
void rotary_spiDriverInit(T_ROTARY_P gpioObj, T_ROTARY_P spiObj);
#endif
#ifdef   __ROTARY_DRV_I2C__
void rotary_i2cDriverInit(T_ROTARYY_P gpioObj, T_ROTARYY_P i2cObj, uint8_t slave);
#endif
#ifdef   __ROTARY_DRV_UART__
void rotary_uartDriverInit(T_ROTARYY_P gpioObj, T_ROTARYY_P uartObj);
#endif

T_rotary_direction rotary_read_encoder ( void );

void rotary_write_ledring ( uint16_t map );

void rotary_enable ( void );

void rotary_disable ( void );

#ifdef __cplusplus
} // extern "C"
#endif
#endif
                                                                   
/// \} 
/* ----------------------------------------------------------------------------

    Rotary_click.h

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