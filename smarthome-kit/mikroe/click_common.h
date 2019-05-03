/*
   click_common.h

 ------------------------------------------------------------------------------

  Copyright (c) 2017, MikroElektonika - http://www.mikroe.com

  All rights reserved.

 ------------------------------------------------------------------------------

- Version               : 1.0.0
- Date                  : Dec 2018.
- Developer             : Milos Vidojevic
    
---

- 1.0.0 : Module created (Milos Vidojevic)

---

    \note

SPI support only.

----------------------------------------------------------------------------- */

#ifndef _HAL_PIC32_HARMONY_
#define _HAL_PIC32_HARMONY_

#include "system/common/sys_module.h"
#include "driver/driver_common.h"

// ----------------------------- MPLAB PIC32 HARMONY HAL TO MIKROSDK ADAPTATION

static DRV_HANDLE spi_obj;

static void hal_spiMap(T_HAL_P spiObj)
{
    spi_obj = (DRV_HANDLE)spiObj;
}

static void hal_spiWrite(uint8_t *pBuf, uint16_t nBytes)
{
    DRV_SPI_BufferAddWrite2(spi_obj, (void*)pBuf, (size_t)nBytes, 
                NULL, NULL, NULL);    
}

static void hal_spiRead(uint8_t *pBuf, uint16_t nBytes)
{
    DRV_SPI_BufferAddRead2(spi_obj, (void*)pBuf, (size_t)nBytes, 
                NULL, NULL, NULL);
}

static void hal_spiTransfer(uint8_t *pIn, uint8_t *pOut, uint16_t nBytes)
{
    DRV_SPI_BufferAddWriteRead2(spi_obj, (void*)pIn, nBytes, (void*)pOut, 
                nBytes, NULL, NULL, NULL);
}

#endif

/* -------------------------------------------------------------------------- */
/*
    click_common.h

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