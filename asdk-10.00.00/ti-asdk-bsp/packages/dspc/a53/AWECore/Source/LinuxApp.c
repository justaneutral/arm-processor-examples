/****************************************************************************
 *
 *		Simple AWECore Linux Application
 *		---------------------
 *
 ****************************************************************************
 *
 *	Description:	Sample app to show basic usage of AWECore
 * 		A single instance AWECore integration with simulated real-time audio on Linux. 
 *		Includes an ethernet/TCPIP tuning interface to connect to AWE Server on port 15092
 *
 *		This is a low latency enabled example, meaning that there can either be 0 or 1 blocks of latency based on certain conditions...
 *			-If the user loads a layout with a blocksize which does NOT equal the AWEInstance's fundamental BS, there will be 1 block of latency.
 * 			-If the user's layout blocksize matches the AWEInstance's fundamental blocksize, then there will be 0 blocks of latency
 *
 *		See the Latency section of the docs for more info. 
 *
 *	Copyright: (c) 2020	DSP Concepts, Inc. All rights reserved.
 *									3235 Kifer Road
 *									Santa Clara, CA 95054-1527
 *
 ***************************************************************************/
/** @example LinuxApp.c

 * 
 * Examples */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <signal.h>
#include <netinet/in.h>
#include <pthread.h>
#include <math.h>
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

// Input/output audio buffers
INT32 *audioInputBuffer = NULL;
INT32 *audioOutputBuffer = NULL;

static BOOL quiet = FALSE;

// Global AWEInstance structure and pins
AWEInstance aweInstance;

static IOPinDescriptor aweInputPin;
static IOPinDescriptor aweOutputPin;

// LISTOFCLASSOBJECTS comes from ModuleList.h
const void* module_descriptor_table[] =
{
	LISTOFCLASSOBJECTS
};

// Global packet buffers
UINT32 awePacketBuffer[MAX_COMMAND_BUFFER_LEN];
UINT32 awePacketBufferReply[MAX_COMMAND_BUFFER_LEN];

// Global heaps (dynamically allocated based on the size argument
UINT32 *fastHeapA;
UINT32 *fastHeapB;
UINT32 *slowHeap;
UINT32 heapSizeFastA = FASTA_HEAP_SIZE_LINUX;
UINT32 heapSizeFastB = FASTB_HEAP_SIZE_LINUX;
UINT32 heapSizeSlow = SLOW_HEAP_SIZE_LINUX;


// Global thread handles and mutexes
pthread_t tuningThreadHandle;
pthread_t processThreadHandle;
pthread_t audioCallbackThreadHandle;
pthread_t audioPumpAllThreadHandle;

pthread_mutex_t packetMutex;

sem_t pumpSem;

INT32 audioStarted = 0;
INT32 exitAudioCallbackThread = 0;
// Tuning parameters
int sockfd, newsockfd;
int exitTuning = 0;

INT32 pumpActive = 0;

// increase priority levels for AWE threads
// Audio callback functions must be highest priority to avoid
// missing real time requirements. Pump is right after,
// followed by tuning interface where actions can be performed
// in the background.
#define AUDIO_CALLBACK_PRIO		9
#define AUDIO_PUMP_PRIO			8
#define TUNING_THREAD_PRIO		7

/**
 * @brief Report program usage.
 * @param [in] program				the program name
 */
static void usage(const char *program)
{
	printf(
		"Usage: %s [args]\n"
		"       -load:<file>                 AWB file to load\n"
	    "       -sr:sampling_rate            value in Hz, default 48KHz\n"
		"       -pf:profile_frequency        value in Hz, default 10MHz\n"
		"       -cf:cpu_frequency            value in Hz, default 1GHz\n"
		"       -profStatus:N                set profiling status (0 - disable, 1 - enable(default), 2 - enable module level only, 3 - enable top level only)\n"
		"       -bsize:N                     default 32, Audio block size for the system\n"
		"       -inchans:N                   default 2, number of input channels\n"
		"       -outchans:N                  default 2, number of output channels\n"
		"       -tport:N                     default 15002, port number for socket interface. User can choose between 15002 - 15098\n"
		"       -hsizefasta:N                fast A heap size in words, default %d\n"
		"       -hsizefastb:N                fast B heap size in words, default %d\n"
		"       -hsizeslow:N                 slow heap size in words, default %d\n"
		"       -quiet                       works in quiet mode, displays only necessary messages\n"
		"This program exercises the AWECore library.\n",
		program, FASTA_HEAP_SIZE_LINUX, FASTB_HEAP_SIZE_LINUX, SLOW_HEAP_SIZE_LINUX);
		exit(0);
}

void destroyApp(void)
{
	free(audioInputBuffer);
	free(audioOutputBuffer);
	free(fastHeapA);
	free(fastHeapB);
	free(slowHeap);
	audioInputBuffer = NULL;
	audioOutputBuffer = NULL;
	fastHeapA = NULL;
	fastHeapB = NULL;
	slowHeap = NULL;
}

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
		
		// Wait until thread ends
		pthread_join(tuningThreadHandle, NULL);
		// Destroy any app specific
		destroyApp();
	}
    printf("Exiting LinuxApp\n");
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
	schedPolicy = SCHED_FIFO;
	ret = pthread_setschedparam(currentHandle, schedPolicy, &schedParam);
	if (ret != 0)
	{
		printf("Failed to increase priority of tuning thread with error: %s \nTry running with sudo\n", strerror(ret));
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
		printf("Listening for connection on port %d\n", configParams.tuningPort);
		clilen = sizeof(cli_addr);
		newsockfd = accept(sockfd, 
					 (struct sockaddr *) &cli_addr, 
					 &clilen);

		if (newsockfd < 0) 
		{
			error("ERROR on accept");
		}
		  

		/* Enter a tuning loop. This will be in a separate thread from audio processing */
		printf( "Found connection!\n");
		while(1)
		{
			unsigned int plen;
			ssize_t readBytes, n;
			
			readBytes = read(newsockfd, aweInstance.pPacketBuffer, MAX_COMMAND_BUFFER_LEN * sizeof(aweInstance.pPacketBuffer[0]));
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
			plen = PACKET_LENGTH_BYTES(aweInstance.pPacketBuffer);
			while (readBytes < plen)
			{
				//printf("Didn't read the entire packet! readBytes = %d, plen = %d\nReading again\n", readBytes, plen);
				n = read(newsockfd,&((char*)aweInstance.pPacketBuffer)[readBytes], MAX_COMMAND_BUFFER_LEN * sizeof(aweInstance.pPacketBuffer[0]));
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
			
			// Don't process the packet if audio is pumping
			pthread_mutex_lock(&packetMutex);
			awe_packetProcess(&aweInstance);	
			pthread_mutex_unlock(&packetMutex);		


			plen = PACKET_LENGTH_WORDS(aweInstance.pReplyBuffer);
			n = write(newsockfd, aweInstance.pReplyBuffer, plen * sizeof(aweInstance.pReplyBuffer[0]));
			
			if (n < plen)
			{
				error("ERROR writing to socket");
			}
		}
	}
	return NULL;
}



void* aweuser_pumpAllAudio(void * args)
{
	// Represents the callback function from the audio framework
	(void) args;
	INT32 pumpMask;
	INT32 ix;
	INT32 i;
	INT32 schedPolicy, ret;
	struct sched_param schedParam;	 	
	
	// Set this thread to run at real time priority
	pthread_t currentHandle = pthread_self();
	schedParam.sched_priority = AUDIO_PUMP_PRIO;
	schedPolicy = SCHED_FIFO;
	ret = pthread_setschedparam(currentHandle, schedPolicy, &schedParam);
	if (ret != 0)
	{
		printf("Failed to increase priority of audio pump thread to real time with error: %s \nTry running with sudo\n", strerror(ret));
	}	
	
	printf("Starting pump all thread\n");		
	while (1)
	{
        sem_wait(&pumpSem);
		
		// Set the pumpActive global flag
		pumpActive = 1;

		if (awe_layoutIsValid(&aweInstance) && awe_audioIsStarted(&aweInstance))
		{
			// Import new samples
			for (i = 0; i < configParams.inChannels ; i++)
			{
				awe_audioImportSamples(&aweInstance, &audioInputBuffer[i], configParams.inChannels, i, Sample32bit);
			}
					
			// Pump any required sublayouts
			pumpMask = awe_audioGetPumpMask(&aweInstance);

			// Lock the instance so packet processing doesn't occur simultaneously
			pthread_mutex_lock(&packetMutex);
			for (ix = 0; ix < NUM_THREADS; ix++)
			{
				if (pumpMask & (1U << ix))
				{

					// Pump the sublayout. Each pump may be in its own thread depending on the application
					ret |= awe_audioPump(&aweInstance, ix);
				}
			}
			// Do deferred processing if needed
			if (ret > 0) 
			{
				// This could be done in a loop until ret == 0, but doing one per fundamental period 
				// is more conservative at the cost of potentially taking longer to update a module parameter
				ret = awe_deferredSetCall(&aweInstance);
			}

			// Export samples
			for (i = 0; i < configParams.outChannels ; i++)
			{
				awe_audioExportSamples(&aweInstance, &audioOutputBuffer[i], configParams.outChannels, i, Sample32bit);
			}

			pthread_mutex_unlock(&packetMutex);
		}
		
		// Clear the pumpActive global flag
		pumpActive = 0;
	}
	return NULL;
}


void* audioCallbackSimulator(void * args)
{
	// Simple real-time audio simulator. Would be ALSA, PortAudio, etc callback
	// Will not achieve exact realtime interrupts
    struct timespec ts;
	struct timespec ts_tmp;
	long time_nsec;
	long long accumulated_time;
	long long target_time;
    long long overshoot = 0.0; 
	(void) args;
	INT32 schedPolicy, ret;
	struct sched_param schedParam;		
	
	// Setup sleep time for fundamental blocksize
	ts.tv_sec = 0;
	time_nsec = (long) ((float)1000000000L * ((float)configParams.fundamentalBlockSize / configParams.sampleRate));
	
	ts.tv_sec = 0;
	ts_tmp.tv_sec = 0;
	ts_tmp.tv_nsec = 0;

	// Set this thread to run at real time priority
	pthread_t currentHandle = pthread_self();
	schedParam.sched_priority = AUDIO_CALLBACK_PRIO;
	schedPolicy = SCHED_FIFO;
	ret = pthread_setschedparam(currentHandle, schedPolicy, &schedParam);
	if (ret != 0)
	{
		printf("Failed to increase priority of audio callback thread with error: %s \nTry running with sudo\n", strerror(ret));
	}		
	
	while (!exitAudioCallbackThread)
	{	
		// Check if the previous pump is active (overflow detection)
		// Workaround for the AWECore overflow detection failure
		if (!pumpActive)
		{
			// Kick off the audio thread
			sem_post(&pumpSem);
			
			// Might need to post more than once due to clock resolution 
			while (overshoot > time_nsec)
			{        
				overshoot -= time_nsec;
				sem_post(&pumpSem);
			}
		}
		else
		{
			overshoot = 0.0;
		}
        
		ts.tv_nsec = time_nsec;
		
		clock_gettime(CLOCK_MONOTONIC, &ts_tmp);
		target_time = ((long long)ts_tmp.tv_sec*1000000000) + ts_tmp.tv_nsec + time_nsec;
		
		while(1)
		{
			clock_nanosleep(CLOCK_MONOTONIC, 0, &ts, NULL);
			
			clock_gettime(CLOCK_MONOTONIC, &ts_tmp);
			
			accumulated_time = ((long long)ts_tmp.tv_sec*1000000000) + ts_tmp.tv_nsec;
			
			if (accumulated_time >= target_time)
			{
                overshoot += (accumulated_time - target_time);
				break;
			}
			else
			{
				ts.tv_nsec = (long)(target_time - accumulated_time);
			}
		}
	}
	return NULL;
}

INT32 aweuser_audioStart(AWEInstance *pAWE)
{
	(void) pAWE;
	// Audio start callback. Can be used to start audio stream from audio framework.
	// Could be ALSA, Pulse, Jack, Portaudio, etc. 

	// Kick off real-time audio simulation thread
	exitAudioCallbackThread = 0;
	pthread_create(&audioCallbackThreadHandle, NULL, audioCallbackSimulator, NULL); 
	audioStarted = 1;
	printf("Audio Started\n");
	return 0;
}

INT32 aweuser_audioStop(AWEInstance *pAWE)
{
	(void) pAWE;
	if(audioCallbackThreadHandle)
	{
		// Audio stop callback. Can be used to clean up audio stream.

		exitAudioCallbackThread = 1;
		pthread_join(audioCallbackThreadHandle, NULL);
		if (audioStarted)
		{
			printf("Audio Stopped\n");
			audioStarted = 0;
		}
	}
	return 0;
}

void InitializeAWEInstance()
{
	int ret = 0;
	int i, j;
	UINT32 module_descriptor_table_size;
	
	//set memory for awe instance and initialize to 0's
	memset(&aweInstance, 0, sizeof(AWEInstance));

	aweInstance.pInputPin = &aweInputPin;
	aweInstance.pOutputPin = &aweOutputPin;

	// Set pointers to the separate packet buffers in this example
	// Could set both pointer to same buffer if desired
	aweInstance.pPacketBuffer = awePacketBuffer;
	aweInstance.pReplyBuffer = awePacketBufferReply;
	
	aweInstance.packetBufferSize = MAX_COMMAND_BUFFER_LEN;
	
	aweInstance.pModuleDescriptorTable = module_descriptor_table;

	module_descriptor_table_size = sizeof(module_descriptor_table) / sizeof(module_descriptor_table[0]); 
	aweInstance.numModules = module_descriptor_table_size;

	aweInstance.numThreads = NUM_THREADS;
	aweInstance.sampleRate = configParams.sampleRate;
	aweInstance.fundamentalBlockSize = configParams.fundamentalBlockSize;
	aweInstance.pFlashFileSystem = HAS_FLASHFILESYSTEM;

	aweInstance.fastHeapASize = heapSizeFastA;
	aweInstance.fastHeapBSize = heapSizeFastB;
	aweInstance.slowHeapSize = heapSizeSlow;
	
	aweInstance.pFastHeapA = fastHeapA;
	aweInstance.pFastHeapB = fastHeapB;
	aweInstance.pSlowHeap = slowHeap;
	
	aweInstance.cbAudioStart = aweuser_audioStart;
	aweInstance.cbAudioStop = aweuser_audioStop;

	aweInstance.coreSpeed = configParams.coreSpeed;
	aweInstance.profileSpeed = configParams.profileSpeed;
	aweInstance.pName = TARGET_NAME;
	

	ret = awe_initPin(&aweInputPin, configParams.inChannels, NULL);
	if (ret != 0)
	{
		printf("awe_initPin inputPin failed\n");
	}
	ret = awe_initPin(&aweOutputPin, configParams.outChannels, NULL);
	if (ret != 0)
	{
		printf("awe_initPin outputPin failed\n");
	}

	ret = awe_init(&aweInstance);
	if (ret != 0) 
	{
		printf("awe_init instance 1 failed\n");
	}
	
	// Enable profiling (by default this is the case, but just called here to demonstrate usage)
	ret = awe_setProfilingStatus(&aweInstance, configParams.profileStatus);
	if (ret != 0) 
	{
		printf("awe_setProfilingStatus failed with error code: %d\n", ret);
	}

#ifndef PI
#define PI 3.141592653589793
#endif 
	// Fill audio input buffers with full scale sin waves
	for (i = 0; i < configParams.fundamentalBlockSize; i++)
	{
		for (j = 0; j < configParams.inChannels; j++) 
		{
			audioInputBuffer[i * configParams.inChannels + j] = float_to_fract32(sinf(2.f*PI*(j+1)*((float)configParams.sampleRate/configParams.fundamentalBlockSize) * (i / (float)configParams.sampleRate)));
		}
	}
}

int main( int argc, const char* argv[] )
{
	INT32 i;
	UINT32 inSize, outSize;
	const char *awbpath = NULL;
	INT32 ret;
	
	// Init default configurations
	configParams.fundamentalBlockSize = BASE_BLOCK_SIZE;
	configParams.sampleRate = SAMPLE_RATE;
	configParams.inChannels = NUM_INPUT_CHANNELS;
	configParams.outChannels = NUM_OUTPUT_CHANNELS;
	configParams.coreSpeed = CORE_SPEED;
	configParams.profileSpeed = PROFILE_SPEED;
	configParams.profileStatus = 1;
	configParams.tuningPort = 15002;
	
	// Check if the argc == 1, meaing that there are not input parameters supplied with the program.
	// Print the usage information, to help the user.
	for (i = 1; i < argc; i++)
	{
		const char *arg = argv[i];
	
		if (0 == strncmp(arg, "-profStatus:", 12))
		{
			configParams.profileStatus = atoi(arg + 12);
			printf("profStatus:%u\n",configParams.profileStatus);
		}
		else if (0 == strncmp(arg, "-bsize:", 7))
		{
			configParams.fundamentalBlockSize = atoi(arg + 7);
			printf("-bsize:     %u\n",configParams.fundamentalBlockSize);
		}
		else if (0 == strncmp(arg, "-inchans:", 9))
		{
			configParams.inChannels = atoi(arg + 9);
			printf("-inchans:   %u\n",configParams.inChannels);
		}
		else if (0 == strncmp(arg, "-outchans:", 10))
		{
			configParams.outChannels = atoi(arg + 10);
			printf("-outchans:  %u\n",configParams.outChannels);
		}
		else if(0 == strncmp(arg, "-sr:", 4))
		{
			configParams.sampleRate = (float)atof(arg + 4);
			printf("-sr:        %f\n",configParams.sampleRate);
		}			
		else if (0 == strncmp(arg, "-cf:", 4))
		{
			configParams.coreSpeed = (float)atof(arg + 4);
			printf("-cf:        %f\n",configParams.coreSpeed);
		}
		else if (0 == strncmp(arg, "-pf:", 4))
		{
			configParams.profileSpeed = (float)atof(arg + 4);
			printf("-pf:        %f\n",configParams.profileSpeed);
		}
		else if (0 == strncmp(arg, "-tport:", 7))
		{
			configParams.tuningPort = atoi(arg + 7);
			printf("-tport:     %d\n",configParams.tuningPort);
		}
		else if (0 == strncmp(arg, "-quiet", 6))
		{
			quiet = TRUE;
			printf("-quiet:     %d\n",quiet);
		}
		else if (0 == strncmp(arg, "-hsizefasta:", 12))
		{
			heapSizeFastA = atoi(arg + 12);
			printf("-hsizefasta:     %u\n",heapSizeFastA);
		}
		else if (0 == strncmp(arg, "-hsizefastb:", 12))
		{
			heapSizeFastB = atoi(arg + 12);
			printf("-hsizefastb:     %u\n",heapSizeFastB);
		}
		else if (0 == strncmp(arg, "-hsizeslow:", 11))
		{
			heapSizeSlow = atoi(arg + 11);
			printf("-hsizeslow:      %u\n",heapSizeSlow);
		}
		else if (0 == strncmp(arg, "-load:", 6))
		{
			awbpath = arg + 6;
			printf("-load:           %s\n",awbpath);		
		}
		else
		{
			usage(argv[0]);
		}
	}
	
	if (!quiet)
	{
		printf("\nAWECore initialised with the following parameters\n");
		printf("-cf:        	 %f Hz\n",configParams.coreSpeed);
		printf("-pf:        	 %f Hz\n",configParams.profileSpeed);
		printf("-sr:        	 %f Hz\n",configParams.sampleRate);
		printf("-bsize:     	 %u\n",configParams.fundamentalBlockSize);
		printf("-inchans:   	 %u\n",configParams.inChannels);
		printf("-outchans:  	 %u\n",configParams.outChannels);
		printf("-tport:     	 %d\n",configParams.tuningPort);
		printf("-hsizefasta:     %u\n",heapSizeFastA);
		printf("-hsizefastb:     %u\n",heapSizeFastB);
		printf("-hsizeslow:      %u\n",heapSizeSlow);
		printf("-load:      	 %s\n",awbpath ? awbpath : "undefined");
		if (configParams.profileStatus)
		{
			printf("-profStatus: Profiling is enabled\n");
		}
		else 
		{
			printf("-profStatus: Profiling is disabled\n");
		}
	}
	
	// Setup signal handler to kill sockets
	if (signal(SIGINT, sig_handler) == SIG_ERR)
	{
		printf("Can't catch SIGINT (%d)\n", SIGINT);
	}
	
	//Allocate the input and output buffers
	inSize = configParams.inChannels * configParams.fundamentalBlockSize;
	outSize = configParams.outChannels * configParams.fundamentalBlockSize;

	audioInputBuffer = malloc(inSize * sizeof(INT32));
	audioOutputBuffer = malloc(outSize * sizeof(INT32));
	
	// Allocate heaps
	fastHeapA = malloc(heapSizeFastA * sizeof(UINT32));
	fastHeapB = malloc(heapSizeFastB * sizeof(UINT32));
	slowHeap  = malloc(heapSizeSlow  * sizeof(UINT32));
	
	// Initialize the AWEInstance
	InitializeAWEInstance();
	
	if ( NULL != awbpath )
    {
		FILE *fin;
		UINT32 position;
		fin = fopen(awbpath, "rb");		
		if (NULL == fin )
		{
			printf("Error opening input file %s\n", awbpath);
			printf("Please check the path\n");
			exit(1);
		}

		//Load the AWB from the file
		ret = awe_loadAWBfromFile(&aweInstance, awbpath, &position);
		if (E_SUCCESS == ret) 
		{
			printf("The layout %s loaded successfully\n", awbpath);
		}
		else 
		{
			printf("The layout %s download unsuccessful in the positions %u with Error = %d \n", awbpath, position, ret);
			exit(1);
		}
	}

	// Initialize mutex and condition variable objects
	pthread_mutex_init(&packetMutex, NULL);

    sem_init(&pumpSem, 0, 0);

	// Start tuning thread
	pthread_create(&tuningThreadHandle, NULL, tuningPacketThread, NULL); 
	
	// Start audio pump all thread
	pthread_create(&audioPumpAllThreadHandle, NULL, aweuser_pumpAllAudio, NULL); 

	// Wait until all threads end
	pthread_join(tuningThreadHandle, NULL);
	
	pthread_join(audioPumpAllThreadHandle, NULL);
	
	return 0;

}
