/*
 *  Copyright (C) 2023 Texas Instruments Incorporated
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

#include <kernel/dpl/DebugP.h>
#include <kernel/dpl/AddrTranslateP.h>
#include <kernel/dpl/ClockP.h>
#include <kernel/dpl/DebugP.h>
#include <drivers/i2c.h>
#include <drivers/gpio.h>
#include <drivers/mcasp.h>
#include <board/ioexp/ioexp_tca6424.h>
#include "ti_drivers_config.h"
#include "ti_drivers_open_close.h"
#include "ti_board_open_close.h"

#include "targetInfo.h"
#include "aic31_regdefs.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* Audio buffer settings */
#define APP_MCASP_AUDIO_BUFF_COUNT  (4U)
#define APP_MCASP_AUDIO_BUFF_SIZE   (2048U)

#if defined (SOC_AM62AX)
/* AM62Ax CODEC I2C address */
#define APP_MCASP_CODEC_ADDR    (0x1BU)

/* I2C address for IO expander */
#define IO_EXP_ADDR             (0x22U)

/* Codec reset pin for I/O expander */
#define IO_EXP_CODEC_RESET_PIN  (0x8U)
#endif

uint8_t i2cAddr = APP_MCASP_CODEC_ADDR;

/* ========================================================================== */
/*                        Static Function Declaration                         */
/* ========================================================================== */
/* Reset codec */
void am62ax_skevm_codec_reset(void);
/* I2C register write for Codec */
int32_t I2C_writeReg(I2C_Handle handle, uint8_t reg, uint8_t val);
/* I2C register read for Codec */
int32_t I2C_readReg(I2C_Handle handle, uint8_t reg, uint8_t *val);

/* Configure codec TLV320AIC31 */
//static void am62ax_skevm_aic31_codec_config(void);

/**
*
*  @brief   This sets the AIC PLL values based on the output sampling rate.  The
*           assumption is PLLCLK_IN is BCLK and the input to BCLK is properly
*           pre-configured for these values.  AIC PLL output frequency is calculated as:
*               fs(ref) = (PLLCLK_IN × K × R) / (2048 × P), where
*               P = 1, 2, 3,?, 8
*               R = 1, 2, ?, 16
*               K = J.D
*               J = 1, 2, 3, ?, 63
*               D = 0000, 0001, 0002, 0003, ?, 9998, 9999
*
*  @param   outSampRate     output sampling rate
*           pllPValue       PLL P Value
*           pllRValue       PLL R Value
*           pllJValue       PLL J Value
*           pllDLowValue    Six least significant bits of a 14-bit unsigned integer for D value
*           pllDHighValue   Eight most significant bits of a 14-bit unsigned integer for D value
*           dualRate        ADC/DAC Dual rate control
*           fsRef           fS(ref) setting
*           sampleRateDiv   ADC/DAC Sample Rate Select
*/
void AicPllValueInit(uint32_t outSampRate,
                            int *pllPValue,
                            int *pllRValue,
                            int *pllJValue,
                            int *pllDLowValue,
                            int *pllDHighValue,
                            int *dualRate,
                            int *fsRef,
                            int *sampleRateDiv)
{
    switch (outSampRate)
    {
        /* Set PLL for 48kHz */
        case 48000:
        {
            *pllPValue     = 1;
            *pllRValue     = 1;
            *pllJValue     = 32;
            
            *pllDLowValue  = 0x0;
            *pllDHighValue = 0x0;
            *dualRate      = 0x0;
            *fsRef         = 0x0;
            *sampleRateDiv = 0x0;
            
            break;
        }
    }
}


int32_t I2C_writeRegBf(I2C_Handle handle, uint8_t regAddr, 
            uint8_t bfPosition, uint8_t bfSize, uint8_t bfData)
{
    int32_t status = 0;
    uint8_t regData = 0; /* Temporary Register data                */
    uint8_t mask = 0;    /* Bit field mask                         */
    uint8_t bFalseWhile = 0;

    do
    {
        /* Read the current value of the register                             */
        status = I2C_readReg(handle, regAddr, &regData);
        if (0 != status){
            break;
        }

        /* Update the value of the bit field                                  */
        mask = ((1 << bfSize) - 1) << bfPosition;
        regData = regData & ~(mask);
        regData = regData | (bfData << bfPosition);
        /* Write the data back into the register                              */

        status = I2C_writeReg(handle, regAddr, regData);
    } while (bFalseWhile);

    /* Return the result of writing data into the bit field                   */
    return (status);
}


void AIC31_config(uint32_t outSampRate)
{
#if (INSTANCE_ID == TI_AWE_PRIMARY_CORE)

    int32_t status = SystemP_SUCCESS;
    I2C_Handle  i2cHandle;

    i2cHandle = gI2cHandle[CONFIG_I2C0];

    status = I2C_probe(i2cHandle, i2cAddr);
    DebugP_assert(status == SystemP_SUCCESS);

    int32_t pllPValue     = 1;
    int32_t pllRValue     = 1;
    int32_t pllJValue     = 32;
    int32_t pllDLowValue  = 0x00;
    int32_t pllDHighValue = 0x00;
    int32_t dualRate      = 0;
    int32_t fsRef         = 0x01;
    int32_t sampleRateDiv = 0x00;

    /* Set AIC PLL parameters based on output sampling rate */
    AicPllValueInit(outSampRate,
                    &pllPValue,
                    &pllRValue,
                    &pllJValue,
                    &pllDLowValue,
                    &pllDHighValue,
                    &dualRate,
                    &fsRef,
                    &sampleRateDiv);

    /* Set page 0 as the active page for the following register accesses */
    I2C_writeReg(i2cHandle, Aic31_PAGE_0, 0x00);

    /* self clearing software reset */
    I2C_writeReg(i2cHandle, Aic31_P0_REG1, 0x80);

    I2C_writeRegBf(i2cHandle, Aic31_P0_REG8,6,2,3); // AIC in MASTER MODE 

    I2C_writeRegBf(i2cHandle, Aic31_P0_REG9, 4, 2, 3);

    /* DAC sample rate select to fs(ref)/1 */
    I2C_writeRegBf(i2cHandle, Aic31_P0_REG2, 0, 4, sampleRateDiv);

    /* DAC dual rate mode is disabled */
    I2C_writeRegBf(i2cHandle, Aic31_P0_REG7, 5, 1, dualRate);

    /* Enable the PLL */
    I2C_writeRegBf(i2cHandle, Aic31_P0_REG3, 7, 1, 1);

    /* Select BCLK as the input source to derive MCLK */
    I2C_writeRegBf(i2cHandle, Aic31_P0_REG102, 4, 2, 2);

    /* select the PLL_IN as codec clock input                          */
    I2C_writeRegBf(i2cHandle, Aic31_P0_REG101, 0, 1, 1);

    /* selec the GPIO to output the divided PLL_IN clock(test purpose)*/
    I2C_writeReg(i2cHandle, Aic31_P0_REG98, 0x20);

    /* Write to PLL programming register A (P=1)                      */
    I2C_writeRegBf(i2cHandle, Aic31_P0_REG3, 0, 3, pllPValue);

    /* Write to PLL programming register B (J=32)                     */
    I2C_writeRegBf(i2cHandle, Aic31_P0_REG4, 2, 6, pllJValue);

    /* write the high and low bits of the D value (D=0)                 */
    I2C_writeReg(i2cHandle, Aic31_P0_REG5, pllDHighValue);
    I2C_writeReg(i2cHandle, Aic31_P0_REG6, pllDLowValue << 2);

    /* Select PLL R value from Audio Codec Overflow Flag Register (R=1) */
    I2C_writeRegBf(i2cHandle, Aic31_P0_REG11, 0, 4, pllRValue);

    /* Write to codec datapath setup register to set fs(ref) to 48kHz       */
    I2C_writeRegBf(i2cHandle, Aic31_P0_REG7, 7, 0, fsRef);

    /* Set right audio data to right channel and left audio data to left channel */
    I2C_writeRegBf(i2cHandle, Aic31_P0_REG7, 1, 4, 5);

    /* Unmute left PGA and set gain */
    I2C_writeRegBf(i2cHandle, Aic31_P0_REG15, 7, 1, 0);

    /* Unmute right PGA and set gain */
    I2C_writeRegBf(i2cHandle, Aic31_P0_REG16, 7, 1, 0);

    /* Set level control gain to Left ADC */
    I2C_writeRegBf(i2cHandle, Aic31_P0_REG17, 0, 4, 0);

    /* Set level control gain to right ADC */
    I2C_writeRegBf(i2cHandle, Aic31_P0_REG18, 0, 4, 0);

    /* Set Mic bias voltage = 2.0V */
    I2C_writeRegBf(i2cHandle, Aic31_P0_REG25, 6, 2, 1);

    /* Set level control gain for Left ADC */
    I2C_writeReg(i2cHandle, Aic31_P0_REG19, 0xFC);

    /* Set level control gain for Right ADC */
    I2C_writeReg(i2cHandle, Aic31_P0_REG22, 0xFC);

    /* power up the left and right DACs, HPLCOM configured as independent single-ended output */
    I2C_writeReg(i2cHandle, Aic31_P0_REG37, 0xE0);

    /* select the DAC L1 R1 Paths                                             */
    /* Right DAC volume follows the left channel control register             */
    I2C_writeReg(i2cHandle, Aic31_P0_REG41, 0x02);
    /* Driver power-on time = 100 ms, Driver ramp-up step time = 4 ms         */
    I2C_writeReg(i2cHandle, Aic31_P0_REG42, 0x6C);

    /* unmute the DAC, L/R gain = 0 dB                                        */
    I2C_writeReg(i2cHandle, Aic31_P0_REG43, 0x00);
    I2C_writeReg(i2cHandle, Aic31_P0_REG44, 0x00);

    /* DAC L1 to HPL OUT Is connected                                         */
    I2C_writeReg(i2cHandle, Aic31_P0_REG47, 0x80);
    /* HPLOUT is not muted, HPLOUT is fully powered up                        */
    I2C_writeReg(i2cHandle, Aic31_P0_REG51, 0x09);

    /* DAC R1 to HPROUT is connected                                          */
    I2C_writeReg(i2cHandle, Aic31_P0_REG64, 0x80);
    /* HPROUT is not muted, HPROUT is fully powered up                        */
    I2C_writeReg(i2cHandle, Aic31_P0_REG65, 0x09);

    /* DAC_L1 is routed to LEFT_LOP/M                                         */
    I2C_writeReg(i2cHandle, Aic31_P0_REG82, 0x80);
    /* LEFT_LOP/M is not muted, LEFT_LOP/M is fully powered up                */
    I2C_writeReg(i2cHandle, Aic31_P0_REG86, 0x09);

    /* DAC_R1 is routed to RIGHT_LOP/M                                        */
    I2C_writeReg(i2cHandle, Aic31_P0_REG92, 0x80);
    /* RIGHT_LOP/M is not muted, RIGHT_LOP/M is fully powered up              */
    I2C_writeReg(i2cHandle, Aic31_P0_REG93, 0x09);

    /* L/R gain = 0 dB                                                        */
    I2C_writeReg(i2cHandle, Aic31_P0_REG43, 0);
    I2C_writeReg(i2cHandle, Aic31_P0_REG44, 0);

#endif //#if (INSTANCE_ID == PRIMARY_CORE)
}


int32_t I2C_writeReg(I2C_Handle handle, uint8_t reg,
                                    uint8_t val)
{
    int32_t status = SystemP_SUCCESS;
    I2C_Transaction i2cTransaction;
    uint8_t txBuffer[2];

    I2C_Transaction_init(&i2cTransaction);
    i2cTransaction.writeBuf   = txBuffer;
    i2cTransaction.writeCount = 2;
    i2cTransaction.slaveAddress = i2cAddr;
    txBuffer[0] = reg;
    txBuffer[1] = val;
    status = I2C_transfer(handle, &i2cTransaction);
    return status;
}

int32_t I2C_readReg(I2C_Handle handle, uint8_t reg,
                                    uint8_t *val)
{
    int32_t status = SystemP_SUCCESS;
    I2C_Transaction i2cTransaction;
    uint8_t txBuffer = reg;
    uint8_t rxBuffer[2];

    I2C_Transaction_init(&i2cTransaction);
    i2cTransaction.writeBuf   = &txBuffer;
    i2cTransaction.writeCount = 1;
    i2cTransaction.readBuf    = rxBuffer;
    i2cTransaction.readCount  = 2;
    i2cTransaction.slaveAddress = i2cAddr;    
    status = I2C_transfer(handle, &i2cTransaction);
    if(status == SystemP_SUCCESS) {
        *val = rxBuffer[0];
    } else {
        *val = 0;
    }

    return status;
}

void am62ax_skevm_codec_reset(void)
{
#if (INSTANCE_ID == TI_AWE_PRIMARY_CORE)
    int32_t status = SystemP_SUCCESS;

    TCA6424_Params TCA6424_IOexp_params =
	{
		.i2cInstance = 0,
		.i2cAddress = IO_EXP_ADDR
	};

    TCA6424_Config TCA6424_IOexp_config;

    status = TCA6424_open(&TCA6424_IOexp_config, &TCA6424_IOexp_params);

    if(status == SystemP_SUCCESS)
    {
        status = TCA6424_config(&TCA6424_IOexp_config, IO_EXP_CODEC_RESET_PIN,
                                    TCA6424_MODE_OUTPUT);
    }

    if(status == SystemP_SUCCESS)
    {
        status = TCA6424_setOutput(&TCA6424_IOexp_config, IO_EXP_CODEC_RESET_PIN,
                                    TCA6424_OUT_STATE_LOW);
    }

    /* Wait for codec to reset */
    //ClockP_usleep(1);

    if(status == SystemP_SUCCESS)
    {
        status = TCA6424_setOutput(&TCA6424_IOexp_config, IO_EXP_CODEC_RESET_PIN,
                                    TCA6424_OUT_STATE_HIGH);
    }
#endif //#if (INSTANCE_ID == TI_AWE_PRIMARY_CORE)
}

int32_t configure_codec(void)
{
    int32_t status = SystemP_SUCCESS;
#if (INSTANCE_ID == TI_AWE_PRIMARY_CORE)
    am62ax_skevm_codec_reset();
    AIC31_config(48000);
#endif //#if (INSTANCE_ID == TI_AWE_PRIMARY_CORE)
    return status;
}

