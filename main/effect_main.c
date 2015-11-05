/**
 * @file effect_main.c
 *
 * @author Jacob Allenwood
 * @date September 1, 2015
 *
 * @brief This file contains the main program to run the selected GAPE effect
 * 
 */

// include files -------------------------------------------------------

#include "stm32f4xx_hal.h"

#ifdef STM32F407xx
#include "stm32f4_discovery.h"
#endif /* STM32F407xx */

#ifdef STM32F429xx
#include "stm32f429i_discovery.h"
#include "stm32f429i_discovery_lcd.h"
#endif /* STM32F429xx */

#include "arm_math.h"
#include "ece486.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "delay.h"
#include "calc_rms.h"
#include "eq.h"

#include "fir_lowpass.h"
// #include "iir_low.h"
// #include "iir_mid.h"
// #include "iir_high.h"

// ---------------------------------------------------------------------

#define FS 48000



int main(int argc, char const *argv[]) {

	// SELECTEFFECT -----------------------------------------------------------------------

	// effect { effect, appropriate parameters for effect }
	// delay = { 1, time_delay, delay_gain }
	// compressor = { 2, threshold, ratio }
	// equalizer = { 3, lowband_gain, midband_gain, highband_gain }

	float effects[4] = {3, 0, 0, 0};
	int effect = effects[0];

	// declare variables used for effects assigned in switch cases ----------
	// cannot declare variables in switch case
	
	// switch delay --------------
	DELAY_T * D; 	// delay struct
	float time_delay, delay_gain; 

	// switch compressor ---------
	RMS_T * V; 		// rms struct
	float threshold, ratio;
	COMP_T * C;		// comp struct

	// switch eq -----------------
	EQ_T * Q;		// eq struct
	float low_gain, mid_gain, high_gain;

	// ------------------------------------------------------------------------------------


	// set up serial buffer
	char outstr[100];

	// Set up the DAC/ADC interface
	initialize(FS_48K, MONO_IN, STEREO_OUT); 

	int block_size, i;
	float * input, * output1, * output2, * lpf_samples_output;

	// allocate memory --------------------------------------------------------------------
	block_size = getblocksize();

	input = (float *)malloc(sizeof(float) * block_size);
	output1 = (float *)malloc(sizeof(float) * block_size);
	output2 = (float *)malloc(sizeof(float) * block_size);
	lpf_samples_output = (float *)malloc(sizeof(float) * block_size);

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

	
	
 	// float coef[10] = {2.2044, 0.0, 2.2044, 0.6088, -0.9702,
 	// 						2.9658, -3.4865, 2.9658, 0.3500, 0.4250};

 	// // state buffer used by arm routine of size 2*NUM_SECTIONS
 	// float *state = (float *)malloc(sizeof(float)*(2*2));
 	// // arm biquad structure initialization
 	// arm_biquad_cascade_df2T_instance_f32 f1;
 	// arm_biquad_cascade_df2T_init_f32(&f1,2,&coef[0],state);

	// INIT EFFECTS -----------------------------------------------------------------------

	switch(effect) {
		case 1:
			// DELAY ---------------------------------------------
			
			time_delay = effects[1];
			delay_gain = effects[2];
			// initialize delay structure for delay routine 
			D = init_delay(FS, time_delay, delay_gain, block_size);
			if(D == NULL) { flagerror(MEMORY_ALLOCATION_ERROR); return 1; }	// errcheck malloc
			
			break;

 		case 2:
			// COMPRESSOR ----------------------------------------

			// initialize rms detection
			V = init_rms(64, block_size);
			if(V == NULL) { flagerror(MEMORY_ALLOCATION_ERROR); return 1; }	// errcheck malloc
			
			// initialize compressor
			C = init_compressor();

			break;

		case 3:
			// EQ ------------------------------------------------

			low_gain = effects[1];
			mid_gain = effects[2];
			high_gain = effects[3];
			// initialize eq
			Q = init_eq(low_gain, mid_gain, high_gain, block_size);
			if(Q == NULL) { flagerror(MEMORY_ALLOCATION_ERROR); return 1; }

			break;

		default:

			break;
	}





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


		// CALCULATE EFFECTS --------------------------------------------------------------

		switch(effect) {
			case 1:
				// DELAY ---------------------------------------------

				// delay the guitar signal by D->sample_delay samples
				calc_delay(D, lpf_samples_output);

				// pass buffers for output to the dac
				putblockstereo(output1, D->output);
				
				break;

	 		case 2:
				// COMPRESSOR ------------------------------------------------------------

				// detect RMS level
				calc_rms(V, lpf_samples_output);

	 		// 	for (i = 0; i < block_size; ++i) {
				// 	output2[i] = (0.0 * 0.6667) - 1.0;
				// }
	 			// arm_rms_f32(lpf_samples_output, block_size, output2);
				
				// compress
				calc_compressor(C, V->output, lpf_samples_output);

				// pass buffers for output to the dac
				putblockstereo(output1, V->output);
				// putblockstereo(output1, output2);

				break;

			case 3:
				// EQ

				// adjust freq bands with equalizer
				calc_eq(Q, lpf_samples_output);

		    	// arm_biquad_cascade_df2T_f32(&f1, input, output2, block_size);
		    	// for(i=0;i<block_size;i++) {
		    	// 	output2[i] = output2[i] * 0.01718740;
		    	// }
				// pass buffers for output to the dac
				putblockstereo(output1, Q->output);
				// putblockstereo(output1, output2);

				break;

			default:

				break;
		}



	}

}




