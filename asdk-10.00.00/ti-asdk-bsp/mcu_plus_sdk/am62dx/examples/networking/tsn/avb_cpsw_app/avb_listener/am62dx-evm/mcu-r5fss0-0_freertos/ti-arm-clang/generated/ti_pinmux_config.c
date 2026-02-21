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

            /* CP_GEMAC_CPTS0 pin config */
    /* CP_GEMAC_CPTS0_TS_SYNC -> MMC1_DAT2 (C22) */
    {
        PIN_MMC1_DAT2,
        ( PIN_MODE(1) | PIN_PULL_DISABLE )
    },
    /* MDIO0 pin config */
    /* MDIO0_MDC -> MDIO0_MDC (V12) */
    {
        PIN_MDIO0_MDC,
        ( PIN_MODE(0) | PIN_PULL_DISABLE )
    },
    /* MDIO0_MDIO -> MDIO0_MDIO (V13) */
    {
        PIN_MDIO0_MDIO,
        ( PIN_MODE(0) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
    /* RGMII2 pin config */
    /* RGMII2_RD0 -> RGMII2_RD0 (AA21) */
    {
        PIN_RGMII2_RD0,
        ( PIN_MODE(0) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
    /* RGMII2_RD1 -> RGMII2_RD1 (Y20) */
    {
        PIN_RGMII2_RD1,
        ( PIN_MODE(0) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
    /* RGMII2_RD2 -> RGMII2_RD2 (AB21) */
    {
        PIN_RGMII2_RD2,
        ( PIN_MODE(0) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
    /* RGMII2_RD3 -> RGMII2_RD3 (AB20) */
    {
        PIN_RGMII2_RD3,
        ( PIN_MODE(0) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
    /* RGMII2_RXC -> RGMII2_RXC (AA20) */
    {
        PIN_RGMII2_RXC,
        ( PIN_MODE(0) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
    /* RGMII2_RX_CTL -> RGMII2_RX_CTL (W18) */
    {
        PIN_RGMII2_RX_CTL,
        ( PIN_MODE(0) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
    /* RGMII2_TD0 -> RGMII2_TD0 (AA19) */
    {
        PIN_RGMII2_TD0,
        ( PIN_MODE(0) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
    /* RGMII2_TD1 -> RGMII2_TD1 (Y18) */
    {
        PIN_RGMII2_TD1,
        ( PIN_MODE(0) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
    /* RGMII2_TD2 -> RGMII2_TD2 (AA18) */
    {
        PIN_RGMII2_TD2,
        ( PIN_MODE(0) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
    /* RGMII2_TD3 -> RGMII2_TD3 (W17) */
    {
        PIN_RGMII2_TD3,
        ( PIN_MODE(0) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
    /* RGMII2_TXC -> RGMII2_TXC (AB19) */
    {
        PIN_RGMII2_TXC,
        ( PIN_MODE(0) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
    /* RGMII2_TX_CTL -> RGMII2_TX_CTL (Y19) */
    {
        PIN_RGMII2_TX_CTL,
        ( PIN_MODE(0) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
    /* RGMII1 pin config */
    /* RGMII1_RD0 -> RGMII1_RD0 (AB16) */
    {
        PIN_RGMII1_RD0,
        ( PIN_MODE(0) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
    /* RGMII1_RD1 -> RGMII1_RD1 (V15) */
    {
        PIN_RGMII1_RD1,
        ( PIN_MODE(0) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
    /* RGMII1_RD2 -> RGMII1_RD2 (W15) */
    {
        PIN_RGMII1_RD2,
        ( PIN_MODE(0) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
    /* RGMII1_RD3 -> RGMII1_RD3 (V14) */
    {
        PIN_RGMII1_RD3,
        ( PIN_MODE(0) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
    /* RGMII1_RXC -> RGMII1_RXC (AA16) */
    {
        PIN_RGMII1_RXC,
        ( PIN_MODE(0) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
    /* RGMII1_RX_CTL -> RGMII1_RX_CTL (AA15) */
    {
        PIN_RGMII1_RX_CTL,
        ( PIN_MODE(0) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
    /* RGMII1_TD0 -> RGMII1_TD0 (Y17) */
    {
        PIN_RGMII1_TD0,
        ( PIN_MODE(0) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
    /* RGMII1_TD1 -> RGMII1_TD1 (V16) */
    {
        PIN_RGMII1_TD1,
        ( PIN_MODE(0) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
    /* RGMII1_TD2 -> RGMII1_TD2 (Y16) */
    {
        PIN_RGMII1_TD2,
        ( PIN_MODE(0) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
    /* RGMII1_TD3 -> RGMII1_TD3 (AA17) */
    {
        PIN_RGMII1_TD3,
        ( PIN_MODE(0) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
    /* RGMII1_TXC -> RGMII1_TXC (AB17) */
    {
        PIN_RGMII1_TXC,
        ( PIN_MODE(0) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
    },
    /* RGMII1_TX_CTL -> RGMII1_TX_CTL (W16) */
    {
        PIN_RGMII1_TX_CTL,
        ( PIN_MODE(0) | PIN_INPUT_ENABLE | PIN_PULL_DISABLE )
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

