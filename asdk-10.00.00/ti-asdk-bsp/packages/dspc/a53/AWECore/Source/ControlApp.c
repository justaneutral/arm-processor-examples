/****************************************************************************
 *
 *		AWECore Control Example
 *		---------------------
 *
 ****************************************************************************
 *
 *	Description:	AWECore Control Example
 *			This example shows how to load an .awb from an array, and then
 *			how to get/set module variables using the corresponding 
 *			controlinterface header file and API's. 
 *			The example will stream an audio file called in_s32_stereo.raw
 *			and write the processed output to out_s32_stereo.raw.
 * 			This example meets all the conditions to achieve 0 blocks of 
 *			latency in the AWE Core audio path. See the Latency section
 *			of the docs for more info.  
 *
 *	Copyright: (c) 2020	DSP Concepts, Inc. All rights reserved.
 *									3235 Kifer Road
 *									Santa Clara, CA 95054-1527
 *
 *
 ***************************************************************************/
/** @example ControlApp.c

 * 
 * Examples */
 
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "AWECore.h"
#include "Errors.h"
#include "TargetInfo.h"
#include "passthrough_InitAWB.h"
#include "passthrough_ControlInterface.h"



const char awbFilename[] = "../Designs/passthrough.awb";
const char inFilename[] = "../Audio/in_s32_stereo.raw";   // input audio file: stereo 32-bit fixed-point
const char outFilename[] = "../Audio/out_s32_stereo.raw"; // output audio file: stereo 32-bit fixed-point

// Global AWEInstance structure and pins
AWEInstance aweInstance;

static IOPinDescriptor aweInputPin;
static IOPinDescriptor aweOutputPin;

// LISTOFCLASSOBJECTS comes from ModuleList.h
const void* module_descriptor_table[] =
{
	LISTOFCLASSOBJECTS
};

// Global heaps
UINT32 fastHeapA[FASTA_HEAP_SIZE];
UINT32 fastHeapB[FASTB_HEAP_SIZE];
UINT32 slowHeap[SLOW_HEAP_SIZE];

// Packet buffer
UINT32 awePacketBuffer[MAX_COMMAND_BUFFER_LEN];

// Input/output audio files and buffers
INT32 * inputBuffer, * outputBuffer;
FILE *fin;
FILE *fout;	

// Floating-point comparison function used for verification
// Inspired by: http://floating-point-gui.de/errors/comparison/
int isNearlyEqual(float a, float b, float epsilon) 
{
    float absA = fabsf(a);
    float absB = fabsf(b);
    float diff = fabsf(a - b);

    if (a == b) { // to handle infinities
        return 1;
    } else if (a == 0 || b == 0 || diff < FLT_MIN) {
        // a or b is zero or both are extremely close to it
        // relative error is less meaningful here
        return diff < (epsilon * FLT_MIN);
    } else { // use relative error
		if ((absA + absB) < FLT_MAX)
		{
			return diff / (absA + absB) < epsilon;
		}
		else
		{
			return diff / FLT_MAX < epsilon;
		}
    }
}

void destroyExample()
{
	fclose(fin);
	fclose(fout);
	free(inputBuffer);
	free(outputBuffer);
	inputBuffer = NULL;
	outputBuffer = NULL;
}

void InitializeAWEInstance()
{
	int ret;
	UINT32 module_descriptor_table_size;
	
	//set memory for awe instance and initialize to 0's
	memset(&aweInstance, 0, sizeof(AWEInstance));

	aweInstance.pInputPin = &aweInputPin;
	aweInstance.pOutputPin = &aweOutputPin;

	// No tuning interface in this example
	aweInstance.pPacketBuffer = awePacketBuffer;
	aweInstance.pReplyBuffer = awePacketBuffer;
	
	aweInstance.packetBufferSize = MAX_COMMAND_BUFFER_LEN;
	
	aweInstance.pModuleDescriptorTable = module_descriptor_table;

	module_descriptor_table_size = sizeof(module_descriptor_table) / sizeof(module_descriptor_table[0]); 
	aweInstance.numModules = module_descriptor_table_size;

	aweInstance.numThreads = 1;
	aweInstance.sampleRate = SAMPLE_RATE;
	aweInstance.fundamentalBlockSize = BASE_BLOCK_SIZE;
	aweInstance.pFlashFileSystem = HAS_FLASHFILESYSTEM;

	aweInstance.fastHeapASize = FASTA_HEAP_SIZE;
	aweInstance.fastHeapBSize = FASTB_HEAP_SIZE;
	aweInstance.slowHeapSize = SLOW_HEAP_SIZE;
	
	aweInstance.pFastHeapA = fastHeapA;
	aweInstance.pFastHeapB = fastHeapB;
	aweInstance.pSlowHeap = slowHeap;
	
	aweInstance.cbAudioStart = NULL;
	aweInstance.cbAudioStop = NULL;

	aweInstance.coreSpeed = CORE_SPEED;
	aweInstance.profileSpeed = PROFILE_SPEED;
	aweInstance.pName = NULL;
	

	ret = awe_initPin(&aweInputPin, NUM_INPUT_CHANNELS, NULL);
	if (ret != 0)
	{
		printf("awe_initPin inputPin failed\n");
	}
	ret = awe_initPin(&aweOutputPin, NUM_OUTPUT_CHANNELS, NULL);
	if (ret != 0)
	{
		printf("awe_initPin outputPin failed\n");
	}

	ret = awe_init(&aweInstance);
	if (ret != 0) 
	{
		printf("awe_init failed\n");
	}
	
	// Enable profiling (by default this is the case, but just called here to demonstrate usage)
	ret = awe_setProfilingStatus(&aweInstance, 1);
	if (ret != 0) 
	{
		printf("awe_setProfilingStatus failed\n");
	}
}


int main() {
    INT32 ret = 0;
	UINT32 position; 
	UINT32 layoutInChannels, layoutOutChannels, layoutBlockSize;
	const INT32 inputChannels = 2;
	const INT32 outputChannels = 2;
	UINT32 inSize, outSize;
	
	InitializeAWEInstance();
	
	// Load the .awb from C array
	ret = awe_loadAWBfromArray(&aweInstance, Core0_InitCommands, Core0_InitCommands_Len, &position);
	if (ret != E_SUCCESS)
	{
		printf("Error loading AWB from array: error = %d, at offset %u\n", ret, position);
		exit(1);
	}
	
	ret = awe_layoutIsValid(&aweInstance);
	if (ret != 1)
	{
		printf("Error: Loaded layout is not valid: error = %d\n", ret);
		exit(1);
	}
	
	ret = awe_audioIsStarted(&aweInstance);
	if (ret != 1)
	{
		printf("Error: Audio not started: error = %d\n", ret);
		exit(1);
	}	
	
	// Get the layout I/O configuration
	// Input and output blocksizes must be the same
	//NOTE: Because there can only be one input pin and one output pin, all API's that take the argument "pinIdx" will always pass in 0. This is demonstrated in the function calls below.
	awe_layoutGetChannelCount(&aweInstance, 0, &layoutInChannels, &layoutOutChannels);
	ret = awe_layoutGetInputBlockSize(&aweInstance, 0, &layoutBlockSize);
	if (ret != E_SUCCESS)
	{
		printf("Error: GetBlockSize failed: error = %d\n", ret);
		exit(1);
	}
	printf("Layout loaded with inputChannels=%u, outputChannels=%u, blockSize=%u\n", layoutInChannels, layoutOutChannels, layoutBlockSize);
	
    // Prepare the stereo input and output buffers, and open files
	inSize = inputChannels * layoutBlockSize;
	outSize = outputChannels * layoutBlockSize;
	inputBuffer = malloc(inSize * sizeof(INT32));
	outputBuffer = malloc(outSize * sizeof(INT32));

    fin = fopen(inFilename, "rb");
    if (fin == NULL)
	{
        printf("Error opening file %s\n", inFilename);
		exit(1);
	}
	
    fout = fopen(outFilename, "wb");
    if (fout == NULL)
	{
        printf("Error opening file %s\n", outFilename);
		exit(1);
	}


	{
		// Set Scaler1 gain attributes
		float scaler1Gain;
		float scaler1TargetGain;
		float meter1Values[AWE_Meter1_value_SIZE];
		long count = 0;
		int numSamplesRead = 0;
		UINT32 classId;

		//NOTE: All handles, IDs, and sizes that are passed into the ctrl interface API functions are found in the example ControlInterface header file "passthrough_ControlInterface.h"

		// Check that the layout has the correct control modules in it
		int moduleFound = 0;
		if (awe_ctrlGetModuleClass(&aweInstance, AWE_Scaler1_gain_HANDLE, &classId) == E_SUCCESS)
		{
			// Check that module assigned this object ID is of module class SinkInt
			if (classId == AWE_Scaler1_classID)
			{
				moduleFound = 1;
			}
		}		
		if (!moduleFound)
		{
			printf("Error: Scaler1 module not found in layout\n");
		}

		if (awe_ctrlGetModuleClass(&aweInstance, AWE_Meter1_value_HANDLE, &classId) == E_SUCCESS)
		{
			// Check that module assigned this object ID is of module class SinkInt
			if (classId == AWE_Meter1_classID)
			{
				moduleFound = 1;
			}
		}		
		if (!moduleFound)
		{
			printf("Error: Meter1 module not found in layout\n");
		}
		
		printf("\n");

		// Set the gain to -10 dB
		scaler1Gain = -10.0;
		ret = awe_ctrlSetValue(&aweInstance, AWE_Scaler1_gain_HANDLE, (const void *)&scaler1Gain, 0, AWE_Scaler1_gain_SIZE);
		printf("SetValue: scaler1Gain = %f, error = %d\n", scaler1Gain, ret);

		ret = awe_ctrlGetValue(&aweInstance, AWE_Scaler1_gain_HANDLE, &scaler1Gain, 0, AWE_Scaler1_gain_SIZE);
		printf("GetValue: scaler1Gain = %f, error = %d\n", scaler1Gain, ret);

		ret = awe_ctrlGetValue(&aweInstance, AWE_Scaler1_targetGain_HANDLE, &scaler1TargetGain, 0, AWE_Scaler1_targetGain_SIZE);
		printf("GetValue: scalerTarget1Gain = %f, error = %d\n", scaler1TargetGain, ret);

		
		// Process the input data
		printf("\nStarting to process [%s] into [%s]\n\n", inFilename, outFilename);
		while ((numSamplesRead = fread(inputBuffer, sizeof(int), inSize, fin)) > 0) 
		{
			INT32 i, j;
			INT32 numOutFrames;
			UINT32 pumpMask;
			// Import and export samples for each channel in I/O files
			for (i = 0; i < inputChannels; i++) 
			{
				ret = awe_audioImportSamples(&aweInstance, inputBuffer + i, inputChannels, i, Sample32bit);
				if (ret != E_SUCCESS)
				{
					printf("Error: audio import failed: error = %d\n", ret);
				}
			}

			// Only one layout in this example, so only bother checking for that field of the pump mask
			pumpMask = awe_audioGetPumpMask(&aweInstance);
			if (pumpMask & 1U)
			{
				// Pump the layout if ready
				ret = awe_audioPump(&aweInstance, 0);
				if (ret != E_SUCCESS)
				{
					printf("Error: audio pump failed: layout=%d, error=%d\n", 0, ret);
				}								
			}

			for (i = 0; i < outputChannels; i++) 
			{
				ret = awe_audioExportSamples(&aweInstance, outputBuffer + i, outputChannels, i, Sample32bit);
				if (ret != E_SUCCESS)
				{
					printf("Error: audio export failed: error = %d\n", ret);
				}
			}
			
			count++;
			// Print the Meter1 value every 1024 pump cycles
			if ((count % 1024) == 0) 
			{
				ret = awe_ctrlGetValue(&aweInstance, AWE_Meter1_value_HANDLE, &meter1Values, 0, AWE_Meter1_value_SIZE);
				printf("GetValue: meter1Value = %f, error = %d\n", meter1Values[0], ret);
			}

			fwrite(outputBuffer, sizeof(int), outSize, fout);

			// Perform inline verification
			// Channel 1 is expected to be attenuated by -10 dB
			// Channel 2 is expected to be passed through without modification
#define EPSILON 0.1f
			numOutFrames = numSamplesRead / outputChannels;
			for (j = 0; j < numOutFrames; j++) {
				float expectedValueChannel1 = powf(10.0, scaler1Gain / 20.0) * (float)inputBuffer[inputChannels * j];
				float expectedValueChannel2 = (float)inputBuffer[inputChannels * j + 1];
				if (isNearlyEqual(expectedValueChannel1, (float)outputBuffer[outputChannels * j], EPSILON) == 0) {
					printf("Verification error. Expected %f on channel 1 but got %f, sample %d\n", expectedValueChannel1,
						   (float)outputBuffer[outputChannels * j], j);
					destroyExample();
					exit(1);
				}
				
				if (isNearlyEqual(expectedValueChannel2, (float)outputBuffer[outputChannels * j + 1], EPSILON) == 0) {
					printf("Verification error. Expected %f on channel 2 but got %f\n, sample %d\n", expectedValueChannel2,
						   (float)outputBuffer[outputChannels * j + 1], j);
					destroyExample();
					exit(1);
				}
			}
			// There is no latency through the audio pump
		}


		printf("\n");
		ret = awe_ctrlGetValue(&aweInstance, AWE_Scaler1_gain_HANDLE, &scaler1Gain, 0, AWE_Scaler1_gain_SIZE);
		printf("GetValue: scaler1Gain = %f, error = %d\n", scaler1Gain, ret);

		ret = awe_ctrlGetValue(&aweInstance, AWE_Scaler1_targetGain_HANDLE, &scaler1TargetGain, 0, AWE_Scaler1_targetGain_SIZE);
		printf("GetValue: scalerTarget1Gain = %f, error = %d\n", scaler1TargetGain, ret);
	}
	
	// Clean up
	destroyExample();
    return 0;
}
