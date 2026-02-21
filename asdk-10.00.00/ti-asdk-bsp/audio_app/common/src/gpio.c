/*
 * Copyright (C) 2021 Texas Instruments Incorporated
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the
 *   distribution.
 *
 *   Neither the name of Texas Instruments Incorporated nor the names of
 *   its contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 *  \file   gpio.c
 *
 *  \brief  Low lever APIs performing hardware register writes and reads for
 *          GPIO IP version 0.
 *
 *   This file contains the hardware register write/read APIs for GPIO.
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <stdint.h>
#include <drivers/gpio.h>
#include <stdio.h>
#include <kernel/dpl/DebugP.h>
#include <kernel/dpl/SemaphoreP.h>
#include "ti_drivers_config.h"

#include "drivers/mcasp.h"
#include "ti_drivers_open_close.h"
#include "ti_board_open_close.h"
#include <string.h>
#include "math.h"

#include "platform.h"

#include <drivers/gpio.h>
#include <drivers/pinmux.h>
#include <kernel/dpl/AddrTranslateP.h>

#include "ti_drivers_config.h"



/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */
uint32_t gpioBaseAddr;

void GPIO_setDirMode(uint32_t baseAddr, uint32_t pinNum, uint32_t pinDir)
{
    uint32_t                regIndex, bitPos;
    volatile CSL_GpioRegs*  hGpio = (volatile CSL_GpioRegs*)((uintptr_t) baseAddr);

    regIndex = GPIO_GET_REG_INDEX(pinNum);
    bitPos = GPIO_GET_BIT_POS(pinNum);
    CSL_FINSR(hGpio->BANK_REGISTERS[regIndex].DIR, bitPos, bitPos, (((uint32_t) pinDir) & 0x01U));

    return;
}

uint32_t GPIO_pinRead(uint32_t baseAddr, uint32_t pinNum)
{
    uint32_t                inData;
    uint32_t                regIndex, bitPos;
    volatile CSL_GpioRegs*  hGpio = (volatile CSL_GpioRegs*)((uintptr_t) baseAddr);

    regIndex = GPIO_GET_REG_INDEX(pinNum);
    bitPos = GPIO_GET_BIT_POS(pinNum);
    inData = CSL_FEXTR(hGpio->BANK_REGISTERS[regIndex].IN_DATA, bitPos, bitPos);

    return (inData);
}

uint32_t GPIO_pinOutValueRead(uint32_t baseAddr, uint32_t pinNum)
{
    uint32_t                outData;
    uint32_t                regIndex, bitPos;
    volatile CSL_GpioRegs*  hGpio = (volatile CSL_GpioRegs*)((uintptr_t) baseAddr);

    regIndex = GPIO_GET_REG_INDEX(pinNum);
    bitPos = GPIO_GET_BIT_POS(pinNum);
    outData = CSL_FEXTR(hGpio->BANK_REGISTERS[regIndex].OUT_DATA, bitPos, bitPos);

    return (outData);
}

void GPIO_setTrigType(uint32_t baseAddr, uint32_t pinNum, uint32_t trigType)
{
    uint32_t                regIndex, regVal;
    volatile CSL_GpioRegs*  hGpio = (volatile CSL_GpioRegs*)((uintptr_t) baseAddr);

    regIndex = GPIO_GET_REG_INDEX(pinNum);
    regVal = GPIO_GET_BIT_MASK(pinNum);
    switch(trigType)
    {
        case GPIO_TRIG_TYPE_NONE:
        {
            /* Disable both rising and falling edge detection interrupts */
            CSL_REG32_WR(&hGpio->BANK_REGISTERS[regIndex].CLR_RIS_TRIG, regVal);
            CSL_REG32_WR(&hGpio->BANK_REGISTERS[regIndex].CLR_FAL_TRIG, regVal);
            break;
        }

        case GPIO_TRIG_TYPE_RISE_EDGE:
        {
            /* Only enable rising edge detection interupt */
            CSL_REG32_WR(&hGpio->BANK_REGISTERS[regIndex].CLR_FAL_TRIG, regVal);
            CSL_REG32_WR(&hGpio->BANK_REGISTERS[regIndex].SET_RIS_TRIG, regVal);
            break;
        }

        case GPIO_TRIG_TYPE_FALL_EDGE:
        {
            /* Only enable falling edge detection interupt */
            CSL_REG32_WR(&hGpio->BANK_REGISTERS[regIndex].CLR_RIS_TRIG, regVal);
            CSL_REG32_WR(&hGpio->BANK_REGISTERS[regIndex].SET_FAL_TRIG, regVal);
            break;
        }

        case GPIO_TRIG_TYPE_BOTH_EDGE:
        {
            /* Enable both rising and falling edge detection interrupts */
            CSL_REG32_WR(&hGpio->BANK_REGISTERS[regIndex].SET_RIS_TRIG, regVal);
            CSL_REG32_WR(&hGpio->BANK_REGISTERS[regIndex].SET_FAL_TRIG, regVal);
            break;
        }

        default:
            break;
    }

    return;
}

void GPIO_bankIntrEnable(uint32_t baseAddr, uint32_t bankNum)
{
    volatile CSL_GpioRegs*  hGpio = (volatile CSL_GpioRegs*)((uintptr_t) baseAddr);

    /* Enable bank interrupt */
    CSL_FINSR(hGpio->BINTEN, bankNum, bankNum, 1U);

    return;
}

void GPIO_bankIntrDisable(uint32_t baseAddr, uint32_t bankNum)
{
    volatile CSL_GpioRegs*  hGpio = (volatile CSL_GpioRegs*)((uintptr_t) baseAddr);

    /* Disable bank interrupt */
    CSL_FINSR(hGpio->BINTEN, bankNum, bankNum, 0U);

    return;
}

void debugGpio_setup()
{
    static Pinmux_PerCfg_t gPinMuxMainDomainCfg[] = {
        {PIN_OSPI0_CSN3, (PIN_MODE(7) | PIN_PULL_DISABLE)},
        {PIN_GPMC0_BE1N, (PIN_MODE(7) | PIN_PULL_DISABLE)},
        {PIN_GPMC0_WPN,  (PIN_MODE(7) | PIN_PULL_DISABLE)},
        {PIN_GPMC0_DIR,  (PIN_MODE(7) | PIN_PULL_DISABLE)},
        {PINMUX_END, PINMUX_END}};

    Pinmux_config(gPinMuxMainDomainCfg, PINMUX_DOMAIN_ID_MAIN);

    gpioBaseAddr = (uint32_t)AddrTranslateP_getLocalAddr(CSL_GPIO0_BASE);

    GPIO_setDirMode(gpioBaseAddr, mcasp_debug_pin, GPIO_DIRECTION_OUTPUT);
    GPIO_setDirMode(gpioBaseAddr, uart_debug_pin, GPIO_DIRECTION_OUTPUT);
    GPIO_setDirMode(gpioBaseAddr, awecore_debug_pin, GPIO_DIRECTION_OUTPUT);
    GPIO_setDirMode(gpioBaseAddr, generic_debug_pin, GPIO_DIRECTION_OUTPUT);
}

void debugGpio_toggle(AweDebugGpio pinNum)
{
    if (GPIO_pinOutValueRead(gpioBaseAddr, pinNum))
    {
        GPIO_pinWriteLow(gpioBaseAddr, pinNum);
    }
    else
    {
        GPIO_pinWriteHigh(gpioBaseAddr, pinNum);
    }
}

void debugGpio_high(AweDebugGpio pinNum)
{
    GPIO_pinWriteHigh(gpioBaseAddr, pinNum);
}

void debugGpio_low(AweDebugGpio pinNum)
{
    GPIO_pinWriteLow(gpioBaseAddr, pinNum);
}
