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

#include "delay.h"
#include <stdio.h>
#include <stdlib.h>
#include "ece486.h"


/**
 * @brief [initialize delay struct]
 * 
 * @param FS [Sampling frequency used to figure out the sample delay]
 * @param time_delay [amount of delay in seconds]
 * @param delay_gain [volume of delayed signal in percentage of original signal]
 * @param block_size [amount of samples to work on]
 * @return [pointer to the delay_struct]
 */
DELAY_T * init_delay(int FS, float time_delay, float delay_gain, int block_size) {

	// initialize variables --------------------------------------------
	int i, j;			// incremental counters		
	int index = 0;		// index through history array
	// sample delay is time delay * Fs
	int sample_delay = FS * time_delay;


	// set up struct for delay function --------------------------------
	DELAY_T * D = malloc(sizeof(DELAY_T));	// allocate struct
	if(D == NULL) return NULL;				// errcheck malloc call
	
	D->sample_delay = sample_delay;
	D->block_size = block_size;
	D->delay_gain = delay_gain;
	D->index = index;

	// initialize array of history of old samples
	D->history = malloc(sizeof(float) * sample_delay);
	if(D->history == NULL) return NULL;
	for(i = 0; i < sample_delay; i++) {
		D->history[i] = 1.0;
	}

	// initialize output array
	D->output = malloc(sizeof(float) * block_size);
	for(j = 0; j < block_size; j++) {
		D->output[j] = 0.0;
	}


	// return pointer to the struct -----------------------------------
	return D;

}


/**
 * @brief [calculates a block_size of delayed samples for output]
 * 
 * @param D [pointer to delay_struct]
 * @param input [buffer containing samples to work on of size block_size]
 */
void calc_delay(DELAY_T * D, float * input) {

	int i;

	// calculate block of output
	for(i = 0; i < D->block_size; i++) {

		// y[n] = x[n] + Gx[n - D]
		// output is input plus sample from sample_delay samples ago
		D->output[i] = input[i] + (D->delay_gain * (D->history[D->index])); 
		D->history[D->index] = input[i];

		// reset index if at the end of history buffer
		if(D->index == (D->sample_delay - 1)) {
			D->index = 0;
		} else {
			// if Fs = 48000 then history index will go all the way to 23999
			D->index++; // incremement index all the way through history array
		}
	}

}


