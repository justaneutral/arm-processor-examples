/*
 *  Copyright (C) 2018-2023 Texas Instruments Incorporated
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

#include <stdlib.h>
#include "ti_drivers_config.h"
#include "ti_board_config.h"
#include "ti_drivers_open_close.h"
#include "ti_board_open_close.h"
#include <kernel/dpl/CycleCounterP.h>
#include "platform.h"
#include "TuningDriver.h"

/* main thread of audio driver */
#define MAIN_TASK_PRI  (configMAX_PRIORITIES-5)
#define MAIN_TASK_SIZE (65536U/sizeof(configSTACK_DEPTH_TYPE))
StackType_t gMainTaskStack[MAIN_TASK_SIZE] __attribute__((aligned(32)));
StaticTask_t gMainTaskObj;
TaskHandle_t gMainTask;

extern volatile uint64_t ullPortSchedularRunning;

void Audio_io_setup();

void awe_mainTask_smp(void *args)
{
    Audio_io_setup();
    vTaskDelete(NULL);
}

void start_smp_freeRTOS()
{
    if (0 == Armv8_getCoreId())
    {
        awe_createInstance();
        TuningServerCreate();
        gMainTask = xTaskCreateStatic( awe_mainTask_smp,   
                                    "awe_mainTask_smp", 
                                    MAIN_TASK_SIZE,  
                                    NULL,            
                                    MAIN_TASK_PRI,   
                                    gMainTaskStack,  
                                    &gMainTaskObj ); 
        configASSERT(gMainTask != NULL);

        vTaskCoreAffinitySet(gMainTask, 1);
        configASSERT(gMainTask != NULL);

        /* Start the scheduler to start the tasks executing. */
        vTaskStartScheduler();
    }
    else
    {
        while(ullPortSchedularRunning == 0)
        {
            ;
        }
        xPortStartScheduler();
    }

    // It should never come over here
    DebugP_assertNoLog(0);
}

int main()
{
    int32_t status = SystemP_SUCCESS;

    System_init();
    Board_init();

    /* Open drivers */
    Drivers_open();

    /* Open flash and board drivers */
    status = Board_driversOpen();
    DebugP_assert(status==SystemP_SUCCESS);

    CycleCounterP_reset();

    start_smp_freeRTOS();

    return 0;
}
