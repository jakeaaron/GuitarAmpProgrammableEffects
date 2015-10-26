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
	RMS_T * R = (RMS_T *)malloc(sizeof(RMS_T));	// allocate struct
	if(R == NULL) return NULL;					// errcheck malloc

	R->window_size = window_size;	// number of samples to average over
	R->block_size = block_size;		// number of input and output samples
	R->old_s = 0.0;					// sum of previous square values
	R->index = 0;					// index through previous square values


	// initialize history of old window_size square samples ---------------
	R->history = (float *)malloc(sizeof(float) * window_size);	// sizeof window to average over
	if(R->history == NULL) return NULL;
	for(i = 0; i < window_size; i++) {
		R->history[i] = 0.0;
	}


	// initialize output array --------------------------------------------
	R->output = (float *)malloc(sizeof(float) * block_size);	// sizeof window to average over
	if(R->output == NULL) return NULL;
	for(j = 0; j < block_size; j++) {
		R->output[j] = 0.0;
	}


	// return pointer to the struct ---------------------------------------
	return R;

}




/**
 * @brief [calculates the rms value of the last window_size input samples]
 * @details [Using a circular buffer to contain the last window_size samples,
 * a running total is kept of the square, so there is no need to re-calculate
 * the new average by going through every old sample, instead there is only one new
 * sample to calculate]
 * 
 * @param R [struct containing fields necessary for rms calculation]
 * @param input [buffer containing input samples to work on]
 */
void calc_rms(RMS_T * R, float * input) {

	int i;

	for(i = 0; i < R->block_size; i++) {

		// new square value 
		float new_s = (input[i] * input[i]);

		// y[n] = sqrt( previous window_size samples squared / window_size)
		R->output[i] = sqrt((R->old_s + new_s) / R->window_size);

		// subtract oldest value out of running square
		R->old_s -= R->history[R->index];
		// update running mean square
		R->old_s += new_s;

		// put new value in history array
		R->history[R->index] = new_s;

		// reset index if at the end of history buffer
		if(R->index == (R->window_size - 1)) {
			R->index = 0;
		} else {
			R->index++;
		}

	}

}


