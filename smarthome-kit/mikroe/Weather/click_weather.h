/*
    Weather_click.h

-----------------------------------------------------------------------------

  This file is part of mikroSDK.
  
  Copyright (c) 2017, MikroElektonika - http://www.mikroe.com

  All rights reserved.

----------------------------------------------------------------------------- */

/**
    \file     Weather_click.h
    \brief    Weather Click
    \defgroup WEATHER
    \brief    Weather Click Driver
    \{

| Global Library Prefix | **WEATHER**           |
|:---------------------:|:---------------------:|
| Version               | **1.1.0**             |
| Date                  | **Feb 2019.**         |
| Developer             | **Milos Vidojevic**   |

---

**Version Info :**
- **1.0.0** Module Created [Nenad Filipovic]
- **1.1.0** MPLAB xc32 and RTOS support added [Milos Vidojevic]

---

*/
/* -------------------------------------------------------------------------- */

#include <xc.h>

#ifndef _WEATHER_H_
#define _WEATHER_H_

/** 
 * @macro T_WEATHER_P
 * @brief Driver Abstract type 
 */
#define T_WEATHER_P    const uint8_t*

#define __WEATHER_DRV_SPI__     ///< \macro __WEATHER_DRV_SPI__  \brief SPI driver selector.
// #define __WEATHER_DRV_I2C__     ///< \macro __WEATHER_DRV_I2C__  \brief I2C driver selector.
// #define __WEATHER_DRV_UART__ ///< \macro __WEATHER_DRV_UART__ \brief UART driver selector.

#define _WEATHER_CHIP_ID                                                    0x60

#define _WEATHER_SHIFT_BIT_POSITION_BY_01_BIT                               1
#define _WEATHER_SHIFT_BIT_POSITION_BY_02_BITS                              2
#define _WEATHER_SHIFT_BIT_POSITION_BY_03_BITS                              3
#define _WEATHER_SHIFT_BIT_POSITION_BY_04_BITS                              4
#define _WEATHER_SHIFT_BIT_POSITION_BY_07_BITS                              7
#define _WEATHER_SHIFT_BIT_POSITION_BY_08_BITS                              8
#define _WEATHER_SHIFT_BIT_POSITION_BY_10_BITS                              10
#define _WEATHER_SHIFT_BIT_POSITION_BY_11_BITS                              11
#define _WEATHER_SHIFT_BIT_POSITION_BY_12_BITS                              12
#define _WEATHER_SHIFT_BIT_POSITION_BY_13_BITS                              13
#define _WEATHER_SHIFT_BIT_POSITION_BY_14_BITS                              14
#define _WEATHER_SHIFT_BIT_POSITION_BY_15_BITS                              15
#define _WEATHER_SHIFT_BIT_POSITION_BY_16_BITS                              16
#define _WEATHER_SHIFT_BIT_POSITION_BY_17_BITS                              17
#define _WEATHER_SHIFT_BIT_POSITION_BY_18_BITS                              18
#define _WEATHER_SHIFT_BIT_POSITION_BY_19_BITS                              19
#define _WEATHER_SHIFT_BIT_POSITION_BY_20_BITS                              20
#define _WEATHER_SHIFT_BIT_POSITION_BY_25_BITS                              25
#define _WEATHER_SHIFT_BIT_POSITION_BY_31_BITS                              31
#define _WEATHER_SHIFT_BIT_POSITION_BY_33_BITS                              33
#define _WEATHER_SHIFT_BIT_POSITION_BY_35_BITS                              35
#define _WEATHER_SHIFT_BIT_POSITION_BY_47_BITS                              47

/* numeric definitions */

#define _WEATHER_PRESSURE_TEMPERATURE_CALIB_DATA_LENGTH                     26
#define _WEATHER_HUMIDITY_CALIB_DATA_LENGTH                                 7
#define _WEATHER_GEN_READ_WRITE_DATA_LENGTH                                 1
#define _WEATHER_HUMIDITY_DATA_LENGTH                                       2
#define _WEATHER_TEMPERATURE_DATA_LENGTH                                    3
#define _WEATHER_PRESSURE_DATA_LENGTH                                       3
#define _WEATHER_ALL_DATA_FRAME_LENGTH                                      8
#define _WEATHER_INIT_VALUE                                                 0
#define _WEATHER_INVALID_DATA                                               0

/* ERROR CODE DEFINITIONS  */
#define SUCCESS                                                   ((char)0)
#define E__WEATHER_NULL_PTR                                       ((short)-127)
#define E__WEATHER_COMM_RES                                       ((short)-1)
#define E__WEATHER_OUT_OF_RANGE                                   ((short)-2)
#define ERROR                                                     ((char)-1)

/* I2C ADDRESS DEFINITIONS  */
#define _WEATHER_I2C_ADDRESS_0                                              0x76
#define _WEATHER_I2C_ADDRESS_1                                              0x77

/* POWER MODE DEFINITIONS  */
/* Sensor Specific constants */
#define _WEATHER_SLEEP_MODE                                                 0x00
#define _WEATHER_FORCED_MODE                                                0x01
#define _WEATHER_NORMAL_MODE                                                0x03
#define _WEATHER_SOFT_RESET                                                 0xB6

/* STANDBY DEFINITIONS  */
#define _WEATHER_STANDBY_TIME_1_MS                                          0x00
#define _WEATHER_STANDBY_TIME_63_MS                                         0x01
#define _WEATHER_STANDBY_TIME_125_MS                                        0x02
#define _WEATHER_STANDBY_TIME_250_MS                                        0x03
#define _WEATHER_STANDBY_TIME_500_MS                                        0x04
#define _WEATHER_STANDBY_TIME_1000_MS                                       0x05
#define _WEATHER_STANDBY_TIME_10_MS                                         0x06
#define _WEATHER_STANDBY_TIME_20_MS                                         0x07

/* OVER SAMPLING DEFINITIONS  */
#define _WEATHER_OVERSAMP_SKIPPED                                           0x00
#define _WEATHER_OVERSAMP_1X                                                0x01
#define _WEATHER_OVERSAMP_2X                                                0x02
#define _WEATHER_OVERSAMP_4X                                                0x03
#define _WEATHER_OVERSAMP_8X                                                0x04
#define _WEATHER_OVERSAMP_16X                                               0x05

/* WORK MODE DEFINITIONS  */
#define _WEATHER_ULTRALOWPOWER_MODE                                         0x00
#define _WEATHER_LOWPOWER_MODE                                              0x01
#define _WEATHER_STANDARDRESOLUTION_MODE                                    0x02
#define _WEATHER_HIGHRESOLUTION_MODE                                        0x03
#define _WEATHER_ULTRAHIGHRESOLUTION_MODE                                   0x04

#define _WEATHER_ULTRALOWPOWER_OSRS_P                                       0x01
#define _WEATHER_ULTRALOWPOWER_OSRS_T                                       0x01
#define _WEATHER_LOWPOWER_OSRS_P                                            0x02
#define _WEATHER_LOWPOWER_OSRS_T                                            0x01
#define _WEATHER_STANDARDRESOLUTION_OSRS_P                                  0x03
#define _WEATHER_STANDARDRESOLUTION_OSRS_T                                  0x01
#define _WEATHER_HIGHRESOLUTION_OSRS_P                                      0x04
#define _WEATHER_HIGHRESOLUTION_OSRS_T                                      0x01
#define _WEATHER_ULTRAHIGHRESOLUTION_OSRS_P                                 0x05
#define _WEATHER_ULTRAHIGHRESOLUTION_OSRS_T                                 0x02
#define _WEATHER_STANDARD_OVERSAMP_HUMIDITY                                 0x01

/* FILTER DEFINITIONS  */
#define _WEATHER_FILTER_COEFF_OFF                                           0x00
#define _WEATHER_FILTER_COEFF_2                                             0x01
#define _WEATHER_FILTER_COEFF_4                                             0x02
#define _WEATHER_FILTER_COEFF_8                                             0x03
#define _WEATHER_FILTER_COEFF_16                                            0x04

/* DELAY DEFINITIONS  */
/* 20/16 = 1.25 ms */
#define _WEATHER_T_INIT_MAX                                                 20
/* 37/16 = 2.3125 ms*/
#define _WEATHER_T_MEASURE_PER_OSRS_MAX                                     37
/* 10/16 = 0.625 ms */
#define _WEATHER_T_SETUP_PRESSURE_MAX                                       10
/* 10/16 = 0.625 ms */
#define _WEATHER_T_SETUP_HUMIDITY_MAX                                       10

/* DEFINITIONS FOR ARRAY SIZE OF DATA   */
#define _WEATHER_HUMIDITY_DATA_SIZE                                         2
#define _WEATHER_TEMPERATURE_DATA_SIZE                                      3
#define _WEATHER_PRESSURE_DATA_SIZE                                         3
#define _WEATHER_DATA_FRAME_SIZE                                            8

/* data frames includes temperature, pressure and humidity */
#define _WEATHER_CALIB_DATA_SIZE                                            26
#define _WEATHER_TEMPERATURE_MSB_DATA                                       0
#define _WEATHER_TEMPERATURE_LSB_DATA                                       1
#define _WEATHER_TEMPERATURE_XLSB_DATA                                      2
#define _WEATHER_PRESSURE_MSB_DATA                                          0
#define _WEATHER_PRESSURE_LSB_DATA                                          1
#define _WEATHER_PRESSURE_XLSB_DATA                                         2
#define _WEATHER_HUMIDITY_MSB_DATA                                          0
#define _WEATHER_HUMIDITY_LSB_DATA                                          1

#define _WEATHER_DATA_FRAME_PRESSURE_MSB_BYTE                               0
#define _WEATHER_DATA_FRAME_PRESSURE_LSB_BYTE                               1
#define _WEATHER_DATA_FRAME_PRESSURE_XLSB_BYTE                              2
#define _WEATHER_DATA_FRAME_TEMPERATURE_MSB_BYTE                            3
#define _WEATHER_DATA_FRAME_TEMPERATURE_LSB_BYTE                            4
#define _WEATHER_DATA_FRAME_TEMPERATURE_XLSB_BYTE                           5
#define _WEATHER_DATA_FRAME_HUMIDITY_MSB_BYTE                               6
#define _WEATHER_DATA_FRAME_HUMIDITY_LSB_BYTE                               7

/*  ARRAY PARAMETER FOR CALIBRATION     */
#define _WEATHER_TEMPERATURE_CALIB_DIG_T1_LSB                               0
#define _WEATHER_TEMPERATURE_CALIB_DIG_T1_MSB                               1
#define _WEATHER_TEMPERATURE_CALIB_DIG_T2_LSB                               2
#define _WEATHER_TEMPERATURE_CALIB_DIG_T2_MSB                               3
#define _WEATHER_TEMPERATURE_CALIB_DIG_T3_LSB                               4
#define _WEATHER_TEMPERATURE_CALIB_DIG_T3_MSB                               5
#define _WEATHER_PRESSURE_CALIB_DIG_P1_LSB                                  6
#define _WEATHER_PRESSURE_CALIB_DIG_P1_MSB                                  7
#define _WEATHER_PRESSURE_CALIB_DIG_P2_LSB                                  8
#define _WEATHER_PRESSURE_CALIB_DIG_P2_MSB                                  9
#define _WEATHER_PRESSURE_CALIB_DIG_P3_LSB                                  10
#define _WEATHER_PRESSURE_CALIB_DIG_P3_MSB                                  11
#define _WEATHER_PRESSURE_CALIB_DIG_P4_LSB                                  12
#define _WEATHER_PRESSURE_CALIB_DIG_P4_MSB                                  13
#define _WEATHER_PRESSURE_CALIB_DIG_P5_LSB                                  14
#define _WEATHER_PRESSURE_CALIB_DIG_P5_MSB                                  15
#define _WEATHER_PRESSURE_CALIB_DIG_P6_LSB                                  16
#define _WEATHER_PRESSURE_CALIB_DIG_P6_MSB                                  17
#define _WEATHER_PRESSURE_CALIB_DIG_P7_LSB                                  18
#define _WEATHER_PRESSURE_CALIB_DIG_P7_MSB                                  19
#define _WEATHER_PRESSURE_CALIB_DIG_P8_LSB                                  20
#define _WEATHER_PRESSURE_CALIB_DIG_P8_MSB                                  21
#define _WEATHER_PRESSURE_CALIB_DIG_P9_LSB                                  22
#define _WEATHER_PRESSURE_CALIB_DIG_P9_MSB                                  23
#define _WEATHER_HUMIDITY_CALIB_DIG_H1                                      25
#define _WEATHER_HUMIDITY_CALIB_DIG_H2_LSB                                  0
#define _WEATHER_HUMIDITY_CALIB_DIG_H2_MSB                                  1
#define _WEATHER_HUMIDITY_CALIB_DIG_H3                                      2
#define _WEATHER_HUMIDITY_CALIB_DIG_H4_MSB                                  3
#define _WEATHER_HUMIDITY_CALIB_DIG_H4_LSB                                  4
#define _WEATHER_HUMIDITY_CALIB_DIG_H5_MSB                                  5
#define _WEATHER_HUMIDITY_CALIB_DIG_H6                                      6
#define _WEATHER_MASK_DIG_H4                                                0x0F

/* CALIBRATION REGISTER ADDRESS DEFINITIONS  */
/*calibration parameters */
#define _WEATHER_TEMPERATURE_CALIB_DIG_T1_LSB_REG                           0x88
#define _WEATHER_TEMPERATURE_CALIB_DIG_T1_MSB_REG                           0x89
#define _WEATHER_TEMPERATURE_CALIB_DIG_T2_LSB_REG                           0x8A
#define _WEATHER_TEMPERATURE_CALIB_DIG_T2_MSB_REG                           0x8B
#define _WEATHER_TEMPERATURE_CALIB_DIG_T3_LSB_REG                           0x8C
#define _WEATHER_TEMPERATURE_CALIB_DIG_T3_MSB_REG                           0x8D
#define _WEATHER_PRESSURE_CALIB_DIG_P1_LSB_REG                              0x8E
#define _WEATHER_PRESSURE_CALIB_DIG_P1_MSB_REG                              0x8F
#define _WEATHER_PRESSURE_CALIB_DIG_P2_LSB_REG                              0x90
#define _WEATHER_PRESSURE_CALIB_DIG_P2_MSB_REG                              0x91
#define _WEATHER_PRESSURE_CALIB_DIG_P3_LSB_REG                              0x92
#define _WEATHER_PRESSURE_CALIB_DIG_P3_MSB_REG                              0x93
#define _WEATHER_PRESSURE_CALIB_DIG_P4_LSB_REG                              0x94
#define _WEATHER_PRESSURE_CALIB_DIG_P4_MSB_REG                              0x95
#define _WEATHER_PRESSURE_CALIB_DIG_P5_LSB_REG                              0x96
#define _WEATHER_PRESSURE_CALIB_DIG_P5_MSB_REG                              0x97
#define _WEATHER_PRESSURE_CALIB_DIG_P6_LSB_REG                              0x98
#define _WEATHER_PRESSURE_CALIB_DIG_P6_MSB_REG                              0x99
#define _WEATHER_PRESSURE_CALIB_DIG_P7_LSB_REG                              0x9A
#define _WEATHER_PRESSURE_CALIB_DIG_P7_MSB_REG                              0x9B
#define _WEATHER_PRESSURE_CALIB_DIG_P8_LSB_REG                              0x9C
#define _WEATHER_PRESSURE_CALIB_DIG_P8_MSB_REG                              0x9D
#define _WEATHER_PRESSURE_CALIB_DIG_P9_LSB_REG                              0x9E
#define _WEATHER_PRESSURE_CALIB_DIG_P9_MSB_REG                              0x9F
#define _WEATHER_HUMIDITY_CALIB_DIG_H1_REG                                  0xA1
#define _WEATHER_HUMIDITY_CALIB_DIG_H2_LSB_REG                              0xE1
#define _WEATHER_HUMIDITY_CALIB_DIG_H2_MSB_REG                              0xE2
#define _WEATHER_HUMIDITY_CALIB_DIG_H3_REG                                  0xE3
#define _WEATHER_HUMIDITY_CALIB_DIG_H4_MSB_REG                              0xE4
#define _WEATHER_HUMIDITY_CALIB_DIG_H4_LSB_REG                              0xE5
#define _WEATHER_HUMIDITY_CALIB_DIG_H5_MSB_REG                              0xE6
#define _WEATHER_HUMIDITY_CALIB_DIG_H6_REG                                  0xE7

/* REGISTER ADDRESS DEFINITIONS  */
/*Chip ID Register */
#define _WEATHER_CHIP_ID_REG                                                0xD0
/*Softreset Register */
#define _WEATHER_RST_REG                                                    0xE0
/*Status Register */
#define _WEATHER_STAT_REG                                                   0xF3
/*Ctrl Measure Register */
#define _WEATHER_CTRL_MEAS_REG                                              0xF4
/*Ctrl Humidity Register*/
#define _WEATHER_CTRL_HUMIDITY_REG                                          0xF2
/*Configuration Register */
#define _WEATHER_CONFIG_REG                                                 0xF5
/*Pressure MSB Register */
#define _WEATHER_PRESSURE_MSB_REG                                           0xF7
/*Pressure LSB Register */
#define _WEATHER_PRESSURE_LSB_REG                                           0xF8
/*Pressure XLSB Register */
#define _WEATHER_PRESSURE_XLSB_REG                                          0xF9
/*Temperature MSB Reg */
#define _WEATHER_TEMPERATURE_MSB_REG                                        0xFA
/*Temperature LSB Reg */
#define _WEATHER_TEMPERATURE_LSB_REG                                        0xFB
/*Temperature XLSB Reg */
#define _WEATHER_TEMPERATURE_XLSB_REG                                       0xFC
/*Humidity MSB Reg */
#define _WEATHER_HUMIDITY_MSB_REG                                           0xFD
/*Humidity LSB Reg */
#define _WEATHER_HUMIDITY_LSB_REG                                           0xFE

/* BIT MASK, LENGTH AND POSITION DEFINITIONS  */
/* Status Register */
#define _WEATHER_STAT_REG_MEASURING_POS                                     3
#define _WEATHER_STAT_REG_MEASURING_MSK                                     0x08
#define _WEATHER_STAT_REG_MEASURING_LEN                                     1
#define _WEATHER_STAT_REG_MEASURING_REG                                     0xF3
#define _WEATHER_STAT_REG_IM_UPDATE_POS                                     0
#define _WEATHER_STAT_REG_IM_UPDATE_MSK                                     0x01
#define _WEATHER_STAT_REG_IM_UPDATE_LEN                                     1
#define _WEATHER_STAT_REG_IM_UPDATE_REG                                     0xF3

/*  BIT MASK, LENGTH AND POSITION DEFINITIONS FOR TEMPERATURE OVERSAMPLING  */
/* Control Measurement Register */
#define _WEATHER_CTRL_MEAS_REG_OVERSAMP_TEMPERATURE_POS     5
#define _WEATHER_CTRL_MEAS_REG_OVERSAMP_TEMPERATURE_MSK     0xE0
#define _WEATHER_CTRL_MEAS_REG_OVERSAMP_TEMPERATURE_LEN     3
#define _WEATHER_CTRL_MEAS_REG_OVERSAMP_TEMPERATURE_REG     _WEATHER_CTRL_MEAS_REG

/*  BIT MASK, LENGTH AND POSITION DEFINITIONS FOR PRESSURE OVERSAMPLING  */
#define _WEATHER_CTRL_MEAS_REG_OVERSAMP_PRESSURE_POS        2
#define _WEATHER_CTRL_MEAS_REG_OVERSAMP_PRESSURE_MSK        0x1C
#define _WEATHER_CTRL_MEAS_REG_OVERSAMP_PRESSURE_LEN        3
#define _WEATHER_CTRL_MEAS_REG_OVERSAMP_PRESSURE_REG        _WEATHER_CTRL_MEAS_REG

/* BIT MASK, LENGTH AND POSITION DEFINITIONS FOR POWER MODE  */
#define _WEATHER_CTRL_MEAS_REG_POWER_MODE_POS               0
#define _WEATHER_CTRL_MEAS_REG_POWER_MODE_MSK               0x03
#define _WEATHER_CTRL_MEAS_REG_POWER_MODE_LEN               2
#define _WEATHER_CTRL_MEAS_REG_POWER_MODE_REG               _WEATHER_CTRL_MEAS_REG

/*  BIT MASK, LENGTH AND POSITION DEFINITIONS FOR HUMIDITY OVERSAMPLING  */
#define _WEATHER_CTRL_HUMIDITY_REG_OVERSAMP_HUMIDITY_POS    0
#define _WEATHER_CTRL_HUMIDITY_REG_OVERSAMP_HUMIDITY_MSK    0x07
#define _WEATHER_CTRL_HUMIDITY_REG_OVERSAMP_HUMIDITY_LEN    3
#define _WEATHER_CTRL_HUMIDITY_REG_OVERSAMP_HUMIDITY_REG    _WEATHER_CTRL_HUMIDITY_REG

/* BIT MASK, LENGTH AND POSITION DEFINITIONS FOR STANDBY TIME  */
/* Configuration Register */
#define _WEATHER_CONFIG_REG_TSB_POS             5
#define _WEATHER_CONFIG_REG_TSB_MSK             0xE0
#define _WEATHER_CONFIG_REG_TSB_LEN             3
#define _WEATHER_CONFIG_REG_TSB_REG             _WEATHER_CONFIG_REG

/* BIT MASK, LENGTH AND POSITION DEFINITIONS FOR FILTER  */
#define _WEATHER_CONFIG_REG_FILTER_POS          2
#define _WEATHER_CONFIG_REG_FILTER_MSK          0x1C
#define _WEATHER_CONFIG_REG_FILTER_LEN          3
#define _WEATHER_CONFIG_REG_FILTER_REG          _WEATHER_CONFIG_REG

/*         BIT MASK, LENGTH AND POSITION DEFINITIONS FOR SPI ENABLE  */
#define _WEATHER_CONFIG_REG_SPI3_ENABLE_POS     0
#define _WEATHER_CONFIG_REG_SPI3_ENABLE_MSK     0x01
#define _WEATHER_CONFIG_REG_SPI3_ENABLE_LEN     1
#define _WEATHER_CONFIG_REG_SPI3_ENABLE_REG     _WEATHER_CONFIG_REG

/* BIT MASK, LENGTH AND POSITION DEFINITIONS FOR PRESSURE AND TEMPERATURE DATA  */
/* Data Register */
#define _WEATHER_PRESSURE_XLSB_REG_DATA_POS     4
#define _WEATHER_PRESSURE_XLSB_REG_DATA_MSK     0xF0
#define _WEATHER_PRESSURE_XLSB_REG_DATA_LEN     4
#define _WEATHER_PRESSURE_XLSB_REG_DATA_REG     _WEATHER_PRESSURE_XLSB_REG
#define _WEATHER_TEMPERATURE_XLSB_REG_DATA_POS  4
#define _WEATHER_TEMPERATURE_XLSB_REG_DATA_MSK  0xF0
#define _WEATHER_TEMPERATURE_XLSB_REG_DATA_LEN  4
#define _WEATHER_TEMPERATURE_XLSB_REG_DATA_REG  _WEATHER_TEMPERATURE_XLSB_REG

#ifdef __cplusplus
extern "C"{
#endif

#ifdef   __WEATHER_DRV_SPI__
void weather_spiDriverInit(T_WEATHER_P gpioObj, T_WEATHER_P spiObj);
#endif
#ifdef   __WEATHER_DRV_I2C__
void weather_i2cDriverInit(T_WEATHER_P gpioObj, T_WEATHER_P i2cObj, uint8_t slave);
#endif
#ifdef   __WEATHER_DRV_UART__
void weather_uartDriverInit(T_WEATHER_P gpioObj, T_WEATHER_P uartObj);
#endif

/**
 * @brief Generic write byte of data function
 *
 * @param[in] regAddress                8-bit register address
 *
 * @param[in] writeData                 Byte of data to write
 *
 * Function write the 8-bit of data to the
 * target 8-bit register address of BME280 chip on Weather Click board.
 */
void weather_writeData( uint8_t regAddress, uint8_t writeData );

/**
 * @brief Generic read byte of data function
 *
 * @param[in] regAddress                Register address
 *
 * Function read the 8-bit of data from the
 * target 8-bit register address of BME280 chip on Weather Click board.
 */
uint8_t weather_readData( uint8_t regAddress );

/**
 * @brief Read measurements function
 *
 * Function read measurements from the
 * target 8-bit register address of BME280 chip on Weather Click board.
 */
void weather_readMeasurements();

/**
 * @brief Get ID function
 *
 * @return
 * 8-bit device ID
 *
 * Function read device ID from the
 * target chip ID register address of BME280 chip on Weather Click board.
 */
uint8_t weather_getID();

/**
 * @brief Soft reset function
 *
 * Function soft reset by write value of 0xB6 to the
 * target reset register address of BME280 chip on Weather Click board.
 */
void weather_softReset();

/**
 * @brief Get status function
 *
 * @return
 * 8-bit status value
 *
 * Function read device status from the
 * target status register address of BME280 chip on Weather Click board.
 */
uint8_t weather_getStatus();

/**
 * @brief Get control measurement function
 *
 * @return
 * 8-bit control measurement value
 *
 * Function read control measurement value from the
 * target control measurement register address of BME280 chip on Weather Click board.
 */
uint8_t weather_getCtrlMeasurement();

/**
 * @brief Get control humidity function
 *
 * @return
 * 8-bit control humidity value
 *
 * Function read control humidity value from the
 * target control humidity register address of BME280 chip on Weather Click board.
 */
uint8_t weather_getCtrlHumidity();

/**
 * @brief Get configuration function
 *
 * @return
 * 8-bit configuration value
 *
 * Function read configuration value from the
 * target configuration register address of BME280 chip on Weather Click board.
 */
uint8_t weather_getConfig();

/**
 * @brief Read factory calibration parameters function
 *
 * Function read factory calibration parameters value from the
 * calibration registers address of BME280 chip on Weather Click board.
 */
void weather_readCalibrationParams();

/**
 * @brief Set pressure oversampling function
 *
 * @param[in] value                  8-bit oversampling pressure value
 *
 * Function write oversampling pressure value to the
 * target control measurement register address of BME280 chip on Weather Click board.
 */
void weather_setOversamplingPressure( uint8_t value );

/**
 * @brief Set temperature oversampling function
 *
 * @param[in] value                  8-bit oversampling temperature value
 *
 * Function write oversampling temperature value to the
 * target control measurement register address of BME280 chip on Weather Click board.
 */
void weather_setOversamplingTemperature( uint8_t value );

/**
 * @brief Set humidity oversampling function
 *
 * @param[in] value                  8-bit oversampling humidity value
 *
 * Function write oversampling humidity value to the
 * target control measurement register address of BME280 chip on Weather Click board.
 */
void weather_setOversamplingHumidity( uint8_t value );

/**
 * @brief Set oversampling mode function
 *
 * @param[in] value                  8-bit oversampling mode value
 *
 * Function write oversampling mode value to the
 * target control measurement register address of BME280 chip on Weather Click board.
 */
void weather_setOversamplingMode( uint8_t value );

/**
 * @brief Set filter coefficient function
 *
 * @param[in] value                  8-bit filter coefficient value
 *
 * Function write filter coefficient value to the
 * target configuration register address of BME280 chip on Weather Click board.
 */
void weather_setFilterCoefficient( uint8_t value );

/**
 * @brief Set stand by time function
 *
 * @param[in] value                  8-bit stand by time value
 *
 * Function write stand by time value to the
 * target configuration register address of BME280 chip on Weather Click board.
 */
void weather_setStandbyTime( uint8_t value );

/**
 * @brief Get measuring status function
 *
 * @return
 * 8-bit configuration value
 *
 * Function read measuring status value from the
 * target status register address of BME280 chip on Weather Click board.
 */
uint8_t weather_isMeasuring();

/**
 * @brief Gets temperature in degrees Celsius function
 *
 * @return
 * float value of temperature in degrees Celsius
 *
 * Function get temperature value in degrees Celsius [ ºC ] from the
 * target temperature register address of BME280 chip on Weather Click board.
 */
float weather_getTemperature();

/**
 * @brief Gets humidity in percentage [ % ] function
 *
 * @return
 * float value of humidity in percentage [ % ]
 *
 * Function get humidity value in percentage [ % ] from the
 * target humidity register address of BME280 chip on Weather Click board.
 */
float weather_getHumidity();

/**
 * @brief Gets pressure in degrees Celsius function
 *
 * @return
 * float value of pressure in [ mbar ]
 *
 * Function get pressure value in [ mbar ] from the
 * target pressure register address of BME280 chip on Weather Click board.
 */
float weather_getPressure();

/**
 * @brief Get weather value function
 *
 * @param[out] temperature              pointer to float value of temperature [ ºC ]
 *
 * @param[out] humidity                 pointer to float value of humidity [ % ]
 *
 * @param[out] pressure                 pointer to float value of pressure [ mbar ]
 *
 * Function gets weather value of temperature value,
 * humidity value and pressure value by read from the
 * temperature, humidity and pressure register address of BME280 chip on Weather Click board
 * and convert temperature in degrees Celsius [ ºC ], humidity in percentage [ % ]
 * and pressure in [ mbar ].
 */
void weather_getWeather( float *temperature, float *humidity, float *pressure );

#ifdef __cplusplus
} // extern "C"
#endif
#endif

/**
    \example Click_Weather_STM.c
    \example Click_Weather_TIVA.c
    \example Click_Weather_CEC.c
    \example Click_Weather_KINETIS.c
    \example Click_Weather_MSP.c
    \example Click_Weather_PIC.c
    \example Click_Weather_PIC32.c
    \example Click_Weather_DSPIC.c
    \example Click_Weather_AVR.c
    \example Click_Weather_FT90x.c
    \example Click_Weather_STM.mbas
    \example Click_Weather_TIVA.mbas
    \example Click_Weather_CEC.mbas
    \example Click_Weather_KINETIS.mbas
    \example Click_Weather_MSP.mbas
    \example Click_Weather_PIC.mbas
    \example Click_Weather_PIC32.mbas
    \example Click_Weather_DSPIC.mbas
    \example Click_Weather_AVR.mbas
    \example Click_Weather_FT90x.mbas
    \example Click_Weather_STM.mpas
    \example Click_Weather_TIVA.mpas
    \example Click_Weather_CEC.mpas
    \example Click_Weather_KINETIS.mpas
    \example Click_Weather_MSP.mpas
    \example Click_Weather_PIC.mpas
    \example Click_Weather_PIC32.mpas
    \example Click_Weather_DSPIC.mpas
    \example Click_Weather_AVR.mpas
    \example Click_Weather_FT90x.mpas
*/                                                                     
/// \}
/* -------------------------------------------------------------------------- */
/*
    Weather_click.h

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