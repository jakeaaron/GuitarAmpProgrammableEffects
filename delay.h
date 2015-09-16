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


#include <stdint.h>



/**
 * DELAY **************************************************************************************************************
 */

typedef struct delay_struct {
	// data structure contains number of coefs, pointer to the coef array
	arm_fir_instance_f32 S;	// and pointer to state variable buffer
	uint32_t sample_delay;	// amount of delay
	float32_t delay_gain;	// amplitude of delayed signal
	uint32_t block_size;	// number of samples to work on
} DELAY_T;



DELAY_T * init_delay(
	uint32_t FS,			// sampling frequency
	float32_t time_delay,	// amount of delay 
	float32_t delay_gain,	// volume of delayed signal
	uint32_t block_size		// number of samples to work on
);

void delay(
	DELAY_T * T,		// struct containing delay information
	float32_t * input,	// input buffer
	float32_t * output	// output buffer
);


