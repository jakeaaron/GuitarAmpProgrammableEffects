/**
 * @file calc_rms.c
 *
 * @author Jacob Allenwood
 * @date October 24, 2015
 *
 * @brief This file contains the functions for calculating the rms value of the input signal.
 * RMS values are needed for the compressor. 
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "arm_math.h"
#include "ece486.h"
 
#include "calc_rms.h"




/**
 * @brief [initialize struct for rms calculations]
 * @details [contains circular buffer variables for the previous mean-square
 * values and the output array holding the rms values]
 * 
 * @param window_size [number of samples to average over to calculate the rms values]
 * @param block_size [number of samples to work on from input buffer]
 * 
 * @return [pointer to the rms struct]
 */
RMS_T * init_rms(int window_size, int block_size) {

	int i, j;

	// set up struct for rms calculation ----------------------------------
	RMS_T * V = (RMS_T *)malloc(sizeof(RMS_T));	// allocate struct
	if(V == NULL) return NULL;					// errcheck malloc

	V->window_size = window_size;	// number of samples to average over
	V->block_size = block_size;		// number of input and output samples
	V->old_s = 0.0;					// sum of previous square values
	V->index = 0;					// index through previous square values


	// initialize history of old window_size square samples ---------------
	V->history = (float *)malloc(sizeof(float) * (window_size - 1));	// sizeof window to average over - 1
	if(V->history == NULL) return NULL;
	for(i = 0; i < (window_size - 1); i++) {
		V->history[i] = 0.0;
	}


	// initialize output array --------------------------------------------
	V->output = (float *)malloc(sizeof(float) * block_size);	// sizeof window to average over
	if(V->output == NULL) return NULL;
	for(j = 0; j < block_size; j++) {
		V->output[j] = 0.0;
	}


	// return pointer to the struct ---------------------------------------
	return V;

}




/**
 * @brief [calculates the rms value of the last window_size input samples]
 * @details [Using a circular buffer to contain the last window_size samples,
 * a running total is kept of the square, so there is no need to re-calculate
 * the new average by going through every old sample, instead there is only one new
 * sample to calculate]
 * 
 * @param V [struct containing fields necessary for rms calculation]
 * @param input [buffer containing input samples to work on]
 */
void calc_rms(RMS_T * V, float * input) {

	int i;

	for(i = 0; i < V->block_size; i++) {

		// new square value 
		float new_s = (input[i] * input[i]);

		// y[n] = sqrt( previous window_size samples squared / window_size)
		V->output[i] = 0.6667 * (sqrt((V->old_s + new_s) / V->window_size)) - 1.0;
		// arm_sqrt_f32(((V->old_s + new_s) / V->window_size), V->output); 

		// subtract oldest value out of running square
		V->old_s -= V->history[V->index];
		// update running mean square
		V->old_s += new_s;

		// put new value in history array
		V->history[V->index] = new_s;

		// reset index if at the end of history buffer
		if(V->index == (V->window_size - 2)) {
			V->index = 0;
		} else {
			V->index++;
		}

	}

}


