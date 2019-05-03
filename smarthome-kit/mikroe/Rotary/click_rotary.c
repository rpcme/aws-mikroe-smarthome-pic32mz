/*
    Rotary_click.c

    | Global Library Prefix | **ROTARY**            |
    |:---------------------:|:---------------------:|
    | Version               | **1.1.0**             |
    | Date                  | **Jan 2019.**         |
    | Developer             | **Milos Vidojevic**   |
    
    ---
    
    **Version Info :**
    - **1.0.0** Module Created [Nenad Filipovic]
    - **1.1.0** MPLAB xc32 and RTOS support added [Milos Vidojevic]

-----------------------------------------------------------------------------

  This file is part of mikroSDK.

  Copyright (c) 2017, MikroElektonika - http://www.mikroe.com

  All rights reserved.

----------------------------------------------------------------------------- */

#include "click_rotary.h"
#include "click_rotary_hal.h"

/* ------------------------------------------------- PRIVATE TYPE DEFINITIONS */

enum 
{
    RISING_EDGE,
    FALLING_EDGE,
};

/* --------------------------------------------------------- PUBLIC FUNCTIONS */

#ifdef   __ROTARY_DRV_SPI__

void rotary_spiDriverInit(T_ROTARY_P gpioObj, T_ROTARY_P spiObj)
{
    hal_spiMap((T_HAL_P)DRV_SPI_Open(DRV_SPI_INDEX_1, DRV_IO_INTENT_READWRITE));

    hal_gpio_csSet(1);
    hal_gpio_rstSet(0);
}

#endif
#ifdef   __ROTARY_DRV_I2C__

void rotary_i2cDriverInit(T_ROTARY_P gpioObj, T_ROTARY_P i2cObj, uint8_t slave)
{
    _slaveAddress = slave;
    hal_i2cMap( (T_HAL_P)i2cObj );
    hal_gpioMap( (T_HAL_P)gpioObj );
}

#endif
#ifdef   __ROTARY_DRV_UART__

void rotary_uartDriverInit(T_ROTARY_P gpioObj, T_ROTARY_P uartObj)
{
    hal_uartMap( (T_HAL_P)uartObj );
    hal_gpioMap( (T_HAL_P)gpioObj );
}

#endif

/* ----------------------------------------------------------- IMPLEMENTATION */

T_rotary_direction rotary_read_encoder()
{
    static int a_state;
    static int b_state;
    static int a_edge;
    static int b_edge;

    //  Detected change on ENCA

    if (a_state != hal_gpio_pwmGet())
    {
        //  Take new value and set edge depends on it.

        ((a_state = hal_gpio_pwmGet()) == 1) ? 
                    (a_edge = RISING_EDGE) : (a_edge = FALLING_EDGE);

        //  Calculate movement depends on encoder's edges.

        switch (a_edge)
        {
        case RISING_EDGE:

            return (b_edge == RISING_EDGE) ? 
                    ROTARY_CCW_DIRECTION : ROTARY_CW_DIRECTION;

        break;
        case FALLING_EDGE:

            return (b_edge == RISING_EDGE) ? 
                    ROTARY_CW_DIRECTION : ROTARY_CCW_DIRECTION;

        break;
        default :

            /* 
                Unknown state, might be first movement detected or too fast 
                rotation of the stick in some cases.
            */
            return ROTARY_UNKNOWN_MOVEMENT;

        break;
        }
    }   

    //  Detected change on ENCB

    if (b_state != hal_gpio_anGet())
    {
        //  Take new value and set edge depends on it.

        ((b_state = hal_gpio_anGet()) == 1) ? 
                    (b_edge = RISING_EDGE) : (b_edge = FALLING_EDGE);

        //  Calculate movement depends on encoder's edges.

        switch (b_edge)
        {
        case RISING_EDGE:

            return (a_edge == RISING_EDGE) ? 
                    ROTARY_CW_DIRECTION : ROTARY_CCW_DIRECTION;

        break;
        case FALLING_EDGE:

            return (a_edge == RISING_EDGE) ? 
                    ROTARY_CCW_DIRECTION : ROTARY_CW_DIRECTION;

        break;
        default :

            /* 
                Unknown state, might be first movement detected or too fast 
                rotation of the stick in some cases.
            */
            return ROTARY_UNKNOWN_MOVEMENT;

        break;
        }
    }

    return ROTARY_NO_MOVEMENT;
}

void rotary_write_ledring(uint16_t value)
{
    uint8_t tmp[ 2 ];

    tmp[ 0 ] = value;
    tmp[ 1 ] = value >> 8;

    hal_gpio_csSet(0);
    hal_spiWrite(tmp, 2);
    hal_gpio_csSet(1);
}

void rotary_enable()
{
    hal_gpio_rstSet(1);
}

void rotary_disable()
{
    hal_gpio_rstSet(0);
}

/* -------------------------------------------------------------------------- */
/*
    Rotary_click.c

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