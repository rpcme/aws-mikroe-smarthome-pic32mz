/*
    Weather_click.c

    | Global Library Prefix | **WEATHER**           |
    |:---------------------:|:---------------------:|
    | Version               | **1.1.0**             |
    | Date                  | **Feb 2019.**         |
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

#include "click_weather.h"
#include "click_weather_hal.h"

/* ------------------------------------------------------------------- MACROS */



/* ---------------------------------------------------------------- VARIABLES */

static uint32_t adc_t;
static uint32_t adc_p;
static uint32_t adc_h;
static uint32_t t_fine;

static uint16_t dig_T1;
static int16_t  dig_T2;
static int16_t  dig_T3;

static uint8_t  dig_H1;
static int16_t  dig_H2;
static uint8_t  dig_H3;
static int16_t  dig_H4;
static int16_t  dig_H5;
static int8_t   dig_H6;

static uint16_t dig_P1;
static int16_t  dig_P2;
static int16_t  dig_P3;
static int16_t  dig_P4;
static int16_t  dig_P5;
static int16_t  dig_P6;
static int16_t  dig_P7;
static int16_t  dig_P8;
static int16_t  dig_P9;

/* -------------------------------------------- PRIVATE FUNCTION DECLARATIONS */

//  Read single register

static void read_register(uint8_t reg, uint8_t *buf, uint8_t len);

//  Write register

static void write_register(uint8_t reg, uint8_t *buf, uint8_t len);

//  Read 16bit register

static uint16_t read_16bit_reg(uint8_t reg);

//  Compensate temperature function

static int32_t compensate_T();

//  Compensate humidity function

static uint32_t compensate_H();

//  Compensate pressure function

static uint32_t compensate_P();

/* --------------------------------------------------------- PUBLIC FUNCTIONS */

#define _WEATHER_SPI 0
#define _WEATHER_I2C 1

static uint8_t dev_comm;
static uint8_t _slaveAddress;

#ifdef   __WEATHER_DRV_SPI__

void weather_spiDriverInit(T_WEATHER_P gpioObj, T_WEATHER_P spiObj)
{
    hal_spiMap((T_HAL_P)DRV_SPI_Open(DRV_SPI_INDEX_1, DRV_IO_INTENT_READWRITE));

    dev_comm = _WEATHER_SPI;

    hal_gpio_csSet(1);
}

#endif
#ifdef   __WEATHER_DRV_I2C__

void weather_i2cDriverInit(T_WEATHER_P gpioObj, T_WEATHER_P i2cObj, uint8_t slave)
{
    _slaveAddress = slave;

#if 0
    hal_i2cMap( (T_HAL_P)i2cObj );
    hal_gpioMap( (T_HAL_P)gpioObj );
#endif
    
    dev_comm = _WEATHER_I2C;
}

#endif
#ifdef   __WEATHER_DRV_UART__

void weather_uartDriverInit(T_WEATHER_P gpioObj, T_WEATHER_P uartObj)
{
    hal_uartMap( (T_HAL_P)uartObj );
    hal_gpioMap( (T_HAL_P)gpioObj );

    // ... power ON
    // ... configure CHIP
}

#endif

/* ----------------------------------------------------------- IMPLEMENTATION */

void weather_writeData(uint8_t regAddress, uint8_t writeData)
{
    write_register(regAddress, &writeData, 1);
}

uint8_t weather_readData(uint8_t regAddress)
{
    uint8_t res;

    read_register(regAddress, &res, 1);

    return res;
}

void weather_readMeasurements()
{
    uint8_t rBuffer[ 8 ];

    read_register(_WEATHER_PRESSURE_MSB_REG, rBuffer, 8);

    adc_h  = (uint32_t)rBuffer[_WEATHER_DATA_FRAME_HUMIDITY_LSB_BYTE];
    adc_h |= (uint32_t)rBuffer[_WEATHER_DATA_FRAME_HUMIDITY_MSB_BYTE] << 8;

    adc_t  = (uint32_t)rBuffer[_WEATHER_DATA_FRAME_TEMPERATURE_XLSB_BYTE] >> 4;
    adc_t |= (uint32_t)rBuffer[_WEATHER_DATA_FRAME_TEMPERATURE_LSB_BYTE] << 4;
    adc_t |= (uint32_t)rBuffer[_WEATHER_DATA_FRAME_TEMPERATURE_MSB_BYTE] << 12;

    adc_p  = (uint32_t)rBuffer[_WEATHER_DATA_FRAME_PRESSURE_XLSB_BYTE] >> 4;
    adc_p |= (uint32_t)rBuffer[_WEATHER_DATA_FRAME_PRESSURE_LSB_BYTE] << 4;
    adc_p |= (uint32_t)rBuffer[_WEATHER_DATA_FRAME_PRESSURE_MSB_BYTE] << 12;
}

void weather_readCalibrationParams()
{
    dig_T1 = read_16bit_reg(_WEATHER_TEMPERATURE_CALIB_DIG_T1_MSB_REG);
    dig_T2 = read_16bit_reg(_WEATHER_TEMPERATURE_CALIB_DIG_T2_MSB_REG);
    dig_T3 = read_16bit_reg(_WEATHER_TEMPERATURE_CALIB_DIG_T3_MSB_REG);

    dig_P1 = read_16bit_reg(_WEATHER_PRESSURE_CALIB_DIG_P1_MSB_REG);
    dig_P2 = read_16bit_reg(_WEATHER_PRESSURE_CALIB_DIG_P2_MSB_REG);
    dig_P3 = read_16bit_reg(_WEATHER_PRESSURE_CALIB_DIG_P3_MSB_REG);
    dig_P4 = read_16bit_reg(_WEATHER_PRESSURE_CALIB_DIG_P4_MSB_REG);
    dig_P5 = read_16bit_reg(_WEATHER_PRESSURE_CALIB_DIG_P5_MSB_REG);
    dig_P6 = read_16bit_reg(_WEATHER_PRESSURE_CALIB_DIG_P6_MSB_REG);
    dig_P7 = read_16bit_reg(_WEATHER_PRESSURE_CALIB_DIG_P7_MSB_REG);
    dig_P8 = read_16bit_reg(_WEATHER_PRESSURE_CALIB_DIG_P8_MSB_REG);
    dig_P9 = read_16bit_reg(_WEATHER_PRESSURE_CALIB_DIG_P9_MSB_REG);

    dig_H1 = weather_readData(_WEATHER_HUMIDITY_CALIB_DIG_H1_REG);
    dig_H2 = read_16bit_reg(_WEATHER_HUMIDITY_CALIB_DIG_H2_MSB_REG);
    dig_H3 = weather_readData(_WEATHER_HUMIDITY_CALIB_DIG_H3_REG);
    dig_H4 = weather_readData(_WEATHER_HUMIDITY_CALIB_DIG_H4_MSB_REG);
    dig_H4 <<= 4;
    dig_H4 |= (weather_readData(_WEATHER_HUMIDITY_CALIB_DIG_H4_LSB_REG) & 0x0F);
    dig_H5 = weather_readData(_WEATHER_HUMIDITY_CALIB_DIG_H5_MSB_REG);
    dig_H5 <<= 4;
    dig_H5 |= (weather_readData(_WEATHER_HUMIDITY_CALIB_DIG_H5_MSB_REG) >> 4);
    dig_H6 = weather_readData(_WEATHER_HUMIDITY_CALIB_DIG_H6_REG);
}

void weather_setOversamplingPressure( uint8_t value )
{
    uint8_t ctrlm;
    
    ctrlm = weather_getCtrlMeasurement();
    
    ctrlm &= ~_WEATHER_CTRL_MEAS_REG_OVERSAMP_PRESSURE_MSK;
    
    ctrlm |= value << _WEATHER_CTRL_MEAS_REG_OVERSAMP_PRESSURE_POS;

    weather_writeData( _WEATHER_CTRL_MEAS_REG, ctrlm );
}

void weather_setOversamplingTemperature( uint8_t value )
{
    uint8_t ctrlm;
    
    ctrlm = weather_getCtrlMeasurement();
    
    ctrlm &= ~_WEATHER_CTRL_MEAS_REG_OVERSAMP_TEMPERATURE_MSK;
    
    ctrlm |= value << _WEATHER_CTRL_MEAS_REG_OVERSAMP_TEMPERATURE_POS;

    weather_writeData( _WEATHER_CTRL_MEAS_REG, ctrlm );
}

void weather_setOversamplingHumidity( uint8_t value )
{
    weather_writeData( _WEATHER_CTRL_HUMIDITY_REG, value );
}

void weather_setOversamplingMode( uint8_t value )
{
    uint8_t ctrlm;
    
    ctrlm = weather_getCtrlMeasurement();
    
    ctrlm |= value;

    weather_writeData( _WEATHER_CTRL_MEAS_REG, ctrlm );
}

void weather_setFilterCoefficient( uint8_t value )
{
    uint8_t cfgv;
    
    cfgv = weather_getConfig();
    
    cfgv &= ~_WEATHER_CONFIG_REG_FILTER_MSK;
    
    cfgv |= value << _WEATHER_CONFIG_REG_FILTER_POS;
    
    weather_writeData( _WEATHER_CONFIG_REG, cfgv );
}

void weather_setStandbyTime( uint8_t value )
{
    uint8_t cfgv;
    
    cfgv = weather_getConfig();
    
    cfgv &= ~_WEATHER_CONFIG_REG_TSB_MSK;
    
    cfgv |= value << _WEATHER_CONFIG_REG_TSB_POS;
    
    weather_writeData( _WEATHER_CONFIG_REG, cfgv );
}

uint8_t weather_isMeasuring()
{
    uint8_t output;
    
    output = weather_getStatus();
    
    output &= _WEATHER_STAT_REG_MEASURING_MSK;
    
    return output;
}

float weather_getTemperature()
{
    float result;
    int32_t tempVal;

    weather_readMeasurements();

    weather_readCalibrationParams();

    tempVal =  compensate_T();

    result = ((float)tempVal) / 100.0;

    return result;
}

float weather_getHumidity()
{
    uint32_t humVal;

    weather_readMeasurements();
    weather_readCalibrationParams();

    humVal = compensate_H();
    
    return (((float) humVal) / 1024.0);
}

float weather_getPressure()
{
    float result;
    uint32_t pressVal;

    weather_readMeasurements();

    weather_readCalibrationParams();

    pressVal = compensate_P();

    result =( ( float ) pressVal ) / 100.0;

    return result;
}

void weather_getWeather(float *temperature, float *humidity, float *pressure)
{
    int32_t tempVal;
    uint32_t humVal;
    uint32_t pressVal;

    weather_readMeasurements();
    
    tempVal  = compensate_T();
    humVal   = compensate_H();
    pressVal = compensate_P();
    
    *temperature = ((float)tempVal) / 100.0;
    *humidity = ((float)humVal) / 1024.0;
    *pressure = ((float)pressVal) / 100.0;
}

uint8_t weather_getID()
{
    uint8_t idVal;

    idVal = weather_readData( _WEATHER_CHIP_ID_REG );
    
    return idVal;
}

void weather_softReset()
{
    weather_writeData( _WEATHER_RST_REG, _WEATHER_SOFT_RESET );
}

uint8_t weather_getStatus()
{
    uint8_t temp;

    temp = weather_readData( _WEATHER_STAT_REG );
    
    return temp;
}

uint8_t weather_getCtrlMeasurement()
{
    uint8_t ctrlMeasure;

    ctrlMeasure = weather_readData( _WEATHER_CTRL_MEAS_REG );
    
    return ctrlMeasure;
}

uint8_t weather_getCtrlHumidity()
{
    uint8_t ctrlHun;

    ctrlHun = weather_readData( _WEATHER_CTRL_HUMIDITY_REG );

    return ctrlHun;
}

uint8_t weather_getConfig()
{
    uint8_t cnfg;
    
    cnfg = weather_readData( _WEATHER_CONFIG_REG );
    
    return cnfg;
}

/* --------------------------------------------- PRIVATE FUNCTION DEFINITIONS */

static void write_register(uint8_t reg, uint8_t *buf, uint8_t len)
{
    uint8_t c;
    uint8_t tmp[256];

    switch (dev_comm)
    {
    case _WEATHER_I2C :
    {
        tmp[0] = reg;
        
        memcpy(&tmp[1], buf, len);
#if 0
        hal_i2cStart();
        hal_i2cWrite( _slaveAddress, tmp, len + 1, END_MODE_STOP );
#endif
        
    }
    break;
    case _WEATHER_SPI :
    {
        uint8_t c;

        hal_gpio_csSet(0);

        for (c = 0; c < len; ++c)
        {
            tmp[0] = (reg++ & 0x7F);
            tmp[1] = buf[c];
            hal_spiWrite(tmp, 2);
        }

        hal_gpio_csSet(1);
    }
    break;
    default :

        //  Unknown communication type.

    break;
    }
}

static void read_register(uint8_t reg, uint8_t *buf, uint8_t len)
{
    switch (dev_comm)
    {
    case _WEATHER_I2C :
    {
#if 0
        hal_i2cStart();
        hal_i2cWrite(_slaveAddress, &reg, 1, END_MODE_RESTART);
        hal_i2cRead(_slaveAddress, buf, len, END_MODE_STOP);
#endif
    }   
    break;
    case _WEATHER_SPI :

        hal_gpio_csSet(0);
        hal_spiWrite(&reg, 1);
        hal_spiRead(buf, len);
        hal_gpio_csSet(1);

    break;
    default :

        //  Unknown communication type.

    break;
    }
}

static uint16_t read_16bit_reg(uint8_t reg)
{
    uint16_t res;
    uint8_t  tmp[2];

    read_register(reg, tmp, 2);

    res  = tmp[0];
    res <<= 8;
    res |= tmp[1];

    return res;
}

static int32_t compensate_T()
{
    int32_t temp1;
    int32_t temp2;
    int32_t valT;

    temp1 = ((((adc_t >> 3) - ((int32_t)dig_T1 << 1))) * ((int32_t)dig_T2)) >> 11;
    temp2 = (((((adc_t >> 4) - ((int32_t)dig_T1)) * ((adc_t >> 4) - ((int32_t)dig_T1))) >> 12) * ((int32_t)dig_T3)) >> 14;

    t_fine = temp1 + temp2;
    valT = (t_fine * 5 + 128) >> 8;
    
    return valT;
}

static uint32_t compensate_H()
{
    int32_t valHum;
    
    valHum = (t_fine - ((int32_t)76800));
    
    valHum = (((((adc_h << 14) - (((int32_t)dig_H4) << 20) - (((int32_t)dig_H5) * valHum)) +
             ((int32_t)16384)) >> 15) * (((((((valHum * ((int32_t)dig_H6)) >> 10) * 
             (((valHum * ((int32_t )dig_H3)) >> 11) + ((int32_t)32768))) >> 10) + 
             ((int32_t)2097152)) * ((int32_t)dig_H2) + 8192) >> 14));
             
    valHum = (valHum - (((((valHum >> 15) * (valHum >> 15)) >> 7) * ((int32_t)dig_H1)) >> 4));
    valHum = (valHum < 0 ? 0 : valHum);
    valHum = (valHum > 419430400 ? 419430400 : valHum);
    
    return (uint32_t)(valHum >> 12);
}

static uint32_t compensate_P()
{
    int32_t press1;
    int32_t  press2;
    uint32_t valP;

    press1 = (((int32_t)t_fine) >> 1) - (int32_t)64000;
    press2 = (((press1 >> 2) * (press1 >> 2)) >> 11) * ((int32_t)dig_P6);
    press2 = press2 + ((press1 * ((int32_t)dig_P5)) << 1);
    press2 = (press2 >> 2) + (((int32_t)dig_P4) << 16);
    press1 = (((dig_P3 * (((press1 >> 2) * (press1 >> 2)) >> 13)) >> 3) + ((((int32_t)dig_P2) * press1) >> 1)) >> 18;
    press1 = ((((32768 + press1)) * ((int32_t)dig_P1)) >> 15);
    
    if (press1 == 0)
    {
        return 0;
    }
    
    valP = (((uint32_t)(((int32_t)1048576) - adc_p) - (press2 >> 12))) * 3125;
    
    if (valP < 0x80000000)
    {
        valP = (valP << 1) / ((uint32_t)press1);
    }
    else
    {
        valP = ( valP / ( uint32_t )press1 ) * 2;
    }
    
    press1 = (((int32_t)dig_P9) * ((int32_t)(((valP >> 3) * (valP >> 3)) >> 13))) >> 12;
    press2 = (((int32_t)(valP >> 2)) * ((int32_t)dig_P8)) >> 13;
    valP = (uint32_t)((int32_t)valP + ((press1 + press2 + dig_P7) >> 4));
    
    return valP;
}

/* -------------------------------------------------------------------------- */
/*
    Weather_click.c

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