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
#define TIMER6_CLOCK_SRC_MUX_ADDR (0x1081C8u)
#define TIMER6_CLOCK_SRC_MCU_HFOSC0 (0x0u)
#define TIMER6_BASE_ADDR     (0x2460000u)

ClockP_Config gClockConfig = {
    .timerBaseAddr = TIMER6_BASE_ADDR,
    .timerHwiIntNum = 158,
    .timerInputClkHz = 25000000,
    .timerInputPreScaler = 1,
    .usecPerTick = 1000,
};

/* ----------- DebugP ----------- */
void putchar_(char character)
{
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
#define CONFIG_MMU_NUM_REGIONS  (3u)

MmuP_Config gMmuConfig = {
    .numRegions = CONFIG_MMU_NUM_REGIONS,
    .enableMmu = 1,
};

MmuP_RegionConfig gMmuRegionConfig[CONFIG_MMU_NUM_REGIONS] =
{
    {
        .vaddr = 0x0u,
        .paddr = 0x0u,
        .size = 0x80000000u,
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
        .size = 0x80000000u,
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
};

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

    /* Initialize Debug module from Core0 only */
    if(0 == Armv8_getCoreId())
    {

    /* Debug log init */
    DebugP_logZoneEnable(DebugP_LOG_ZONE_ERROR);
    DebugP_logZoneEnable(DebugP_LOG_ZONE_WARN);
    /* Initialize shared memory writer on this CPU */
    DebugP_shmLogWriterInit(&gDebugShmLog[CSL_CORE_ID_A53SS0_0], CSL_CORE_ID_A53SS0_0);

    }

    /* set timer clock source */
    SOC_controlModuleUnlockMMR(SOC_DOMAIN_ID_MAIN, 2);
    *(volatile uint32_t*)(TIMER6_CLOCK_SRC_MUX_ADDR) = TIMER6_CLOCK_SRC_MCU_HFOSC0;
    SOC_controlModuleLockMMR(SOC_DOMAIN_ID_MAIN, 2);
    /* Initialize Clock from Core0 only */
    if(0 == Armv8_getCoreId())
    {

    /* initialize Clock */
    ClockP_init();


    }

    /* Enable interrupt handling */
    HwiP_enable();
}

void Dpl_deinit(void)
{
    /* de-initialize Clock */
    ClockP_deinit();
    /* Disable interrupt handling */
    HwiP_disable();
}
