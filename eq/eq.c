/**
 * @file eq.c
 *
 * @author Jacob Allenwood
 * @date October 28, 2015
 *
 * @brief This file contains the functions for the 3 band equalizer portion 
 * of the GAPE suite.
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include "ece486.h"

#include "eq.h"
#include "iir_low.h"
#include "iir_mid.h"
#include "iir_high.h"






/**
 * @brief [brief description]
 * @details [long description]
 * 
 * @param low_gain [description]
 * @param mid_gain [description]
 * @param high_gain [description]
 * @param block_size [description]
 * @return [description]
 */
EQ_T * init_eq(float low_gain, float mid_gain, float high_gain, int block_size) {
	// function gain parameters are in db
	// pow(dB / 20) = gain

	int i;

	// set up struct for eq --------------------------------
	EQ_T * Q = (EQ_T *)malloc(sizeof(EQ_T));	// allocate struct
	if(Q == NULL) return NULL;					// errcheck malloc call
	
	Q->low_scale = pow(10, (low_gain / 20.0));
	Q->mid_scale = pow(10, (mid_gain / 20.0));
	Q->high_scale = pow(10, (high_gain / 20.0));

	Q->S_low = arm_biquad_cascade_df2T_instance_f32 S_low;
	Q->S_mid = arm_biquad_cascade_df2T_instance_f32 S_mid;
	Q->S_high = arm_biquad_cascade_df2T_instance_f32 S_high;

	float * low_state = (float *)malloc(sizeof(float) * (2 * LOW_MWSPT_NSEC));
	float * mid_state = (float *)malloc(sizeof(float) * (2 * MID_MWSPT_NSEC));
	float * high_state = (float *)malloc(sizeof(float) * (2 * HIGH_MWSPT_NSEC));
	
	Q->output = (float *)malloc(sizeof(float) * block_size);
	for(i = 0; i < block_size; i++) {
		Q->output[i] = 0.0;
	}

	arm_biquad_cascade_df2T_init_f32(&S_low, LOW_SECTIONS, &iir_low_coefs[0], low_state);
	arm_biquad_cascade_df2T_init_f32(&S_mid, MID_SECTIONS, &iir_mid_coefs[0], mid_state);
	arm_biquad_cascade_df2T_init_f32(&S_high, HIGH_SECTIONS, &iir_high_coefs[0], high_state);

}


/**
 * @brief [brief description]
 * @details [long description]
 * 
 * @param Q [description]
 * @param input [description]
 */
void calc_eq(EQ_T * Q, float * input) {

	int i, j;

	// set up output buffer for biquad routines
	float * low_filter_out = (float *)malloc(sizeof(float) * Q->block_size);
	float * mid_filter_out = (float *)malloc(sizeof(float) * Q->block_size);
	float * high_filter_out = (float *)malloc(sizeof(float) * Q->block_size);
	
	// filter low, mid and high bands
	arm_biquad_cascade_df2T_f32(&(Q->S_low), input, low_filter_out, Q->block_size);
	arm_biquad_cascade_df2T_f32(&(Q->S_mid), input, mid_filter_out, Q->block_size);
	arm_biquad_cascade_df2T_f32(&(Q->S_high), input, high_filter_out, Q->block_size);

	// y[n] = low[n] + mid[n] + high[n]
	for(i = 0; i < Q->block_size; i++) {
		Q->output[i] = (Q->low_scale * low_filter_out[i]) + (Q->mid_scale * mid_filter_out[i]) + (Q->high_scale * high_filter_out[i]);
	}
	
}








