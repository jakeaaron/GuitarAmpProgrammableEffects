/**
 * @file effect_main.c
 *
 * @author Jacob Allenwood
 * @date September 1, 2015
 *
 * @brief This file contains the main program to run the selected GAPE effect
 * 
 * @details
 * 
 */


// INCLUDE -------------------------------------------------------

#include "stm32f4xx_hal.h"
#include "stm32f4_discovery.h"

#include "arm_math.h"
#include "ece486.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "delay.h"
#include "calc_rms.h"
#include "compressor.h"
#include "eq.h"

#include "fir_lowpass.h"

// ---------------------------------------------------------------------


// DEFINES -------------------------------------------------------------

#define FS 48000

// ---------------------------------------------------------------------


 

int main(int argc, char const *argv[]) {

	// SELECTEFFECT ------------------------------------------------------------------------------------

	// effect { effect, appropriate parameters for effect }
	// delay = { 1, time_delay, delay_gain }
	// compressor = { 2, threshold, ratio }
	// equalizer = { 3, lowband_gain, midband_gain, highband_gain }

	float effects[4] = {2, 0, 0, 0};
	int effect = effects[0];

	// declare variables used for effects assigned in switch cases ----------
	// cannot declare variables in switch case
	
	// switch delay --------------
	DELAY_T * D; 	// delay struct
	float delay, delay_gain; 

	// switch compressor ---------
	RMS_T * V; 		// rms struct
	float threshold, ratio;
	COMP_T * C;		// comp struct

	// switch eq -----------------
	EQ_T * Q;		// eq struct
	float low_gain, mid_gain, high_gain;

	// -------------------------------------------------------------------------------------------------


	// set up serial buffer
	char outstr[100];
	int block_size, i;

	// Set up the DAC/ADC interface
	initialize(FS_48K, MONO_IN, STEREO_OUT); 

	block_size = getblocksize();


	// allocate memory --------------------------------------------------------------------
	float * input = (float *)malloc(sizeof(float) * block_size);
	float * output1 = (float *)malloc(sizeof(float) * block_size);
	float * output2 = (float *)malloc(sizeof(float) * block_size);
	float * lpf_samples_output = (float *)malloc(sizeof(float) * block_size);
	if(input == NULL || output1 == NULL || output2 == NULL || lpf_samples_output == NULL) {
		flagerror(MEMORY_ALLOCATION_ERROR);
		while(1);
	} 


	// initialize lowpass arm_fir filter to filter input guitar signal --------------------
	// setup state variable array used by arm_fir routine
	float * fir_state = (float *)malloc(sizeof(float) * (BL + block_size - 1));
	// initialize arm_fir struct
	arm_fir_instance_f32 S;
	// ceofs found in fir_lowpass.h
	arm_fir_init_f32(&S, BL, &(B[0]), fir_state, block_size);

	
	

	// INIT EFFECTS ---------------------------------------------------------------------------------------------------------------

	switch(effect) {
		case 1: // DELAY ---------------------------------------------
			
			delay = effects[1];												// this is delay in seconds
			delay_gain = effects[2];
			// initialize delay structure for delay routine 
			D = init_delay(1, FS, delay, delay_gain, block_size);			// 1 means delay is in seconds
			if(D == NULL) { flagerror(MEMORY_ALLOCATION_ERROR); return 1; }	// errcheck malloc
			
			break;

 		case 2: // COMPRESSOR ----------------------------------------

			// initialize rms detection
			V = init_rms(block_size, block_size);
			if(V == NULL) { flagerror(MEMORY_ALLOCATION_ERROR); return 1; }	// errcheck malloc
			
			// initialize compressor
			threshold = -8;	// 0 is 1VRMS
			ratio = 20;
			C = init_compressor(threshold, ratio, block_size);

			break;

		case 3: // EQ ------------------------------------------------

			low_gain = effects[1];
			mid_gain = effects[2];
			high_gain = effects[3];
			// initialize eq
			Q = init_eq(low_gain, mid_gain, high_gain, block_size, FS);
			if(Q == NULL) { flagerror(MEMORY_ALLOCATION_ERROR); return 1; }

			break;

		default:

			break;
	}

	// --------------------------------------------------------------------------------------------------------------------------



	// process input data stream, "block_size" samples at a time
	while(1) {

		// get input samples from adc
		getblock(input);	// Wait here until the input buffer is filled... Then process	

    	// lowpass filter the input guitar signal
    	arm_fir_f32(&S, input, lpf_samples_output, block_size);

    	// output the input samples
		for (i = 0; i < block_size; ++i) {
			output1[i] = lpf_samples_output[i];
		}


		// CALCULATE EFFECTS -------------------------------------------------------------------------------------------

		switch(effect) {
			case 1: // DELAY -----------------------------------------------------------------
				

				// delay the guitar signal by D->sample_delay samples
				calc_delay(1, D, lpf_samples_output);	// 1 is to add delay to input signal

				// pass buffers for output to the dac
				putblockstereo(output1, D->output);
				
				break;

	 		case 2:	// COMPRESSOR ------------------------------------------------------------

			
				// detect RMS level
				calc_rms(V, lpf_samples_output);

				// compress
				calc_compressor(C, V->output, lpf_samples_output);
			

				
	 			for (i = 0; i < block_size; ++i) {
					output1[i] = (V->output[i] * 0.6667) - 1.0;
				}
				// pass buffers for output to the dac
				putblockstereo(output1, C->output);
				// putblockstereo(output1, output2);

				break;

			case 3:	// EQ -------------------------------------------------------------------
				

				// adjust freq bands with equalizer
				calc_eq(Q->D, Q, lpf_samples_output);

				// pass buffers for output to the dac
				putblockstereo(lpf_samples_output, Q->output);

				break;

			default:

				break;
		}

		// -------------------------------------------------------------------------------------------------------------


	}

}