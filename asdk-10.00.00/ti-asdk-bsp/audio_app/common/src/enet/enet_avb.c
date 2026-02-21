/*
 *  Copyright (c) Texas Instruments Incorporated 2022-23
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

/*!
 * \file  enet_setup.c
 *
 * \brief This file contains Enet intialization
 *        
 */

/* ========================================================================== */
/*                              Include Files                                 */
/* ========================================================================== */


#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "FreeRTOS.h"
#include <kernel/dpl/TaskP.h>
#include <task.h>
#include "tsn_gptp/gptpmasterclock.h"
#include "kernel/dpl/HwiP.h"
#include "kernel/dpl/TimerP.h"
#include "kernel/dpl/SemaphoreP.h"
#include "ti_dpl_config.h"

#include <tsn_combase/combase.h>
#include <tsn_unibase/unibase_binding.h>
#include <tsn_uniconf/yangs/yang_db_runtime.h>
#include <tsn_uniconf/yangs/yang_modules.h>
#include "tsn_conl2/aaf_avtpc_listener.h"
#include "tsn_conl2/aaf_avtpc_talker.h"
#include <tsn_l2/tilld/frtos_avtp_include.h>
#include <tsn_uniconf/ucman.h>
#include <tsn_uniconf/uc_dbal.h>

/* lwIP core includes */
#include "lwip/opt.h"
#include "lwip/sys.h"
#include "lwip/tcpip.h"
#include "lwip/dhcp.h"

#include "ti_board_config.h"
#include "ti_board_open_close.h"
#include "ti_drivers_open_close.h"

#include "enet_setup.h"
#include "platform.h"


SemaphoreP_Object gTimerSem;

extern EnetApp_Cfg gEnetAppCfg;

static uint8_t gListnerStackBuf[8U * 1024U] \
__attribute__ ((aligned(32U)));

static uint8_t gTalkerStackBuf[8U * 1024U] \
__attribute__ ((aligned(32U)));

uint8_t g_avb_rxbuf[SHM_AVB_DATA_RX_SIZE] \
	__attribute__((section(".asdk_avb_rx_buffer")));

uint8_t g_avb_txbuf[SHM_AVB_DATA_TX_SIZE] \
	__attribute__((section(".asdk_avb_tx_buffer")));

#define  AVB_BUF_CMN_SIZE   3072

uint8_t g_data[1000U];
uint32_t g_Countwrite = 0U;
volatile uint16_t avbDataSize;

/*
#define BLOCKSIZE 				(256)
#define CHANNEL_CNT				(2)
#define SAMPLES_CNT				(BLOCKSIZE*CHANNEL_CNT)
#define AVB_SAMPLE_SIZE			(2)
#define NUM_BUFS                (4)
#define AVB_RX_BUF_SIZE			(SAMPLES_CNT*AVB_SAMPLE_SIZE)

uint8_t av_buf[NUM_BUFS][AVB_RX_BUF_SIZE] \
   __attribute__((section(".avb_shm_mem")));

*/

typedef struct {
    void (*avtpc_close)(void *avtpc_talker);
    int (*avtpc_send)(void *avtpc_talker, uint64_t pts,
              uint8_t *audio_data, uint32_t data_size);
    void *avtpc_talker;
    int read_size;
    int sent_bytes;
    uint32_t sent_packets;
} audio_talker_t;

typedef struct {
    void (*avtpc_close)(void *avtpc_listener);
    void *avtpc_listener; /* aaf or iec61883_6 */
} audio_listener_t;

audio_talker_t audio_talker;
audio_listener_t audio_listener;

typedef struct {
    audio_talker_t audio_talker;
    audio_listener_t audio_listener;
    conl2_basic_conparas_t bcp;
    int audio_samples; //number of audio samples per AVTP packet
    char* netdev;
} conl2_data_t;

static int audio_aaf_avtp_push_packet(uint8_t *payload, int plsize, avbtp_rcv_cb_info_t *cbinfo, void *cbdata);

static void audio_aaf_avtpc_talker_close(void *avtpc_talker)
{
    if (avtpc_talker) aaf_avtpc_talker_close(avtpc_talker);
}

static void audio_aaf_avtpc_listener_close(void *audio_listener)
{
    if (audio_listener) aaf_avtpc_listener_close(audio_listener);
}

static int audio_aaf_listener_init(conl2_basic_conparas_t* basic_param, aaf_avtpc_pcminfo_t* pcm_info)
{
    audio_listener.avtpc_listener = aaf_avtpc_listener_init(basic_param, audio_aaf_avtp_push_packet, &audio_listener);
    if (!audio_listener.avtpc_listener) {
        DPRINT("%s:failed to open aaf listener\n",__func__);
        return -1;
    }
    audio_listener.avtpc_close=audio_aaf_avtpc_listener_close;
    return 0;
}

static int audio_aaf_avtpc_talker_send(void *avtpc_talker, uint64_t pts,
                    uint8_t *audio_data, uint32_t data_size)
{
    return aaf_avtpc_talker_write(avtpc_talker, pts, audio_data, data_size);
}

static int audio_aaf_talker_init(conl2_basic_conparas_t* basic_param, aaf_avtpc_pcminfo_t* pcm_info)
{
    audio_talker.avtpc_talker = aaf_avtpc_talker_init(basic_param);
    if (!audio_talker.avtpc_talker) {
        DPRINT("%s:failed to open aaf talker\n",__func__);
        return -1;
    }
    aaf_avtpc_set_pcm_info(audio_talker.avtpc_talker, pcm_info);

    audio_talker.avtpc_close = audio_aaf_avtpc_talker_close;
    audio_talker.avtpc_send = audio_aaf_avtpc_talker_send;

    return 0;
}

static void init_default_params_talker(conl2_basic_conparas_t* basic_param, aaf_avtpc_pcminfo_t* pcm_info)
{
    basic_param->vid = 110;
    basic_param->send_ahead_ts = 20000; // ahead 20ms
    ub_streamid_t streamid = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x00, 0x02};
    memcpy(basic_param->streamid, streamid, sizeof(streamid));

    ub_macaddr_t mcastmac = {0x91, 0xE0, 0xF0, 0x00, 0xFE, 0x00};
    memcpy(basic_param->mcastmac, mcastmac, sizeof(mcastmac));

    basic_param->time_intv = 250;
    basic_param->max_frm_sz = 1500;
    basic_param->max_intv_frames = 1;
    basic_param->pcp = 2;
    basic_param->avtpd_bufftime_us = 10000;
    basic_param->is_direct = true;
    

    pcm_info->format = AVBTP_AAF_FORMAT_INT_16BIT;
    pcm_info->bit_depth = AVB_AUDIO_SAMPLE_SIZE*8;
    pcm_info->channels = AVB_TX_CHANNEL_COUNT;
    pcm_info->srate = 48000;
}

static void init_default_params_listener(conl2_basic_conparas_t* basic_param, aaf_avtpc_pcminfo_t* pcm_info)
{
    basic_param->vid = 110;
    basic_param->send_ahead_ts = 20000; // ahead 20ms
    ub_streamid_t streamid = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x00, 0x02};;
    memcpy(basic_param->streamid, streamid, sizeof(streamid));

    ub_macaddr_t mcastmac = {0x91, 0xE0, 0xF0, 0x00, 0xFE, 0x00};
    memcpy(basic_param->mcastmac, mcastmac, sizeof(mcastmac));

    basic_param->time_intv = 250;
    basic_param->max_frm_sz = 1500;
    basic_param->max_intv_frames = 1;
    basic_param->pcp = 2;
    basic_param->avtpd_bufftime_us = 10000;
    basic_param->is_direct = true;


    pcm_info->format = AVBTP_AAF_FORMAT_INT_16BIT;
    pcm_info->bit_depth = AVB_AUDIO_SAMPLE_SIZE*8;
    pcm_info->channels = AVB_RX_CHANNEL_COUNT;
    pcm_info->srate = 48000;
}


static int audio_aaf_avtp_push_packet(uint8_t *payload, int plsize,
                avbtp_rcv_cb_info_t *cbinfo, void *cbdata)
{
    static int total_rx_frames = 0;
    audio_listener_t *audio_listener = (audio_listener_t *)cbdata;
    ShdMemStatus    shmStatus;
    if (!audio_listener) {return -1;}

    avbtp_sd_info_t *rsdinfo=&cbinfo->u.rsdinfo;
    aaf_avtpc_pcminfo_t pcminfo;
    if (rsdinfo->u.aaf.format == AVBTP_AAF_FORMAT_AES3_32BIT)
    {
        UB_LOG(UBL_INFO,"%s unexpected rx format %d\n", __func__, rsdinfo->u.aaf.format);
        return -1;
    }

    if (aaf_avtpc_get_pcm_info(&cbinfo->u.rsdinfo, &pcminfo) == 0)
    {
        shmStatus = shm_write(gEnetAppCfg.hShmListenerQ, payload, plsize);
        if(shmStatus != SHDMEM_STATUS_OK)
        {
            if (shmStatus == SHDMEM_STATUS_ERR_FULL)
            {
                DebugP_log("%s:SHM buffer full \n",__func__);
            }
            else
            {
                DebugP_log("%s:SHM buffer error %d \n",__func__, shmStatus);
            }
        }
        else
        {
            g_Countwrite++;
        }

        /// 48000 frames print one time
        int rx_frames = plsize / (pcminfo.bit_depth / 8) / pcminfo.channels;
        total_rx_frames += rx_frames;
        if (total_rx_frames >= 48000)
        {
            UB_LOG(UBL_INFO,"[RX] pcminfo bdepth[%d] channels[%d] format[%d] srate[%d]\n",
                         pcminfo.bit_depth,
                         pcminfo.channels,
                         pcminfo.format,
                         pcminfo.srate);
            total_rx_frames = 0;
        }
    }

    return 0;
}

static int32_t start_aaf_pcm_listener(char* netdev)
{
    conl2_basic_conparas_t basic_param;
    aaf_avtpc_pcminfo_t pcm_info;

    memset(&basic_param, 0, sizeof(basic_param));
    memset(&pcm_info, 0, sizeof(pcm_info));
    memcpy(basic_param.netdev, netdev, strlen(netdev));
    init_default_params_listener(&basic_param, &pcm_info);

    while(gptpmasterclock_init(NULL)){
            UB_LOG(UBL_INFO,"Waiting for tsn_gptpd to be ready...\n");
            CB_USLEEP(100000);
        }
    // wait for semaphore
    //SemaphoreP_pend(&gEnetAppCfg.startListnerSemObj, SystemP_WAIT_FOREVER);
    
    // initialize the avtp connection device
    
    if (audio_aaf_listener_init(&basic_param, &pcm_info) == -1) return -1;
    
    return 0;
}

int start_aaf_pcm_talker(char* netdev)
{
    uint64_t pts;
    uint64_t sts, ets, tsdiff; // start and end ts, use to print report
    conl2_basic_conparas_t basic_param;
    aaf_avtpc_pcminfo_t pcm_info;
    int32_t status;

    memset(&basic_param, 0, sizeof(basic_param));
    memset(&pcm_info, 0, sizeof(pcm_info));
    memcpy(basic_param.netdev, netdev, strlen(netdev));
    init_default_params_talker(&basic_param, &pcm_info);

    status = SemaphoreP_constructBinary(&gTimerSem, 0);
    DebugP_assert(SystemP_SUCCESS == status);
    TimerP_start(gTimerBaseAddr[CONFIG_TIMER0]);

    while(gptpmasterclock_init(NULL)){
        UB_LOG(UBL_INFO,"Waiting for tsn_gptpd to be ready...\n");
        CB_USLEEP(100000);
    }

    // 1ms => buffer size = 48 (frames per 1ms) * 1 (ms) * 2 (16 bit-depth) * 2 (channels) = 960 bytes

    if (audio_aaf_talker_init(&basic_param, &pcm_info) == -1) return -1;

    int pcmbuf_size = 48 * AVB_AUDIO_SAMPLE_SIZE * AVB_TX_CHANNEL_COUNT; // 960 bytes
    
    sts = ub_mt_gettime64();

    // repeat: start from 0 and read until last sync frame

    while(true)
    {
        uint8_t* audio_buf;
        uint16_t pcmBufReadSize = pcmbuf_size;

        //wait for timer trigger
        SemaphoreP_pend(&gTimerSem, SystemP_WAIT_FOREVER);

        // read the data from shared memory
        shm_readBufPtr(gEnetAppCfg.hShmTalkerQ, &audio_buf, &pcmBufReadSize);


        pts = gptpmasterclock_getts64();
        if (audio_talker.avtpc_send( audio_talker.avtpc_talker,
                                pts + basic_param.send_ahead_ts,
                                audio_buf,
                                pcmbuf_size) < 0)
        {
            return -1;
        }
        audio_talker.sent_packets++;
        audio_talker.sent_bytes += pcmbuf_size;

        ets = ub_mt_gettime64();
        tsdiff = ets - sts;
        if (tsdiff >= UB_SEC_NS) 
        {
            UB_LOG(UBL_INFO,"audio talker sent: %f(packets/sec), %f(mbps)\n",
                (float)audio_talker.sent_packets/((float)tsdiff/(float)UB_SEC_NS),
                (float)(audio_talker.sent_bytes * 8000.0f)/(float)tsdiff);
            audio_talker.sent_packets = 0;
            audio_talker.sent_bytes = 0;

            sts = ets;
        }
    }

    return 0;
}

void asdk_createShmbuffer()
{
    uint32_t rxSize         = AVB_BUF_CMN_SIZE; //AVB_RX_FRAME_SIZE;
    uint32_t txSize         = AVB_BUF_CMN_SIZE; //AVB_TX_FRAME_SIZE;
    uint32_t shmOvrHd       = shm_metadata_overhead();
    uint32_t rxShmBufSize   = ((SHM_AVB_DATA_RX_SIZE - shmOvrHd)/rxSize) * rxSize + shmOvrHd;
    uint32_t txShmBufSize   = ((SHM_AVB_DATA_TX_SIZE - shmOvrHd)/txSize) * txSize + shmOvrHd;

    DebugP_assert(rxShmBufSize < SHM_AVB_DATA_RX_SIZE);
    DebugP_assert(txShmBufSize < SHM_AVB_DATA_TX_SIZE);

    gEnetAppCfg.hShmListenerQ = shm_create((uint32_t)g_avb_rxbuf, rxShmBufSize);
    gEnetAppCfg.hShmTalkerQ = shm_create((uint32_t)g_avb_txbuf, txShmBufSize);
}

static void EnetApp_TalkerTask(void *arg)
{
    int32_t         status = SystemP_SUCCESS;

#ifndef AVTP_DIRECT_MODE
    WAIT_AVTPD_READY;
#endif

   status = start_aaf_pcm_talker("tilld0");
   DebugP_assert(status == 0U);
}

static void EnetApp_ListenerTask(void *arg)
{
    int32_t         status = SystemP_SUCCESS;

#ifndef AVTP_DIRECT_MODE
    WAIT_AVTPD_READY;
#endif

   status = start_aaf_pcm_listener("tilld0");
   DebugP_assert(status == 0U);
   while(true)
   {
       CB_USLEEP(100000);
       ub_log_flush();
   }
}

void asdk_createListenerTask()
{
    TaskP_Params    taskParams;
    int32_t         status = SystemP_SUCCESS;

    TaskP_Params_init(&taskParams);
    taskParams.priority       = 2U;
    taskParams.stack          = gListnerStackBuf;
    taskParams.stackSize      = sizeof(gListnerStackBuf);
    taskParams.args           = NULL;
    taskParams.name           = "AVB Listener Task";
    taskParams.taskMain       = &EnetApp_ListenerTask;

    status = TaskP_construct(&gEnetAppCfg.listenerTaskObj, &taskParams);
    DebugP_assert(SystemP_SUCCESS == status);
}

void asdk_createTalkerTask()
{
    TaskP_Params    taskParams;
    int32_t         status = SystemP_SUCCESS;

    TaskP_Params_init(&taskParams);
    taskParams.priority       = 2U;
    taskParams.stack          = gTalkerStackBuf;
    taskParams.stackSize      = sizeof(gTalkerStackBuf);
    taskParams.args           = NULL;
    taskParams.name           = "AVB Talker Task";
    taskParams.taskMain       = &EnetApp_TalkerTask;

    status = TaskP_construct(&gEnetAppCfg.talkerTaskObj, &taskParams);
    DebugP_assert(SystemP_SUCCESS == status);
}

void avbTimerIsr(void)
{
    SemaphoreP_post(&gTimerSem);
}
