/**
 * @file delay.c
 *
 * @author Jacob Allenwood
 * @date September 1, 2015
 *
 * @brief This file contains the functions for the delay effect portion 
 * of the GAPE suite.
 * 
 */

#include <stdio.h>
#include <stdlib.h>

#include "delay.h"




/**
 * @brief [initialize delay struct]
 * 
 * @param FS [Sampling frequency used to figure out the sample delay]
 * @param time_delay [amount of delay in seconds]
 * @param delay_gain [volume of delayed signal in percentage of original signal]
 * @param block_size [amount of samples to work on]
 * @return [pointer to the delay_struct]
 */
DELAY_T * init_delay(int delay_units, int FS, float delay, float delay_gain, int block_size) {

	// initialize variables --------------------------------------------
	int i, j;								// incremental counters		
	int index = 0;							// index through history array
	// if delay entered is in time, then figure out the delay in samples
	// otherwise the delay was already entered in samples
	if(delay_units == 1) {
		int delay = (FS * delay);			// number of samples to delay by
	}


	// set up struct for delay function --------------------------------
	DELAY_T * D = (DELAY_T *)malloc(sizeof(DELAY_T));	// allocate struct
	if(D == NULL) return NULL;							// errcheck malloc call
	
	D->sample_delay = (int)delay;	// amount to delay by in samples
	D->block_size = block_size;
	D->delay_gain = delay_gain;
	D->index = index;


	// initialize array of history of old samples ----------------------
	D->history = (float *)malloc(sizeof(float) * delay);	// sizeof delay
	if(D->history == NULL) return NULL;
	for(i = 0; i < delay; i++) {
		D->history[i] = 0.0;
	}


	// initialize output array -----------------------------------------
	D->output = (float *)malloc(sizeof(float) * block_size);
	for(j = 0; j < block_size; j++) {
		D->output[j] = 0.0;
	}


	// return pointer to the struct ------------------------------------
	return D;

}


/**
 * @brief [calculates a block_size of delayed samples for output]
 * 
 * @param D [pointer to delay_struct]
 * @param input [buffer containing samples to work on of size block_size]
 */
void calc_delay(int input_toggle, DELAY_T * D, float * input) {

	int i;

	// calculate block of output
	for(i = 0; i < D->block_size; i++) {

		// output is either current input and delayed signal or just delayed signal
		if(input_toggle) {	// used for delay effect, want input signal and delayed signal
			// y[n] = x[n] + (G * x[n - D])
			// output is input plus scaled sample from sample_delay samples ago
			D->output[i] = input[i] + (D->delay_gain * (D->history[D->index])); 
		} else {	// used for eq, need to delay signal to keep each band signal in phase with each other
			// this is just the delayed sample from sample_delay samples ago
			D->output[i] = (D->delay_gain * (D->history[D->index]))
		}

		// place new sample in history array
		D->history[D->index] = input[i];

		// reset index if at the end of history buffer
		if(D->index == (D->sample_delay - 1)) {
			D->index = 0;	// reset to beginning of buffer
		} else {
			// if Fs = 48000 then history index will go all the way to 23999 for a .5s delay
			D->index++; // incremement index all the way through history array
		}
		
	}

}


