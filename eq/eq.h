/**
 * @file eq.h
 *
 * @author Jacob Allenwood
 * @date October 28, 2015
 *
 * @brief This file contains subroutine and data-type declarations necessary for 
 * the 3 band equalizer.
 * 
 */


// HEADER DEFINITION --------------------------------------------------

#ifndef EQ
#define EQ
 
// --------------------------------------------------------------------


// INCLUDE ------------------------------------------------------------

#include <stdint.h>

// --------------------------------------------------------------------




/**
 * @brief [structure containing necessary fields for the eq]
 * 
 */
typedef struct eq_struct {
	float low_scale;			// scale to RMS val to reach correct dB for the low frequency band
	float mid_scale;			// scale to RMS val to reach correct dB for the mid frequency band
	float high_scale;			// scale to RMS val to reach correct dB for the high frequency band
	int block_size;				// number of samples to work on
	arm_fir_instance_f32 S_low;	// arm fir struct for the low band lowpass filter
	arm_fir_instance_f32 S_mid;	// arm fir struct for the mid band lowpass filter
	DELAY_T * D;				// pointer to the delay struct
	float * low_band_out;		// output buffer for the low band calculation		
	float * mid_band_out;		// output buffer for the mid band calculation
	float * high_band_out;		// output buffer for the high band calculation
	float * output;				// buffer containing the equalized output samples
} EQ_T;


/**
 * @brief [initialize eq struct for arm iir routines]
 * 
 * @param low_gain [bass gain in dB]
 * @param mid_gain [mid gain in dB]
 * @param high_gain [treble gain in dB]
 * @param block_size [number of samples to work on]
 * @return [pointer to the eq struct]
 */
EQ_T * init_eq(
	float low_gain,		// scale in dB for low band
	float mid_gain,		// scale in dB for mid band
	float high_gain,	// scale in dB for high band
	int block_size,		// number of samples to work on
	int FS 				// sampling frequency necessary for delay
);


/**
 * @brief [calculate output for equalizer]
 * 
 * @param Q [pointer to the eq struct]
 * @param input [buffer containing samples to work on]
 */
void calc_eq(
	DELAY_T * D,	// pointer to delay struct
	EQ_T * Q,		// pointer to eq struct 
	float * input	// buffer of input samples to work on
);


#endif