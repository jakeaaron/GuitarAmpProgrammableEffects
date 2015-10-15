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
 * DELAY **************************************************************************************************************
 */

/**
 * @brief [brief description]
 * @details [long description]
 * 
 */
typedef struct delay_struct {
	// data structure contains number of coefs, pointer to the coef array
	float sample_delay;		// amount of delay
	float * delay_coefs;	// array holding the impulse response for the delayed signal
} DELAY_T;


/**
 * 
 */
DELAY_T * init_delay(
	float FS,			// sampling frequency
	float time_delay,	// amount of delay 
	float delay_gain	// volume of delayed signal
);


#endif
