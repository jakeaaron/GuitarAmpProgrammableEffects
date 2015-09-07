/**
 * @file effects.h
 *
 * @author Jacob Allenwood
 * @date September 1, 2015
 *
 * @brief This file contains subroutine and data-type declarations necessary for 
 * the GAPE unit.
 * 
 */


#include <stdint.h>

/**
 * DELAY **************************************************************************************************************
 */

// define decimation to reduce sample rate for guitar signal, so the dsp doesn't run out of memory
#define Fs 124000

/**
 * @brief struct declaration for delay function using an FIR filter. convolve impulse response with delayed input
 */
typedef struct delay_struct {
	 	uint32_t sample_delay;	// number of samples to delay by ie. how long the delay is
		uint32_t delay_gain;	// volume of delayed signal
		float* h;				// array holding impulse response
} DELAY_STRUCT;

// allocate memory for delay buffer
// increasing the amount of values stored in the buffer will increase the amount of delay of the signal

/**
 * @brief initialize the delay struct used for FIR calculations
 * 
 * @param time_delay user controlled amount of time to delay the signal
 * @param delay_gain volume of the delayed signal
 * @return pointer to the delay struct 
 */
DELAY_STRUCT* init_delay(		
		uint32_t time_delay,
		uint32_t delay_gain	
);

// delay the signal by the appropriate amount of time
// using stm 16bit FIR filtering







/**
 * DELAY END **********************************************************************************************************
 */
