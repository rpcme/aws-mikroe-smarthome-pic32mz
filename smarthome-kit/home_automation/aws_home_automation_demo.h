/*
 * Amazon FreeRTOS V1.4.4
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

#ifndef _AWS_CLICKBOARD_H_
#define _AWS_CLICKBOARD_H_

#define AWS_WORKSHOP 1

#if AWS_WORKSHOP == 1
#define AWS_WORKSHOP_SECTION_2_CONN_1    1
#define AWS_WORKSHOP_SECTION_2_CONN_2    0
#define AWS_WORKSHOP_SECTION_3_TELEMETRY 0
#define AWS_WORKSHOP_SECTION_4_SHADOW    0
#define AWS_WORKSHOP_SECTION_5_OTA       0
#else
#define AWS_WORKSHOP_SECTION_2_CONN_1    1
#define AWS_WORKSHOP_SECTION_2_CONN_2    1
#define AWS_WORKSHOP_SECTION_3_TELEMETRY 1
#define AWS_WORKSHOP_SECTION_4_SHADOW    1
#define AWS_WORKSHOP_SECTION_5_OTA       1
#endif

#include "aws_demo.h"

void vStartRemoteHVACDemo( void );

#include "FreeRTOS.h"
#include "remote_hvac/module_hvac.h"

typedef struct Shadow_Properties
{
    uint8_t           req;  // Used for request type
    float             target_temp;
    FAN_STATE         fan;
    AIRCON_STATE      aircon;
} xShadowProperties;


#endif
