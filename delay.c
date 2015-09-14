/**
 * @file effects.c
 *
 * @author Jacob Allenwood
 * @date September 1, 2015
 *
 * @brief This file contains the functions for each effect in the 
 * GAPE unit. This program will be passed parameters to control 
 * the way each effect behaves.
 * 
 */

#include "effects.h"

DELAY_T * init_delay(uint32_t FS, float32_t sample_delay, float32_t delay_gain, uint32_t block_size) {

	// sample delay is time delay * Fs
	float32_t sample_delay = FS * time_delay;

	// coef array is delayed impulse response in reverse order
	float32_t firCoeffs32[sample_delay] = 0;
	firCoeffs32[0] = delay_gain;
	firCoeffs32[sample_delay - 1] = 1;

	float32_t firStateF32[block_size + sample_delay - 1];

	// set up struct for delay function
	DELAY_T * D;
	D = (DELAY_T *)malloc(sizeof(DELAY_T));

	D->S = arm_fir_instance_f32 S;	// FIR struct instance
	D->block_size = block_size;
	D->sample_delay = sample_delay;
	D->delay_gain = delay_gain;
	

	// Call FIR init function to initialize the instance structure -------------
	// sample_delay is number of fir coefs
	arm_fir_init_f32(S, sample_delay, (float32_t *)&firCoeffs32[0], &firStateF32[0], block_size);

	return D;

}


delay(DELAY_T * T, float32_t * input, float32_t * output1) {

	// fir routine to delay input signal
	arm_fir_f32(&(T->S), input, output1, T->block_size);
}