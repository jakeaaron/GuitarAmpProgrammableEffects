/**
 * @file delay.h
 *
 * @author Jacob Allenwood
 * @date September 1, 2015
 *
 * @brief This file contains subroutine and data-type declarations necessary for 
 * the delay effect.
 * 
 */

#ifndef DELAY
#define DELAY
 
#include <stdint.h>


/**
 * @brief [structure containing necessary fields for the delay calculations]
 * 
 */
typedef struct delay_struct {
	// data structure contains number of coefs, pointer to the coef array
	int sample_delay;		// amount of delay in number of samples
	int block_size;			// amount of samples to work on
	float delay_gain;		// scaled volume of original input
	int index;				// index through circular buffer of old values
	float * history;		// array holding old samples for delay
	float * output;			// array for output
} DELAY_T;


/**
 * @brief [initialize the delay struct]
 * 
 * @param FS [sampling frequency]
 * @param time_delay [amount of delay in seconds]
 * @param delay_gain [volume of delayed signal in percentage of original signal]
 * @param block_size [amount of samples to work on]
 * @return [description]
 */
DELAY_T * init_delay(
	int delay_units,	// 0 is delay in samples, 1 is delay in seconds
	int FS,				// sampling frequency
	float time_delay,	// amount of delay 
	float delay_gain,	// volume of delayed signal
	int block_size		// amount of samples to work on
);


/**
 * @brief [calculates a block_size of delayed samples for output]
 * 
 * @param D [pointer to delay_struct]
 * @param input [buffer containing samples to work on of size block_size]
 */
void calc_delay(
	int input_toggle,	// 0 is just delay signal, 1 is add delayed signal back to input
	DELAY_T * D,		// pointer to struct 
	float * input		// buffer of input samples to work on
);


#endif
