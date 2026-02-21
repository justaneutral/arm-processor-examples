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
#include <stdio.h>
#include <drivers/soc.h>
#include <kernel/dpl/AddrTranslateP.h>
#include "ti_dpl_config.h"
#include "ti_drivers_config.h"


/* ----------- ClockP ----------- */
#define TIMER2_CLOCK_SRC_MUX_ADDR (0x1081B8u)
#define TIMER2_CLOCK_SRC_MCU_HFOSC0 (0x0u)
#define TIMER2_BASE_ADDR     (0x2420000u)

ClockP_Config gClockConfig = {
    .timerBaseAddr = TIMER2_BASE_ADDR,
    .timerHwiIntNum = 10,
    .eventId = 378,
    .timerInputClkHz = 25000000,
    .timerInputPreScaler = 1,
    .usecPerTick = 1000,
};

/* ----------- DebugP ----------- */
void putchar_(char character)
{
    /* Output to CCS console */
    putchar(character);
    /* Output to shared memory */
    DebugP_shmLogWriterPutChar(character);
}

/* Shared memory log base address, logs of each CPUs are put one after other in the below region.
 *
 * IMPORTANT: Make sure of below,
 * - The section defined below should be placed at the exact same location in memory for all the CPUs
 * - The memory should be marked as non-cached for all the CPUs
 * - The section should be marked as NOLOAD in all the CPUs linker command file
 */
DebugP_ShmLog gDebugShmLog[CSL_CORE_ID_MAX] __attribute__((aligned(128), section(".bss.log_shared_mem")));

/* ----------- MmuP_armv8 ----------- */
#define CONFIG_MMU_NUM_REGIONS  (14u)

MmuP_Config gMmuConfig = {
    .numRegions = CONFIG_MMU_NUM_REGIONS,
    .enableMmu = 1,
};

MmuP_RegionConfig gMmuRegionConfig[CONFIG_MMU_NUM_REGIONS] =
{
    {
        .vaddr = 0x0u,
        .paddr = 0x0u,
        .size = 0x20000000u,
        .attr = {
            .accessPerm = MMUP_ACCESS_PERM_PRIV_RW_USER_NONE,
            .privExecute = 1,
            .userExecute = 0,
            .shareable = MMUP_SHARABLE_OUTER,
            .attrIndx = MMUP_ATTRINDX_MAIR0,
            .global = 1
        }
    },
    {
        .vaddr = 0x20000000u,
        .paddr = 0x20000000u,
        .size = 0x20000000u,
        .attr = {
            .accessPerm = MMUP_ACCESS_PERM_PRIV_RW_USER_NONE,
            .privExecute = 1,
            .userExecute = 0,
            .shareable = MMUP_SHARABLE_OUTER,
            .attrIndx = MMUP_ATTRINDX_MAIR0,
            .global = 1
        }
    },
    {
        .vaddr = 0x40000000u,
        .paddr = 0x40000000u,
        .size = 0x20000000u,
        .attr = {
            .accessPerm = MMUP_ACCESS_PERM_PRIV_RW_USER_NONE,
            .privExecute = 1,
            .userExecute = 0,
            .shareable = MMUP_SHARABLE_OUTER,
            .attrIndx = MMUP_ATTRINDX_MAIR0,
            .global = 1
        }
    },
    {
        .vaddr = 0x60000000u,
        .paddr = 0x60000000u,
        .size = 0x10000000u,
        .attr = {
            .accessPerm = MMUP_ACCESS_PERM_PRIV_RW_USER_NONE,
            .privExecute = 1,
            .userExecute = 0,
            .shareable = MMUP_SHARABLE_OUTER,
            .attrIndx = MMUP_ATTRINDX_MAIR0,
            .global = 1
        }
    },
    {
        .vaddr = 0x7C200000u,
        .paddr = 0x7C200000u,
        .size = 0x100000u,
        .attr = {
            .accessPerm = MMUP_ACCESS_PERM_PRIV_RW_USER_NONE,
            .privExecute = 1,
            .userExecute = 0,
            .shareable = MMUP_SHARABLE_OUTER,
            .attrIndx = MMUP_ATTRINDX_MAIR0,
            .global = 1
        }
    },
    {
        .vaddr = 0x7C400000u,
        .paddr = 0x7C400000u,
        .size = 0x100000u,
        .attr = {
            .accessPerm = MMUP_ACCESS_PERM_PRIV_RW_USER_NONE,
            .privExecute = 1,
            .userExecute = 0,
            .shareable = MMUP_SHARABLE_OUTER,
            .attrIndx = MMUP_ATTRINDX_MAIR0,
            .global = 1
        }
    },
    {
        .vaddr = 0x80000000u,
        .paddr = 0x80000000u,
        .size = 0x20000000u,
        .attr = {
            .accessPerm = MMUP_ACCESS_PERM_PRIV_RW_USER_NONE,
            .privExecute = 1,
            .userExecute = 0,
            .shareable = MMUP_SHARABLE_OUTER,
            .attrIndx = MMUP_ATTRINDX_MAIR7,
            .global = 1
        }
    },
    {
        .vaddr = 0xA0000000u,
        .paddr = 0xA0000000u,
        .size = 0x20000000u,
        .attr = {
            .accessPerm = MMUP_ACCESS_PERM_PRIV_RW_USER_NONE,
            .privExecute = 1,
            .userExecute = 0,
            .shareable = MMUP_SHARABLE_OUTER,
            .attrIndx = MMUP_ATTRINDX_MAIR7,
            .global = 1
        }
    },
    {
        .vaddr = 0xA0400000u,
        .paddr = 0xA0400000u,
        .size = 0x300000u,
        .attr = {
            .accessPerm = MMUP_ACCESS_PERM_PRIV_RW_USER_NONE,
            .privExecute = 1,
            .userExecute = 0,
            .shareable = MMUP_SHARABLE_OUTER,
            .attrIndx = MMUP_ATTRINDX_MAIR4,
            .global = 1
        }
    },
    {
        .vaddr = 0xAA000000u,
        .paddr = 0xAA000000u,
        .size = 0x2000000u,
        .attr = {
            .accessPerm = MMUP_ACCESS_PERM_PRIV_RW_USER_NONE,
            .privExecute = 1,
            .userExecute = 0,
            .shareable = MMUP_SHARABLE_OUTER,
            .attrIndx = MMUP_ATTRINDX_MAIR4,
            .global = 1
        }
    },
    {
        .vaddr = 0x99800000u,
        .paddr = 0x99800000u,
        .size = 0x1000000u,
        .attr = {
            .accessPerm = MMUP_ACCESS_PERM_PRIV_RW_USER_NONE,
            .privExecute = 1,
            .userExecute = 0,
            .shareable = MMUP_SHARABLE_OUTER,
            .attrIndx = MMUP_ATTRINDX_MAIR4,
            .global = 1
        }
    },
    {
        .vaddr = 0xA1000000u,
        .paddr = 0xA1000000u,
        .size = 0x40000u,
        .attr = {
            .accessPerm = MMUP_ACCESS_PERM_PRIV_RW_USER_NONE,
            .privExecute = 1,
            .userExecute = 0,
            .shareable = MMUP_SHARABLE_OUTER,
            .attrIndx = MMUP_ATTRINDX_MAIR4,
            .global = 1
        }
    },
    {
        .vaddr = 0x99C10000u,
        .paddr = 0x99C10000u,
        .size = 0x1000u,
        .attr = {
            .accessPerm = MMUP_ACCESS_PERM_PRIV_RW_USER_NONE,
            .privExecute = 1,
            .userExecute = 0,
            .shareable = MMUP_SHARABLE_OUTER,
            .attrIndx = MMUP_ATTRINDX_MAIR4,
            .global = 1
        }
    },
    {
        .vaddr = 0xA3000000u,
        .paddr = 0xA3000000u,
        .size = 0x80000u,
        .attr = {
            .accessPerm = MMUP_ACCESS_PERM_PRIV_RW_USER_NONE,
            .privExecute = 1,
            .userExecute = 0,
            .shareable = MMUP_SHARABLE_OUTER,
            .attrIndx = MMUP_ATTRINDX_MAIR4,
            .global = 1
        }
    },
};

/* ----------- TimerP ----------- */
#define CONFIG_TIMER1_CLOCK_SRC_MUX_ADDR (0x1081B0u)
#define CONFIG_TIMER1_CLOCK_SRC_HFOSC0_CLKOUT (0x0u)


HwiP_Object gTimerHwiObj[TIMER_NUM_INSTANCES];
uint32_t gTimerBaseAddr[TIMER_NUM_INSTANCES];

void TimerP_isr0(void *args)
{
    void remoteTimerIsr(void *args);

    remoteTimerIsr(args);
    TimerP_clearOverflowInt(gTimerBaseAddr[CONFIG_TIMER1]);
    HwiP_clearInt(CONFIG_TIMER1_INT_NUM);
}

void TimerP_init()
{
    TimerP_Params timerParams;
    HwiP_Params timerHwiParams;
    int32_t status;

    /* set timer clock source */
    SOC_controlModuleUnlockMMR(SOC_DOMAIN_ID_MAIN, 2);
    *(volatile uint32_t*)AddrTranslateP_getLocalAddr(CONFIG_TIMER1_CLOCK_SRC_MUX_ADDR) = CONFIG_TIMER1_CLOCK_SRC_HFOSC0_CLKOUT;
    SOC_controlModuleLockMMR(SOC_DOMAIN_ID_MAIN, 2);

    gTimerBaseAddr[CONFIG_TIMER1] = (uint32_t)AddrTranslateP_getLocalAddr(CONFIG_TIMER1_BASE_ADDR);

    TimerP_Params_init(&timerParams);
    timerParams.inputPreScaler = CONFIG_TIMER1_INPUT_PRE_SCALER;
    timerParams.inputClkHz     = CONFIG_TIMER1_INPUT_CLK_HZ;
    timerParams.periodInNsec   = CONFIG_TIMER1_NSEC_PER_TICK;
    timerParams.oneshotMode    = 0;
    timerParams.enableOverflowInt = 1;
    timerParams.enableDmaTrigger  = 0;
    TimerP_setup(gTimerBaseAddr[CONFIG_TIMER1], &timerParams);

    HwiP_Params_init(&timerHwiParams);
    timerHwiParams.intNum = CONFIG_TIMER1_INT_NUM;
    timerHwiParams.eventId = CONFIG_TIMER1_EVENT_ID;
    timerHwiParams.callback = TimerP_isr0;
    timerHwiParams.isPulse = 0;
    timerHwiParams.priority = 4;
    status = HwiP_construct(&gTimerHwiObj[CONFIG_TIMER1], &timerHwiParams);
    DebugP_assertNoLog(status==SystemP_SUCCESS);

}

void TimerP_deinit()
{
    TimerP_stop(gTimerBaseAddr[CONFIG_TIMER1]);
    HwiP_destruct(&gTimerHwiObj[CONFIG_TIMER1]);

}
/* This function is called by __system_start */
void __mmu_init()
{
    MmuP_init();
    CacheP_enable(CacheP_TYPE_ALL);
}

void Dpl_init(void)
{
    /* initialize Hwi but keep interrupts disabled */
    HwiP_init();

    /* init debug log zones early */
    /* Debug log init */
    DebugP_logZoneEnable(DebugP_LOG_ZONE_ERROR);
    DebugP_logZoneEnable(DebugP_LOG_ZONE_WARN);
    /* Initialize shared memory writer on this CPU */
    DebugP_shmLogWriterInit(&gDebugShmLog[CSL_CORE_ID_C75SS0_0], CSL_CORE_ID_C75SS0_0);


    /* set timer clock source */
    SOC_controlModuleUnlockMMR(SOC_DOMAIN_ID_MAIN, 2);
    *(volatile uint32_t*)(TIMER2_CLOCK_SRC_MUX_ADDR) = TIMER2_CLOCK_SRC_MCU_HFOSC0;
    SOC_controlModuleLockMMR(SOC_DOMAIN_ID_MAIN, 2);
    /* initialize Clock */
    ClockP_init();


    TimerP_init();
    /* Enable interrupt handling */
    HwiP_enable();
}

void Dpl_deinit(void)
{
    /* de-initialize Clock */
    ClockP_deinit();
    TimerP_deinit();
    /* Disable interrupt handling */
    HwiP_disable();
}
