/**
 * @file delay.c
 *
 * @author Jacob Allenwood
 * @date September 1, 2015
 *
 * @brief This file contains the functions for the delay effect portion 
 * of the GAPE suite. This program will be passed parameters to control 
 * the way each effect behaves.
 * 
 */

#include "delay.h"
#include <stdio.h>
#include <stdlib.h>
#include "ece486.h"



DELAY_T * init_delay(float FS, float time_delay, float delay_gain) {

	int i;			// counter for incrementing through coef array

	// sample delay is time delay * Fs
	int sample_delay = FS * time_delay;

	// set up struct for delay function --------------------------------------
	DELAY_T * D = malloc(sizeof(DELAY_T));
	if(D == NULL) return NULL;	// errcheck malloc call
	
	D->sample_delay = sample_delay;

	// put impulse response (fir filter coefficients) in delay struct -------
	// coef array is delayed impulse response in reverse order
	D->delay_coefs = malloc(sizeof(float) * sample_delay);
	if(D->delay_coefs == NULL) return NULL;
	for(i = 0; i < sample_delay; i++) {
		D->delay_coefs[i] = 0.0;
	}
	D->delay_coefs[0] = 1.0;
	D->delay_coefs[sample_delay - 1] = delay_gain;


	return D;

}


// void delay(DELAY_T * T, float * input, float * output1) {

// 	// fir routine to delay input signal
// 	arm_fir_f32(&(T->S), input, output1, T->block_size);
// }