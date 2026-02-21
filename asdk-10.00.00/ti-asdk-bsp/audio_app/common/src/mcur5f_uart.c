
/*
 *  Copyright (C) 2018-2022 Texas Instruments Incorporated
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
#include <kernel/dpl/DebugP.h>
#include <kernel/dpl/ClockP.h>
#include "ti_drivers_config.h"
#include "ti_board_config.h"
#include "ti_drivers_open_close.h"
#include "ti_board_open_close.h"
#include "FreeRTOS.h"
#include "task.h"

#include <platform.h>

extern SemaphoreP_Object semDataReady;
extern SemaphoreP_Object semWriteDone;

#define UART_BUFFER_SIZE (MAX_COMMAND_BUFFER_LEN*sizeof(UINT32)*2)

UART_Transaction trans_rx;
UART_Transaction trans_tx;
UART_Handle uartHandle = NULL;
uint32_t read_byte_cnt = 0;
uint8_t TuningBuffer[UART_BUFFER_SIZE];
extern uint32_t AWE_Packet_Buffer[MAX_COMMAND_BUFFER_LEN];
uint8_t gUartRx_data;

int32_t awe_uart_receive_byte();

void am62ax_uart_setup()
{
    uartHandle = gUartHandle[CONFIG_UART_CONSOLE]; 

    // submit first 1byte buffer for receive
    awe_uart_receive_byte();   
}

int32_t awe_uart_transmit(void* buf, uint32_t len)
{
    int32_t transferOK = SystemP_SUCCESS;

    UART_Transaction_init(&trans_tx);
    trans_tx.buf   = buf;
    trans_tx.count = len;

    if(uartHandle != NULL)
    {
        transferOK = UART_write(uartHandle, &trans_tx);
        DebugP_assert(SystemP_SUCCESS == transferOK);
    }
    
    return transferOK;
}

int32_t awe_uart_receive_byte()
{
    int32_t transferOK = SystemP_SUCCESS;

    UART_Transaction_init(&trans_rx);
    trans_rx.buf   = &gUartRx_data;
    trans_rx.count = 1;
    if(uartHandle != NULL)
    {
        transferOK = UART_read(uartHandle, &trans_rx);
        DebugP_assert(SystemP_SUCCESS == transferOK);
    }
    return transferOK;
}

void awe_uart_write_callback(UART_Handle handle, UART_Transaction *trans)
{
	read_byte_cnt = 0;
	SemaphoreP_post(&semWriteDone);
}

void awe_uart_read_callback(UART_Handle handle, UART_Transaction *trans)
{
    read_byte_cnt++;
    SemaphoreP_post(&semDataReady);
}

/**
  * @brief  UART send reply
  * @param  None
  * @retval None
  */
void awe_uart_sendreply(uint32_t bufSize)
{
	uint32_t count;
	int32_t  nPacketState;
	
	for(count = 0; count < UART_BUFFER_SIZE; count++)
	{
		nPacketState = tuningEncodeByteUART(AWE_Packet_Buffer, &TuningBuffer[count]);
        if (nPacketState == COMPLETE_NEW_PACKET)
            break;
	}
	
	if(count > 0) {
		//DebugP_log("Uart Send : %d bytes\r\n", count);
        awe_uart_transmit(TuningBuffer, count);
    } else {
		SemaphoreP_post(&semWriteDone);
	}
 
}   
