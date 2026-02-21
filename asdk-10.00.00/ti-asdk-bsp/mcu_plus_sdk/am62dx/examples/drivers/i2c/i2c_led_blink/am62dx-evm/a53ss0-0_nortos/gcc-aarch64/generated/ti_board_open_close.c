/*
 *  Copyright (C) 2021 Texas Instruments Incorporated
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Auto generated file 
 */

#include "ti_board_open_close.h"

int32_t Board_driversOpen(void)
{
    int32_t status = SystemP_SUCCESS;

    if(0 == Armv8_getCoreId())
    {

    if(status==SystemP_SUCCESS)
    {
        status = Board_ledOpen();
    }


    }

    return status;
}

void Board_driversClose(void)
{

    if(0 == Armv8_getCoreId())
    {

    Board_ledClose();


    }

}

/*
 * LED
 */
/* LED specific includes */
#include <board/led/led_ioexp.h>

/* LED Object - initalized during LED_open() */
LED_Object gLedObject[CONFIG_LED_NUM_INSTANCES];
/* LED Driver handles - opened during Board_ledOpen() */
LED_Handle gLedHandle[CONFIG_LED_NUM_INSTANCES];

/* LED Config */
LED_Config gLedConfig[CONFIG_LED_NUM_INSTANCES] =
{
    {
        .attrs = &gLedAttrs_Ioexp,
        .fxns = &gLedFxns_Ioexp,
        .object = (void *)&gLedObject[0],
    },
};
uint32_t gLedConfigNum = CONFIG_LED_NUM_INSTANCES;

/* LED params */
LED_Params gLedParams[CONFIG_LED_NUM_INSTANCES] =
{
    {
        .gpioBaseAddr   = 0U,
        .gpioPinNum     = 0U,
        .i2cInstance    = CONFIG_I2C0,
        .i2cAddress     = 0x22,
        .ioIndex        = 23U,
    },
};

int32_t Board_ledOpen()
{
    uint32_t instCnt;
    int32_t  status = SystemP_SUCCESS;

    for(instCnt = 0U; instCnt < CONFIG_LED_NUM_INSTANCES; instCnt++)
    {
        gLedHandle[instCnt] = NULL;   /* Init to NULL so that we can exit gracefully */
    }

    /* Open all instances */
    for(instCnt = 0U; instCnt < CONFIG_LED_NUM_INSTANCES; instCnt++)
    {
        gLedHandle[instCnt] = LED_open(instCnt, &gLedParams[instCnt]);
        if(NULL == gLedHandle[instCnt])
        {
            DebugP_logError("LED open failed for instance %d !!!\r\n", instCnt);
            status = SystemP_FAILURE;
            break;
        }
    }

    if(SystemP_FAILURE == status)
    {
        Board_ledClose();   /* Exit gracefully */
    }

    return status;
}

void Board_ledClose(void)
{
    uint32_t instCnt;

    /* Close all instances that are open */
    for(instCnt = 0U; instCnt < CONFIG_LED_NUM_INSTANCES; instCnt++)
    {
        if(gLedHandle[instCnt] != NULL)
        {
            LED_close(gLedHandle[instCnt]);
            gLedHandle[instCnt] = NULL;
        }
    }

    return;
}

