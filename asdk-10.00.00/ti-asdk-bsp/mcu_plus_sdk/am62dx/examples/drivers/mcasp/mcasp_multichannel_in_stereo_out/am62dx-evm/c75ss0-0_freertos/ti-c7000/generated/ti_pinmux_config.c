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
#include "ti_drivers_config.h"
#include <drivers/pinmux.h>

static Pinmux_PerCfg_t gPinMuxMainDomainCfg[] = {
            /* I2C0 pin config */
        /* I2C0_SCL -> I2C0_SCL (D17) */
    {
        PIN_I2C0_SCL,
        ( PIN_MODE(0) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
        /* I2C0_SDA -> I2C0_SDA (E16) */
    {
        PIN_I2C0_SDA,
        ( PIN_MODE(0) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },

            /* MCASP2 pin config */
        /* MCASP2_AFSR -> GPMC0_AD14 (T20) */
    {
        PIN_GPMC0_AD14,
        ( PIN_MODE(3) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
        /* MCASP2_ACLKR -> GPMC0_AD15 (T21) */
    {
        PIN_GPMC0_AD15,
        ( PIN_MODE(3) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
        /* MCASP2_AFSX -> GPMC0_AD12 (T22) */
    {
        PIN_GPMC0_AD12,
        ( PIN_MODE(3) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
        /* MCASP2_ACLKX -> GPMC0_AD13 (R21) */
    {
        PIN_GPMC0_AD13,
        ( PIN_MODE(3) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
    /* AUDIO_EXT_REFCLK1 -> GPMC0_WPn (K17) */
    {
        PIN_GPMC0_WPN,
        ( PIN_MODE(1) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },

            /* MCASP2 pin config */
        /* MCASP2_AXR0 -> GPMC0_AD8 (P22) */
    {
        PIN_GPMC0_AD8,
        ( PIN_MODE(3) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
            /* MCASP2 pin config */
        /* MCASP2_AXR14 -> GPMC0_CSn0 (M19) */
    {
        PIN_GPMC0_CSN0,
        ( PIN_MODE(3) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },

        
    {PINMUX_END, 0U}
};

static Pinmux_PerCfg_t gPinMuxMcuDomainCfg[] = {
        
        
                
            /* MCU_USART0 pin config */
        /* MCU_UART0_RXD -> MCU_UART0_RXD (D8) */
    {
        PIN_MCU_UART0_RXD,
        ( PIN_MODE(0) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
        /* MCU_UART0_TXD -> MCU_UART0_TXD (F8) */
    {
        PIN_MCU_UART0_TXD,
        ( PIN_MODE(0) | PIN_PULL_DISABLE )
    },

    {PINMUX_END, 0U}
};

/*
 * Pinmux
 */
void Pinmux_init(void)
{
    Pinmux_config(gPinMuxMainDomainCfg, PINMUX_DOMAIN_ID_MAIN);
    Pinmux_config(gPinMuxMcuDomainCfg, PINMUX_DOMAIN_ID_MCU);
}

