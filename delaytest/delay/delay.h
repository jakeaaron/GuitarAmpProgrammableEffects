/**
 * @file delay.h
 *
 * @author Jacob Allenwood
 * @date September 1, 2015
 *
 * @brief This file contains subroutine and data-type declarations necessary for 
 * the GAPE unit.
 * 
 */

#ifndef DELAY
#define DELAY

#include <stdint.h>



/**
 * DELAY **************************************************************************************************************
 */

typedef struct delay_struct {
	// data structure contains number of coefs, pointer to the coef array
	float sample_delay;		// amount of delay
	float * delay_coefs;	// array holding the impulse response for the delayed signal
} DELAY_T;


DELAY_T * init_delay(
	float FS,			// sampling frequency
	float time_delay,	// amount of delay 
	float delay_gain	// volume of delayed signal
);


// void delay(
// 	DELAY_T * T,	// struct containing delay information
// 	float * input,	// input buffer
// 	float * output	// output buffer
// );

#endif
