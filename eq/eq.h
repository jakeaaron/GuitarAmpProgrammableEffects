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

#ifndef EQ
#define EQ
 
#include <stdint.h>


/**
 * @brief [structure containing necessary fields for the eq]
 * dB = 20log10(RMS)
 */
typedef struct eq_struct {
	float low_scale;		// scale to RMS to reach correct dB 
	float mid_scale;
	float high_scale;
	int block_size;
	arm_biquad_cascade_df2T_instance_f32 S_low;
	arm_biquad_cascade_df2T_instance_f32 S_mid;
	arm_biquad_cascade_df2T_instance_f32 S_high;
	float * output;
} EQ_T;



EQ_T * init_eq(
	float low_gain,		// scale in dB for low band
	float mid_gain,		// scale in dB for mid band
	float high_gain,	// scale in dB for high band
	int block_size
);



void calc_eq(
	EQ_T * Q,		// pointer to struct 
	float * input	// buffer of input samples to work on
);


#endif
