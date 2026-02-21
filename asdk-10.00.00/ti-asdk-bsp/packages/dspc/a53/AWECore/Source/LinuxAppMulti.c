/****************************************************************************
 *
 *        Simple Multi-Instance AWECore Linux Application
 *        -----------------------------------------------
 *
 ****************************************************************************
 *
 *    Description:    Sample app to show basic usage of AWECore's multi-instance features
 *         A multi-instance AWECore integration with simulated real-time audio on Linux. 
 *        Includes an ethernet/TCPIP tuning interface to connect to AWE Server on port 15092
 *        Each instance of AWECore shares a tuning packet buffer and shared heap.
 *        AWE Instance 1 is connected to ethernet interface and will internally signal Instance 2 to process packets when needed.
 *        AWE Instance 2 polls continuously for packets in its own thread. 
 *        AWE Instance 2 is triggered to pump by AWE Instance 1, which is in turn triggered by simulated real-time audio callback.
 *
 *    Copyright: (c) 2023    DSP Concepts, Inc. All rights reserved.
 *                                    3235 Kifer Road
 *                                    Santa Clara, CA 95054-1527
 *
 ***************************************************************************/
/** @example LinuxAppMulti.c

 * 
 * Examples */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <signal.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sched.h>
#include <math.h>
#include <time.h>
#include <semaphore.h>
#include <signal.h>
#include <time.h>
#include <semaphore.h>

#include "AWECore.h"
#include "ProxyIDs.h"
#include "AWECoreUtils.h"
#include "TargetInfo.h"

typedef struct ConfigParameters
{
	float coreSpeed;
	float profileSpeed;
	float sampleRate;
	UINT32 fundamentalBlockSize;
	UINT32 inChannels;
	UINT32 outChannels;
	UINT32 profileStatus;
	INT32 tuningPort;
} ConfigParameters;

// Config parameters to control from app arguments
static ConfigParameters configParams; 

// AWB with 2 instances used in layout
const unsigned int loopback_dualcore_startAudio_InitCommands[] = {
	0x0002000c, 0x00060006, 0x473b8000, 0x40008002, 0x00000020, 0x00000000, 0x00060006, 0x473b8000,
	0x40008002, 0x00000020, 0x00000000, 0x00060006, 0x473b8000, 0x40008002, 0x00000020, 0x00000000,
	0x00060106, 0x473b8000, 0x40008002, 0x00000020, 0x00000000, 0x00040007, 0x00000001, 0x00000001,
	0x00040007, 0x00000002, 0x00000002, 0x0009000f, 0xbeef0866, 0x00000101, 0x00000002, 0x00000001,
	0x00000003, 0x00000001, 0x00000000, 0x0008000f, 0xbeef1037, 0x00000001, 0x00000002, 0x00000003,
	0x00000000, 0x00000001, 0x0007000f, 0xbeef086d, 0x00000001, 0x00000001, 0x00000003, 0x00000000,
	0x00080065, 0x0000680a, 0x00000000, 0x00000400, 0x00000002, 0xc0a00000, 0x40a00000, 0x0008010f,
	0xbeef1038, 0x00000100, 0x00000002, 0x00000001, 0x00000000, 0x00000000, 0x0008010f, 0xbeef1037,
	0x00000001, 0x00000002, 0x00000001, 0x00000000, 0x00000001, 0x0007010f, 0xbeef086d, 0x00000001,
	0x00000001, 0x00000001, 0x00000000, 0x00080165, 0x0000480a, 0x00000000, 0x00000400, 0x00000002,
	0xc0a00000, 0x40a00000, 0x0008000f, 0xbeef1038, 0x00000100, 0x00000002, 0x00000003, 0x00000000,
	0x00000000, 0x0007000f, 0xbeef086d, 0x00000001, 0x00000001, 0x00000003, 0x00000000, 0x00080065,
	0x0000880a, 0x00000000, 0x00000400, 0x00000002, 0xc0a00000, 0x40a00000, 0x0009000f, 0xbeef0866,
	0x00000101, 0x00000002, 0x00000003, 0x00000002, 0x00000000, 0x00000001, 0x00040010, 0x00000006,
	0x00000001, 0x000a002f, 0x0000000a, 0x00000000, 0x00000004, 0x00000005, 0x00000006, 0x00000007,
	0x00000008, 0x00000009, 0x00040110, 0x00000003, 0x00000001, 0x0007012f, 0x00000005, 0x00000000,
	0x00000002, 0x00000003, 0x00000004, 0x0006013c, 0x00002008, 0x00000008, 0x00000100, 0x00000000,
	0x0006003c, 0x00007008, 0x0000008c, 0x00000100, 0x00000000, 0x0002001b, 0x00000000,
};

const unsigned int loopback_dualcore_startAudio_InitCommands_Len = sizeof(loopback_dualcore_startAudio_InitCommands) / sizeof(int);	// 143 words

// Don't load AWB on startup, to avoid compatibility issues with prebuilt C array above
#define LOAD_MULTICORE_AWB 0
#define NUM_INSTANCES 3

// Table of AWE Instances used for multi-instance applications
AWEInstance *pInstances[NUM_INSTANCES];

static IOPinDescriptor aweInputPin;
static IOPinDescriptor aweOutputPin;

// LISTOFCLASSOBJECTS comes from ModuleList.h .. shared by both instances
const void* module_descriptor_table[] =
{
    LISTOFCLASSOBJECTS
};

// Allocate shared heap buffer, used by all instances
UINT32 sharedHeap[SHARED_HEAP_SIZE_LINUX];

// Universal packet buffer used by all AWE instances
UINT32 tuningPacketBuffer[MAX_COMMAND_BUFFER_LEN];
UINT32 tuningReplyBuffer[MAX_COMMAND_BUFFER_LEN];

// Audio buffers
UINT32 *audioInputBuffer;
UINT32 *audioOutputBuffer;

// Global thread handles and mutexes
pthread_t packetProcessHandles[NUM_INSTANCES];
pthread_t audioCallbackThreadHandle;
pthread_t audioPumpAllThreadHandles[NUM_INSTANCES];
sem_t pumpSems[NUM_INSTANCES];

INT32 exitAudioCallbackThread;
INT32 audioStarted;
unsigned long long callbackCnt;
unsigned long long pumpCnts[NUM_INSTANCES];
INT32 quiet;

// Tuning parameters
int sockfd, newsockfd;
int exitTuning = 0;
#define DEFAULT_TUNING_PORT             (15092)
#define DEFAULT_BLOCK_SIZE              (128)   // Reasonable block size to simulate real time callbacks!!!

// increase priority levels for AWE threads
// Audio callback functions must be highest priority to avoid
// missing real time requirements. Pump is right after,
// followed by tuning interface where actions can be performed
// in the background.
#define AUDIO_CALLBACK_PRIO     (99)
#define AUDIO_PUMP_PRIO         (AUDIO_CALLBACK_PRIO - 1)
#define TUNING_THREAD_PRIO      (AUDIO_PUMP_PRIO - NUM_INSTANCES)

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void sig_handler(int signo)
{    
    if (signo == SIGINT)
    {
        // Cought user termination signal
        exitTuning = 1;
        shutdown(newsockfd, SHUT_RDWR);
        close(newsockfd);    
        shutdown(sockfd, SHUT_RDWR);
        close(sockfd);    
        
        if (!quiet)
        {
            printf("Exiting LinuxAppMulti\n");
        }
        exit(0);
    }
}

/**
 * @brief Report program usage.
 * @param [in] program				the program name
 */
static void usage(const char *program)
{
	printf(
		"Usage: %s [args]\n"
	    "       -sr:sampling_rate           value in Hz, default 48KHz\n"
		"       -pf:profile_frequency       value in Hz, default 10MHz\n"
		"       -cf:cpu_frequency           value in Hz, default 1GHz\n"
		"       -profStatus:N               set profiling status (0 - disable, 1 - enable(default), 2 - enable module level only, 3 - enable top level only)\n"
		"       -bsize:N                    default 32, Audio block size for the system\n"
		"       -inchans:N                  default 2, number of input channels\n"
		"       -outchans:N                 default 2, number of output channels\n"
		"       -tport:N                    default 15092, port number for socket interface. User can choose between 15002 - 15098\n"
		"       -quiet                      print only error messages\n"        
        "       -h, -help                   show this message\n"
		"This program exercises the AWECore library with 2 AWEInstances.\n",
		program);
		exit(0);
}


void* tuningPacketThread(void* arg)
{
    /*
        Tuning packets can be recieved and processed here. Another implementation 
        may choose to process the packet in another thread.
    */
    (void) arg;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    INT32 schedPolicy, ret;
    struct sched_param schedParam;     
    int socketOption;
    
    // Set this thread to run at real time priority
    pthread_t currentHandle = pthread_self();
    schedParam.sched_priority = TUNING_THREAD_PRIO;
    schedPolicy = SCHED_RR;
    ret = pthread_setschedparam(currentHandle, schedPolicy, &schedParam);
    if (ret != 0)
    {
        printf("Failed to increase priority of tuning thread with error: %s \nTry running with sudo\n", strerror(ret));
    }

    char threadNameBuffer[16]; 
    snprintf(threadNameBuffer, 16, "awe0_packet");
    ret = pthread_setname_np(packetProcessHandles[0], threadNameBuffer);
    if (ret)
    {
        printf("Failed to set thread name to %s\n", threadNameBuffer);
    }
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
    {
        error("ERROR opening socket");
    }
    
    // Set the socket connection option to immediately allow reuse of port upon losing connection
    socketOption = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &socketOption, sizeof(socketOption));    
    
    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(configParams.tuningPort);
    
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
    {
        error("ERROR on binding");
    }
    
    
    while (!exitTuning)
    {
        listen(sockfd,5);
        if (!quiet)
        {
            printf("Listening for connection on port %d\n", configParams.tuningPort);
        }
        clilen = sizeof(cli_addr);
        newsockfd = accept(sockfd, 
                     (struct sockaddr *) &cli_addr, 
                     &clilen);

        if (newsockfd < 0) 
        {
            error("ERROR on accept");
        }


        /* Enter a tuning loop. This will be in a separate thread from audio processing */
        if (!quiet)
        {        
            printf( "Found connection!\n");
        }
        
        // Setup sleep time for 5 ms
        struct timespec ts;
        ts.tv_nsec = 5000000;
        ts.tv_sec = 0;
        
        while(1)
        {
            unsigned int plen;
            ssize_t readBytes, n;

            /* Only read from socket if multi-instance packet API isn't waiting on a response from another AWE instance */
            if (ret != E_MULTI_PACKET_WAITING)
            {
                readBytes = read(newsockfd, tuningPacketBuffer, MAX_COMMAND_BUFFER_LEN * sizeof(tuningPacketBuffer[0]));
                if (readBytes == 0)
                {
                    // No message to read
                    break;
                }
                else if (readBytes < 0) 
                {
                    error("ERROR reading from socket");
                }

                // May not have received the whole message. Read again if not
                plen = PACKET_LENGTH_BYTES(tuningPacketBuffer);
                while (readBytes < plen)
                {
                    //printf("Didn't read the entire packet! readBytes = %d, plen = %d\nReading again\n", readBytes, plen);
                    n = read(newsockfd,&((char*)tuningPacketBuffer)[readBytes], MAX_COMMAND_BUFFER_LEN * sizeof(tuningPacketBuffer[0]));
                    if (n == 0)
                    {
                        break;
                    }
                    else if (n < 0) 
                    {
                        error("ERROR reading from socket");
                    }
                    else 
                    {
                        readBytes += n;
                    }
                }
            }

            // Process the received message (may involve signaling instance 2 to process packet)
            ret = awe_packetProcessMulti(pInstances[0], TRUE); 

            // Send reply packet back to PC if multi-instance packet API is not waiting on a response packet
            if (ret != E_MULTI_PACKET_WAITING)
            {
                plen = PACKET_LENGTH_WORDS(tuningReplyBuffer);

                n = write(newsockfd, tuningReplyBuffer, plen * sizeof(tuningReplyBuffer[0]));
                
                if (n < plen)
                {
                    error("ERROR writing to socket");
                }
            }
            else
            {
                // Sleep while waiting for other instances to process packets
                clock_nanosleep(CLOCK_MONOTONIC, 0, &ts, NULL);
            }
        }
    }
    return NULL;
}

void* packetProcessThread(void * args)
{
    AWEInstance * pAWE = (AWEInstance *) args;
    INT32 instanceId = pAWE->instanceId;
    INT32 schedPolicy, ret;
    struct sched_param schedParam;
    struct timespec ts;
    
    // Set this thread to run at real time priority
    pthread_t currentHandle = pthread_self();
    schedParam.sched_priority = TUNING_THREAD_PRIO - instanceId;
    schedPolicy = SCHED_RR;
    ret = pthread_setschedparam(currentHandle, schedPolicy, &schedParam);
    if (ret != 0)
    {
        printf("Failed to increase priority of tuning thread %d with error: %s \nTry running with sudo\n", instanceId, strerror(ret));
    }

    char threadNameBuffer[16]; 
    snprintf(threadNameBuffer, 16, "awe%d_packet", instanceId);
    pthread_setname_np(packetProcessHandles[instanceId], threadNameBuffer);
    if (ret)
    {
        printf("Failed to set thread name to %s\n", threadNameBuffer);
    }
    
    if (!quiet)
    {
        printf("Starting instance %d packet process thread\n", instanceId);
    }
    
    // Setup sleep time for 5 ms
    ts.tv_nsec = 5000000;
	ts.tv_sec = 0;
    while(1)
    {
        // constantly poll packet process thread to respond to packets received on instance 1
        awe_packetProcessMulti(pAWE, FALSE);
        clock_nanosleep(CLOCK_MONOTONIC, 0, &ts, NULL);
    }
    return NULL;
}


void* aweuser_pumpAudio(void * args)
{
    // Represents the callback function from the audio framework
    INT32 pumpMask;
    INT32 schedPolicy, ret, targetPriority;
    struct sched_param schedParam;
    
    AWEInstance * pAWE = (AWEInstance *) args;
    INT32 instanceId = pAWE->instanceId;
    
    // Set this thread to run at real time priority
    pthread_t currentHandle = pthread_self();
    targetPriority = AUDIO_PUMP_PRIO - instanceId;
    schedParam.sched_priority = targetPriority;
    schedPolicy = SCHED_FIFO;
    pthread_setschedparam(currentHandle, schedPolicy, &schedParam);
	int policy;
	pthread_getschedparam(currentHandle, &policy, &schedParam);
	if ((SCHED_FIFO != policy) || (schedParam.sched_priority != targetPriority))
	{
		printf("Warning: Failed to increase priority of audio simulation thread to %d\nTry running as root\n", targetPriority);
	}
    
    char threadNameBuffer[16]; 
    snprintf(threadNameBuffer, 16, "awe%d_pump", instanceId);    
    ret = pthread_setname_np(audioPumpAllThreadHandles[instanceId], threadNameBuffer);


    if (!quiet)
    {
        printf("Starting pump thread for instance %d\n", instanceId);
    }
    while (1)
    {
        // Wait for signal to run thread
        sem_wait(&pumpSems[instanceId]);
        if (awe_layoutIsValid(pAWE) && awe_audioIsStarted(pAWE))
        {
            if (instanceId == 0)
            {   
                // Import new samples only for layout 0
                for (int i = 0; i < configParams.inChannels ; i++)
                {
                    // Import new samples
                    ret = awe_audioImportSamples(pAWE, &audioInputBuffer[i], configParams.inChannels, i, Sample32bit);
                    if (ret)
                    {
                        printf("Error: awe_audioImportSamples() failed, channel = %d, error = %d\n", i, ret);
                    }
                }
            }
            
            // Pump any required sublayouts
            pumpMask = awe_audioGetPumpMask(pAWE);
			
			if (instanceId == 0)
            {
                // Instance 1 has to signal all other instances to run
                for (int ix = 1; ix < NUM_INSTANCES; ix++)
                {
					if (awe_audioIsReadyToPumpMulti(pAWE, ix))
					{
						sem_post(&pumpSems[ix]);
					}
                }
            }

            // Each sublayout should be pumped in its own thread, 
            // but running all sequentially here for simplicity. This does not
            // propertly handle sublayouts at higher clockDividers, as all sublayouts
            // must complete at layout input blockrate.
            for (int j = 0; j < NUM_THREADS; j++)
            {
                if (pumpMask & (1U << j))
                {
                    // Pump the sublayout. 
                    ret |= awe_audioPump(pAWE, j);
                }
            }
            // Do deferred processing if needed
            if (ret > 0) 
            {
                // This could be done in a loop until ret == 0, but doing one per fundamental period 
                // is more conservative at the cost of potentially taking longer to update a module parameter
                ret = awe_deferredSetCall(pAWE);
            }
            
            if (instanceId == 0)
            {
                // Export samples
                for (int i = 0; i < configParams.outChannels ; i++)
                {
                    // Import new samples
                    ret = awe_audioExportSamples(pAWE, &audioOutputBuffer[i], configParams.outChannels, i, Sample32bit);
                    if (ret)
                    {  
                        printf("Error: awe_audioExportSamples() failed, channel = %d, error = %d\n", i, ret);
                    }
                }
            }
            pumpCnts[instanceId]++;            
        } // if layout valid
    }
    return NULL;
}

void* audioCallbackSimulator(void * args)
{
    // Simple real-time audio simulator. Would be ALSA, PortAudio, etc callback
    // Attempts to get accurate pumping rate on average by correction for overshoot
    // caused by timer resolution
    struct timespec ts;
	struct timespec ts_tmp;
	long time_nsec;
	long long accumulated_time1;
	long long accumulated_time2;
	long long overshoot = 0.0; 
    
    (void) args;
    INT32 schedPolicy, ret;
    struct sched_param schedParam;
    
    // Setup sleep time for fundamental blocksize
    time_nsec = (long) ((float)1000000000L * ((float)configParams.fundamentalBlockSize / configParams.sampleRate));
	ts.tv_sec = 0;
	ts_tmp.tv_sec = 0;
	ts_tmp.tv_nsec = 0;
    
    // Set this thread to run at real time priority
    pthread_t currentHandle = pthread_self();
    schedParam.sched_priority = AUDIO_CALLBACK_PRIO;
    schedPolicy = SCHED_FIFO;
    pthread_setschedparam(currentHandle, schedPolicy, &schedParam);

	int policy;
	pthread_getschedparam(currentHandle, &policy, &schedParam);
	if ((SCHED_FIFO != policy) || (schedParam.sched_priority != AUDIO_CALLBACK_PRIO)) 
	{
		printf("Warning: Failed to increase priority of audio simulation thread to %d\nTry running as root\n", AUDIO_CALLBACK_PRIO);
	}
    
    char threadNameBuffer[32]; 
    snprintf(threadNameBuffer, 32, "awe_callback");
    ret = pthread_setname_np(audioCallbackThreadHandle, threadNameBuffer);
    if (ret)
    {
        printf("Failed to set thread name to %s\n", threadNameBuffer);
    }    

    while (!exitAudioCallbackThread)
    {
        callbackCnt++;

        // Kick off the primary audio thread
        sem_post(&pumpSems[0]);
        
        // Subtract any overshoot from the last sleep to balance block rate
        // Will there be a chance of overshoot > time_nsec ???
		ts.tv_nsec = time_nsec - overshoot;
		clock_gettime(CLOCK_MONOTONIC, &ts_tmp);
		accumulated_time1 = ((long long)ts_tmp.tv_sec*1000000000) + ts_tmp.tv_nsec + time_nsec - overshoot;
		
		while(1)
		{
			clock_nanosleep(CLOCK_MONOTONIC, 0, &ts, NULL);
			clock_gettime(CLOCK_MONOTONIC, &ts_tmp);
			accumulated_time2 = ((long long)ts_tmp.tv_sec*1000000000) + ts_tmp.tv_nsec;
			if (accumulated_time2 >= accumulated_time1)
			{
				overshoot = (accumulated_time2 - accumulated_time1);
				break;
			}
			else
			{
				ts.tv_nsec = (long)(accumulated_time1 - accumulated_time2);
			}
		}
    }
    return NULL;
}

INT32 aweuser_audioStart(AWEInstance *pAWE)
{
    // Audio start callback. Can be used to start audio stream from audio framework.
    // Could be ALSA, Pulse, Jack, Portaudio, etc. 

    // Kick off real-time audio simulation thread, instance 0 is the driver
    if (!audioStarted && pAWE->instanceId == 0)
    {
        exitAudioCallbackThread = 0;
        callbackCnt = 0;
        memset(pumpCnts, 0, sizeof(pumpCnts));

        INT32 ret = pthread_create(&audioCallbackThreadHandle, NULL, audioCallbackSimulator, NULL);     
        if (ret)
        {
            printf("Error creating audio callback simulator! ret = %d\n", ret);
            return 0;
        }

        audioStarted = 1;
        if (!quiet)
        {
            printf("Audio Started\n");
        }
    }        
    return 0;
}

INT32 aweuser_audioStop(AWEInstance *pAWE)
{
    // Audio stop callback. Can be used to clean up audio stream.
    if (audioStarted)
    {
        INT32 ret;
        exitAudioCallbackThread = 1;
        ret = pthread_join(audioCallbackThreadHandle, NULL);
        if (ret != 0)
        {
            printf("Failed to join audio simulator thread: ret = %d\n", ret);
        }
        
        if (!quiet)
        {
			UINT32 avgCycles;
			const FLOAT32 clockRatios = configParams.coreSpeed / configParams.profileSpeed;
            printf("\nAudio Stopped\n");
            
            for (int i = 0; i < NUM_INSTANCES; i++)
            {
                ret = awe_getAverageLayoutCycles(pInstances[i], 0, &avgCycles);
                if (ret != E_SUCCESS)
                {
                    avgCycles = 0;
                }
                printf("-- Pumped instance %d %llu times, average cycles per pump: %f\n", i, pumpCnts[i], 
                    (avgCycles >> 8) * clockRatios);
            }
        }
        audioStarted = 0;
    }
    return 0;
}

void InitializeAWEInstance(AWEInstance * awePtr, INT32 instanceId)
{
    int ret = 0;
    UINT32 module_descriptor_table_size = sizeof(module_descriptor_table) / sizeof(module_descriptor_table[0]);

    // Set pointers to universal AWE packet buffer, same buffer for all instances
    awePtr->pPacketBuffer = tuningPacketBuffer;
    awePtr->pReplyBuffer = tuningReplyBuffer;
    awePtr->packetBufferSize = MAX_COMMAND_BUFFER_LEN;
    
    awePtr->pModuleDescriptorTable = module_descriptor_table;
    awePtr->numModules = module_descriptor_table_size;

    awePtr->numThreads = NUM_THREADS;
    awePtr->sampleRate = configParams.sampleRate;
    awePtr->fundamentalBlockSize = configParams.fundamentalBlockSize;
    awePtr->pFlashFileSystem = HAS_FLASHFILESYSTEM;

    awePtr->fastHeapASize = FASTA_HEAP_SIZE_LINUX;
    awePtr->fastHeapBSize = FASTB_HEAP_SIZE_LINUX;
    awePtr->slowHeapSize = SLOW_HEAP_SIZE_LINUX;
    awePtr->sharedHeapSize = SHARED_HEAP_SIZE_LINUX;
    
    awePtr->pFastHeapA = malloc(FASTA_HEAP_SIZE_LINUX * sizeof(UINT32));
    awePtr->pFastHeapB = malloc(FASTB_HEAP_SIZE_LINUX * sizeof(UINT32));
    awePtr->pSlowHeap = malloc(SLOW_HEAP_SIZE_LINUX * sizeof(UINT32));
    awePtr->pSharedHeap = sharedHeap;

    awePtr->coreSpeed = configParams.coreSpeed;
    awePtr->profileSpeed = configParams.profileSpeed;
    // Give each instance a unique name
    awePtr->pName = malloc(sizeof(char) * 8);
    sprintf((char*)awePtr->pName, "awe#%d", instanceId);
    
    awePtr->instanceId = instanceId;
    // Need to set this value for all instances
    awePtr->numProcessingInstances = NUM_INSTANCES;
    awePtr->cbAudioStart = aweuser_audioStart;
    awePtr->cbAudioStop = aweuser_audioStop;


    if (instanceId == 0)
    {
        int i, j;
        // Only instance 0 deals with inputs and outputs,
        // so only this instance needs pins and audio start/stop
		awePtr->pInputPin = &aweInputPin;
		awePtr->pOutputPin = &aweOutputPin;        
        ret = awe_initPin(&aweInputPin, configParams.inChannels, NULL);
        if (ret != 0)
        {
            printf("awe_initPin inputPin failed on AWEInstance 0\n");
        }
        ret = awe_initPin(&aweOutputPin, configParams.outChannels, NULL);
        if (ret != 0)
        {
            printf("awe_initPin outputPin failed on AWEInstance 0\n");
        }

#ifndef PI
#define PI 3.141592653589793
#endif 
        // Fill audio input buffers with full scale sin waves
        float blockRate = configParams.sampleRate / configParams.fundamentalBlockSize;
        for (i = 0; i < configParams.fundamentalBlockSize; i++)
        {
            for (j = 0; j < configParams.inChannels; j++) 
            {
                audioInputBuffer[i * configParams.inChannels + j] = float_to_fract32(sinf(2.f * PI * 
                    (j + 1) * blockRate * (i / configParams.sampleRate)));
            }
        }
    }

    ret = awe_init(awePtr);
    if (ret != 0) 
    {
        printf("awe_init on instance %d failed with %d\n", instanceId, ret);
    }
    else
    {
        if (!quiet)
        {
            printf("Initialized instance %d\n", instanceId);
        }
    }

}


int main( int argc, const char* argv[] )
{
    int i;

    configParams.coreSpeed = CORE_SPEED;
    configParams.profileSpeed = PROFILE_SPEED;
    configParams.sampleRate = SAMPLE_RATE;
    configParams.fundamentalBlockSize = DEFAULT_BLOCK_SIZE;
    configParams.inChannels = NUM_INPUT_CHANNELS;
    configParams.outChannels = NUM_OUTPUT_CHANNELS;
    configParams.tuningPort = DEFAULT_TUNING_PORT;
    configParams.profileStatus = 1;
    quiet = 0;

	// Handle user inputs
	for (i = 1; i < argc; i++)
	{
		const char *arg = argv[i];
	
		if (0 == strncmp(arg, "-profStatus:", 12))
		{
			configParams.profileStatus = atoi(arg + 12);
		}
		else if (0 == strncmp(arg, "-bsize:", 7))
		{
			configParams.fundamentalBlockSize = atoi(arg + 7);
		}
		else if (0 == strncmp(arg, "-inchans:", 9))
		{
			configParams.inChannels = atoi(arg + 9);
		}
		else if (0 == strncmp(arg, "-outchans:", 10))
		{
			configParams.outChannels = atoi(arg + 10);
		}
		else if(0 == strncmp(arg, "-sr:", 4))
		{
			configParams.sampleRate = (float)atof(arg + 4);
		}			
		else if (0 == strncmp(arg, "-cf:", 4))
		{
			configParams.coreSpeed = (float)atof(arg + 4);
		}
		else if (0 == strncmp(arg, "-pf:", 4))
		{
			configParams.profileSpeed = (float)atof(arg + 4);
		}
		else if (0 == strncmp(arg, "-tport:", 7))
		{
			configParams.tuningPort = atoi(arg + 7);
		}
		else if (0 == strncmp(arg, "-quiet", 6))
		{
			quiet = 1;
		}
		else if ((0 == strncmp(arg, "-help", 5)) || (0 == strncmp(arg, "-h", 2)))
		{
			usage(argv[0]);
		}
	}
	
	if (!quiet)
	{
		printf("\nAWECore initialised with the following parameters\n");
		printf("-cf:            %f Hz\n", configParams.coreSpeed);
		printf("-pf:            %f Hz\n", configParams.profileSpeed);
		printf("-sr:            %f Hz\n", configParams.sampleRate);
		printf("-bsize:         %u\n", configParams.fundamentalBlockSize);
		printf("-inchans:       %u\n", configParams.inChannels);
		printf("-outchans:      %u\n", configParams.outChannels);
		printf("-tport:         %d\n", configParams.tuningPort);
		printf("-profStatus:    %u\n", configParams.profileStatus);
	}    
    
    // Setup signal handler to kill sockets
    if (signal(SIGINT, sig_handler) == SIG_ERR)
    {
        printf("Can't catch SIGINT (%d)\n", SIGINT);
    }

    // Allocate audio buffers used by instance 0
    audioInputBuffer  = malloc(configParams.inChannels  * configParams.fundamentalBlockSize * sizeof(UINT32));
    audioOutputBuffer = malloc(configParams.outChannels * configParams.fundamentalBlockSize * sizeof(UINT32));
        
    // Allocate the instances needed and then initialize
    for (i = 0; i < NUM_INSTANCES; i++)
    {
        pInstances[i] = calloc(1, sizeof(AWEInstance));
        InitializeAWEInstance(pInstances[i], i);
        awe_setProfilingStatus(pInstances[i], configParams.profileStatus);
    }

    // Register instance table, required when using multiple AWE instances on a single-core (application dependent on Linux)
    awe_setInstancesInfo(pInstances, NUM_INSTANCES);

    // Start tuning thread for each instance
    pthread_create(&packetProcessHandles[0], NULL, tuningPacketThread, NULL);
    for (i = 1; i < NUM_INSTANCES; i++)
    {
        pthread_create(&packetProcessHandles[i], NULL, packetProcessThread, pInstances[i]); 
    }
    
    // Start audio pump for all instances
    for (i = 0; i < NUM_INSTANCES; i++)
    {
        sem_init(&pumpSems[i], 0, 0);
        pthread_create(&audioPumpAllThreadHandles[i], NULL, aweuser_pumpAudio, pInstances[i]);
    }    
    
    if (LOAD_MULTICORE_AWB)
    {
        UINT32 position;
        INT32 ret = awe_loadAWBfromArray(pInstances[0], loopback_dualcore_startAudio_InitCommands, loopback_dualcore_startAudio_InitCommands_Len, &position);
        if (ret != E_SUCCESS)
        {
            printf("Error loading awb: error = %d, at offset %u\n", ret, position);
            exit(1);
        }
        else 
        {
            printf("Loaded multi-instance awb\n");
        }
    }
    
    // Close it all down once ready
    for (i = 0; i < NUM_INSTANCES; i++)
    {
        pthread_join(packetProcessHandles[i], NULL);
        pthread_join(audioPumpAllThreadHandles[i], NULL);
    }    
    
    free(audioInputBuffer);
    free(audioOutputBuffer);
    for (i = 0; i < NUM_INSTANCES; i++)
    {
        free(pInstances[i]->pFastHeapA);
        free(pInstances[i]->pFastHeapB);
        free(pInstances[i]->pSlowHeap);
        free((void*)pInstances[i]->pName);
        free(pInstances[i]);
    }

    return 0;
}
