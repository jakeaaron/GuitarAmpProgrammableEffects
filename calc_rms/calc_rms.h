/**
 * @file calc_rms.h
 *
 * @author Jacob Allenwood
 * @date October 24, 2015
 *
 * @brief This file contains subroutine and data-type declarations necessary for 
 * calulating the rms value of the input signal. This will be used for the compressor
 * effect.
 * 
 */
 


#ifndef RMS_H
#define RMS_H

#include <stdint.h>

/**
 * @brief [structure containing necessary fields for the root mean square calculations]
 * 
 */
typedef struct rms_struct {  
	int window_size;    // number of samples to average over  		
	int block_size;  	// number of input and output samples
	float old_rms; 		// sum of previous mean-square values
	float * history;   	// buffer containing previous mean-square values
	int index;			// index through previous mean-square values
	float * output;  	// buffer for output rms values
} RMS_T;


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
RMS_T * init_rms(
	int window_size, 	// number of samples to average over			   
	int block_size		// number of input and output samples
);


/**
 * @brief [calculates the rms value of the last window_size input samples]
 * @details [Using a circular buffer to contain the last window_size samples
 * a running total is kept of the mean-square, so there is no need to re-calculate
 * the new average by going through every old sample, instead there is only one new
 * sample to calculate]
 * 
 * @param R [struct containing fields necessary for rms calculation]
 * @param input [buffer containing input samples to work on]
 */
void calc_rms(
	RMS_T * R,			// pointer to rms struct
	float * input		// buffer containing input samples to work on
);


#endif


