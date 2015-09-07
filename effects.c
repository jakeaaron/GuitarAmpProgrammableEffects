/**
 * @file effects.c
 *
 * @author Jacob Allenwood
 * @date September 1, 2015
 *
 * @brief This file contains the functions for each effect in the 
 * GAPE unit. This program will be passed parameters to control 
 * the way each effect behaves.
 * 
 */

#include "effects.h"

/**
 * DELAY **************************************************************************************************************
 */

/**
 * @brief [brief description]
 * @details [long description]
 * 
 * @param time_delay [description]
 * @param delay_gain [description]
 * 
 * @return [description]
 */
DELAY_STRUCT* init_delay(uint32_t time_delay, uint32_t delay_gain) {
	// set up structure
	DELAY_STRUCT* s;					// instance of delay struct
	s = malloc(sizeof(DELAY_STRUCT));	// allocate mem for the struct

	// set up delay parameters
	s->sample_delay = (time_delay * Fs);// number of samples to delay by
	s->delay_gain = delay_gain;			// volume of delayed signal
	
	// allocate memory for delay buffer
	s->h = malloc(sizeof(float) * s->sample_delay);	

	// set up impulse response
	s->h[0] = 1; 	// impulse at the origin
	s->h[s->sample_delay - 1] = delay_gain;	// impulse of the gain strength at the delayed sample index
}

// delay the signal by the appropriate amount of time
/**
 * @brief [brief description]
 * @details [long description]
 * 
 * @param s [description]
 * @param x [description]
 * @return [description]
 */
DELAY_STRUCT* calc_delay(DELAY_STRUCT* s, float x) {
	
}