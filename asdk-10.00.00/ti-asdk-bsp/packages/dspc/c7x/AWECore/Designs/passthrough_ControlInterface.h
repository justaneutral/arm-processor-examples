/****************************************************************************
 *
 *		Target Tuning Symbol File
 *		-------------------------
 *
 * 		This file is populated with symbol information only for modules
 *		that have an object ID of 30000 or greater assigned.
 *
 *          Generated on:  01-Oct-2020 15:24:41
 *
 ***************************************************************************/

#ifndef AWE_PASSTHROUGH_CONTROLINTERFACE_H
#define AWE_PASSTHROUGH_CONTROLINTERFACE_H

// ----------------------------------------------------------------------
//  [ScalerV2]
// Newly created subsystem

#define AWE_Scaler2_classID 0xBEEF0813
#define AWE_Scaler2_ID 30001

// int profileTime - 24.8 fixed point filtered execution time. Must be pumped 1000 times to get to within .1% accuracy
#define AWE_Scaler2_profileTime_HANDLE 0x07531007
#define AWE_Scaler2_profileTime_MASK 0x00000080
#define AWE_Scaler2_profileTime_SIZE 0x00000001

// float gain - Gain in either linear or dB units.
// Default value: 0
// Range: -40 to 40
#define AWE_Scaler2_gain_HANDLE 0x07531008
#define AWE_Scaler2_gain_MASK 0x00000100
#define AWE_Scaler2_gain_SIZE 0x00000001

// float smoothingTime - Time constant of the smoothing process (0 = 
//         unsmoothed).
// Default value: 0
// Range: 0 to 1000
#define AWE_Scaler2_smoothingTime_HANDLE 0x07531009
#define AWE_Scaler2_smoothingTime_MASK 0x00000200
#define AWE_Scaler2_smoothingTime_SIZE 0x00000001

// int isDB - Selects between linear (=0) and dB (=1) operation
// Default value: 1
// Range: 0 to 1
#define AWE_Scaler2_isDB_HANDLE 0x0753100A
#define AWE_Scaler2_isDB_MASK 0x00000400
#define AWE_Scaler2_isDB_SIZE 0x00000001

// float targetGain - Target gain in linear units.
#define AWE_Scaler2_targetGain_HANDLE 0x0753100B
#define AWE_Scaler2_targetGain_MASK 0x00000800
#define AWE_Scaler2_targetGain_SIZE 0x00000001

// float currentGain - Instantaneous gain applied by the module.
#define AWE_Scaler2_currentGain_HANDLE 0x0753100C
#define AWE_Scaler2_currentGain_MASK 0x00001000
#define AWE_Scaler2_currentGain_SIZE 0x00000001

// float smoothingCoeff - Smoothing coefficient.
#define AWE_Scaler2_smoothingCoeff_HANDLE 0x0753100D
#define AWE_Scaler2_smoothingCoeff_MASK 0x00002000
#define AWE_Scaler2_smoothingCoeff_SIZE 0x00000001


// ----------------------------------------------------------------------
//  [Meter]
// Newly created subsystem

#define AWE_Meter2_classID 0xBEEF086B
#define AWE_Meter2_ID 30003

// int profileTime - 24.8 fixed point filtered execution time. Must be pumped 1000 times to get to within .1% accuracy
#define AWE_Meter2_profileTime_HANDLE 0x07533007
#define AWE_Meter2_profileTime_MASK 0x00000080
#define AWE_Meter2_profileTime_SIZE 0x00000001

// int meterType - Operating mode of the meter. Selects between peak and 
//         RMS calculations. See the discussion section for more details.
// Default value: 18
// Range: unrestricted
#define AWE_Meter2_meterType_HANDLE 0x07533008
#define AWE_Meter2_meterType_MASK 0x00000100
#define AWE_Meter2_meterType_SIZE 0x00000001

// float attackTime - Attack time of the meter. Specifies how quickly 
//         the meter value rises.
#define AWE_Meter2_attackTime_HANDLE 0x07533009
#define AWE_Meter2_attackTime_MASK 0x00000200
#define AWE_Meter2_attackTime_SIZE 0x00000001

// float releaseTime - Release time of the meter. Specifies how quickly 
//         the meter decays.
#define AWE_Meter2_releaseTime_HANDLE 0x0753300A
#define AWE_Meter2_releaseTime_MASK 0x00000400
#define AWE_Meter2_releaseTime_SIZE 0x00000001

// float attackCoeff - Internal coefficient that realizes the attack 
//         time.
#define AWE_Meter2_attackCoeff_HANDLE 0x0753300B
#define AWE_Meter2_attackCoeff_MASK 0x00000800
#define AWE_Meter2_attackCoeff_SIZE 0x00000001

// float releaseCoeff - Internal coefficient that realizes the release 
//         time.
#define AWE_Meter2_releaseCoeff_HANDLE 0x0753300C
#define AWE_Meter2_releaseCoeff_MASK 0x00001000
#define AWE_Meter2_releaseCoeff_SIZE 0x00000001

// float value[1] - Array of meter output values, one per channel.
#define AWE_Meter2_value_HANDLE 0x8753300D
#define AWE_Meter2_value_MASK 0x00002000
#define AWE_Meter2_value_SIZE 0x00000001


// ----------------------------------------------------------------------
//  [ScalerV2]
// Newly created subsystem

#define AWE_Scaler1_classID 0xBEEF0813
#define AWE_Scaler1_ID 30000

// int profileTime - 24.8 fixed point filtered execution time. Must be pumped 1000 times to get to within .1% accuracy
#define AWE_Scaler1_profileTime_HANDLE 0x07530007
#define AWE_Scaler1_profileTime_MASK 0x00000080
#define AWE_Scaler1_profileTime_SIZE 0x00000001

// float gain - Gain in either linear or dB units.
// Default value: 0
// Range: -40 to 40
#define AWE_Scaler1_gain_HANDLE 0x07530008
#define AWE_Scaler1_gain_MASK 0x00000100
#define AWE_Scaler1_gain_SIZE 0x00000001

// float smoothingTime - Time constant of the smoothing process (0 = 
//         unsmoothed).
// Default value: 0
// Range: 0 to 1000
#define AWE_Scaler1_smoothingTime_HANDLE 0x07530009
#define AWE_Scaler1_smoothingTime_MASK 0x00000200
#define AWE_Scaler1_smoothingTime_SIZE 0x00000001

// int isDB - Selects between linear (=0) and dB (=1) operation
// Default value: 1
// Range: 0 to 1
#define AWE_Scaler1_isDB_HANDLE 0x0753000A
#define AWE_Scaler1_isDB_MASK 0x00000400
#define AWE_Scaler1_isDB_SIZE 0x00000001

// float targetGain - Target gain in linear units.
#define AWE_Scaler1_targetGain_HANDLE 0x0753000B
#define AWE_Scaler1_targetGain_MASK 0x00000800
#define AWE_Scaler1_targetGain_SIZE 0x00000001

// float currentGain - Instantaneous gain applied by the module.
#define AWE_Scaler1_currentGain_HANDLE 0x0753000C
#define AWE_Scaler1_currentGain_MASK 0x00001000
#define AWE_Scaler1_currentGain_SIZE 0x00000001

// float smoothingCoeff - Smoothing coefficient.
#define AWE_Scaler1_smoothingCoeff_HANDLE 0x0753000D
#define AWE_Scaler1_smoothingCoeff_MASK 0x00002000
#define AWE_Scaler1_smoothingCoeff_SIZE 0x00000001


// ----------------------------------------------------------------------
//  [Meter]
// Newly created subsystem

#define AWE_Meter1_classID 0xBEEF086B
#define AWE_Meter1_ID 30002

// int profileTime - 24.8 fixed point filtered execution time. Must be pumped 1000 times to get to within .1% accuracy
#define AWE_Meter1_profileTime_HANDLE 0x07532007
#define AWE_Meter1_profileTime_MASK 0x00000080
#define AWE_Meter1_profileTime_SIZE 0x00000001

// int meterType - Operating mode of the meter. Selects between peak and 
//         RMS calculations. See the discussion section for more details.
// Default value: 18
// Range: unrestricted
#define AWE_Meter1_meterType_HANDLE 0x07532008
#define AWE_Meter1_meterType_MASK 0x00000100
#define AWE_Meter1_meterType_SIZE 0x00000001

// float attackTime - Attack time of the meter. Specifies how quickly 
//         the meter value rises.
#define AWE_Meter1_attackTime_HANDLE 0x07532009
#define AWE_Meter1_attackTime_MASK 0x00000200
#define AWE_Meter1_attackTime_SIZE 0x00000001

// float releaseTime - Release time of the meter. Specifies how quickly 
//         the meter decays.
#define AWE_Meter1_releaseTime_HANDLE 0x0753200A
#define AWE_Meter1_releaseTime_MASK 0x00000400
#define AWE_Meter1_releaseTime_SIZE 0x00000001

// float attackCoeff - Internal coefficient that realizes the attack 
//         time.
#define AWE_Meter1_attackCoeff_HANDLE 0x0753200B
#define AWE_Meter1_attackCoeff_MASK 0x00000800
#define AWE_Meter1_attackCoeff_SIZE 0x00000001

// float releaseCoeff - Internal coefficient that realizes the release 
//         time.
#define AWE_Meter1_releaseCoeff_HANDLE 0x0753200C
#define AWE_Meter1_releaseCoeff_MASK 0x00001000
#define AWE_Meter1_releaseCoeff_SIZE 0x00000001

// float value[1] - Array of meter output values, one per channel.
#define AWE_Meter1_value_HANDLE 0x8753200D
#define AWE_Meter1_value_MASK 0x00002000
#define AWE_Meter1_value_SIZE 0x00000001


#define AWE_OBJECT_FOUND 0

#endif // AWE_PASSTHROUGH_CONTROLINTERFACE_H

