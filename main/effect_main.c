/**
 * @file effect_main.c
 *
 * @author Jacob Allenwood
 * @date September 1, 2015
 *
 * @brief This file contains the main program to run the selected GAPE effect; either a delay, a compressor, or an equalizer.
 * 
 * @setup []
 * 
 * @details [
 * At the start of this program, the usart_read function is called to find out what effect should be run. The function 
 * waits for the RX_Complete flag to be set by the receive complete call back function. This receive complete function is 
 * called after the HAL DMA recieve interrupt is completed, i.e. once the program receives the characters from the python gui. 
 * 
 * After all the variables and buffers needed for the program are declared, the FIR filter that lowpass filters the input guitar
 * signal is initialized using the arm cmsis dsp library routine. This filter is designed to cutoff at 10kHz to filter any 
 * unwanted noise/harmonics before we do the dsp on the signal. The design was based on the frequency spectrum of an electric guitar
 * output, as the range goes out to around 10kHz.
 * 
 * Then the selected effect is initialized with the appropriate inititialize function. These functions initialize the structures needed
 * for their corresponding calculation routines, that actually manipulate the signal to produce the corresponding guitar effect. Before
 * the effect is initialized however, the values go through error checking to make sure the user isn't trying to run the effect with
 * inappropriate paramters. For example, a delay that is not much larger than the contract specified 0.5 seconds causes a memory overflow
 * which completely distorts the output signal and produces garbage. The error checking makes sure that it doesn't use a delay value of 
 * more than 0.5 seconds.
 * 
 * The rest of the program is an infinite loop manipulating the input to produce the appropriate output effect. The input is first lowpass
 * filtered with the cutoff at 10kHz as previously mentioned, and then continues to call the calculate function corresponding to the 
 * previously called initialize function. 
 * 
 * The program never returns. If an error is caught, then an error led is lit up on the STM32F407-Discovery board and then remains
 * in an infinite loop.
 * ]
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
#include "read_effect.h"

#include "fir_lowpass.h"

// ---------------------------------------------------------------------


// DEFINES -------------------------------------------------------------

#define FS 48000	// sampling frequency of 48kHz

// ---------------------------------------------------------------------


 

int main(int argc, char const *argv[]) {

	// Set up system clock, adc, dac, etc
	initialize(FS_48K, MONO_IN, STEREO_OUT); 

	// GET EFFECT --------------------------------------------------------------------------------------
	// set up buffers for reading the effect and params from gui
	FX_T * F = init_effects_read();
	// F->effect and F->effect_params are updated by the read effect call
	read_effect(F);
	// -------------------------------------------------------------------------------------------------

	// now start the adc and dac since we don't have to wait for an input anymore
	init_dac(FS_48K, STEREO_OUT);
	init_adc(FS_48K, MONO_IN);
	init_uart();

	
	// SELECTEFFECT ------------------------------------------------------------------------------------

	/* effects format:
	effect { effect, appropriate parameters for effect }
	delay = { 1, time_delay, delay_gain }
	compressor = { 2, threshold, ratio }
	equalizer = { 3, lowband_gain, midband_gain, highband_gain } */

	int effect = F->effect;

	// declare variables used for effects assigned in switch cases --------------
	// cannot declare variables in switch case, so we declare all here
	
	// switch delay --------------
	DELAY_T * D; 	// delay struct
	float delay, delay_gain; 

	// switch compressor ---------
	RMS_T * V; 		// rms struct
	int window;
	float threshold, ratio;
	COMP_T * C;		// comp struct

	// switch eq -----------------
	EQ_T * Q;		// eq struct
	float low_gain, mid_gain, high_gain;

	// -------------------------------------------------------------------------------------------------


	// initialize ---------------------------------------------------------------
	int block_size, i;


	// get number of samples in each block (default of 100)
	block_size = getblocksize();


	// allocate memory for input and output buffers -----------------------------
	float * input = (float *)malloc(sizeof(float) * block_size);
	float * output1 = (float *)malloc(sizeof(float) * block_size);
	float * output2 = (float *)malloc(sizeof(float) * block_size);
	float * lpf_samples_output = (float *)malloc(sizeof(float) * block_size);
	if(input == NULL || output1 == NULL || output2 == NULL || lpf_samples_output == NULL) {
		flagerror(MEMORY_ALLOCATION_ERROR);
		while(1);
	} 


	// initialize lowpass arm_fir filter to filter input guitar signal to 10K ---
	// setup state variable array used by arm_fir routine
	float * fir_state = (float *)malloc(sizeof(float) * (BL + block_size - 1));
	if(fir_state == NULL) { flagerror(MEMORY_ALLOCATION_ERROR); while(1); }
	// initialize arm_fir struct
	arm_fir_instance_f32 S;
	// ceofs found in fir_lowpass.h
	arm_fir_init_f32(&S, BL, &(B[0]), fir_state, block_size);	
	
	

	// INIT EFFECTS ----------------------------------------------------------------------------------------------------

	switch(effect) {
		case 1: // DELAY --------------------------------------------------------
			
			delay = R->rx_string[1];		// this is delay in seconds
			if(delay > 0.5) { flagerror(DEBUG_ERROR); while(1); }		// don't delay more than half a second
			delay_gain = R->rx_string[2];
			if(delay_gain > 1) { flagerror(DEBUG_ERROR); while(1); }	// limit output vol to input vol

			// initialize delay structure for delay routine 
			D = init_delay(1, FS, 0.5, 1, block_size);		// 1 means delay is in seconds
			if(D == NULL) { flagerror(MEMORY_ALLOCATION_ERROR); while(1); }	
			
			break;

 		case 2: // COMPRESSOR ---------------------------------------------------

			// initialize rms detection -----------
 			window = 100 * block_size;

			V = init_rms(window, block_size);
			if(V == NULL) { flagerror(MEMORY_ALLOCATION_ERROR); while(1); }
			
			// initialize compressor --------------
			// threshold = R->rx_string[1];	// 0db entered is 1VRMS
			// if(threshold > 6) { flagerror(DEBUG_ERROR); while(1); } // limit threshold to the max rms voltage the board is capable of
			// ratio = R->rx_string[2];
			// if(ratio <= 0) { flagerror(DEBUG_ERROR); while(1); }	// limit ratio to positive value

			// C = init_compressor(threshold, ratio, block_size);
			C = init_compressor(-7, 10, block_size);
			if(C == NULL) { flagerror(MEMORY_ALLOCATION_ERROR); while(1); }

			break;

		case 3: // EQ -----------------------------------------------------------

			// initialize eq
			// limit the gain for each band to +-15dB
			// low_gain = R->rx_string[1];
			// if(low_gain > 15 || low_gain < -15) { flagerror(DEBUG_ERROR); while(1); }
			// mid_gain = R->rx_string[2];
			// if(mid_gain > 15 || mid_gain < -15) { flagerror(DEBUG_ERROR); while(1); }
			// high_gain = R->rx_string[3];
			// if(high_gain > 15 || high_gain < -15) { flagerror(DEBUG_ERROR); while(1); }
			// Q = init_eq(low_gain, mid_gain, high_gain, block_size, FS);
			Q = init_eq(0, 10, 0, block_size, FS);
			if(Q == NULL) { flagerror(MEMORY_ALLOCATION_ERROR); while(1); }

			break;

		default:

			// invalid effect input
			flagerror(DEBUG_ERROR);	// turn on error led
			while(1);
	}

	// -----------------------------------------------------------------------------------------------------------------



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


		// CALCULATE EFFECTS ---------------------------------------------------------------------------------------------------

		switch(effect) {
			case 1: // DELAY -----------------------------------------------------------------
				// delay the guitar signal by D->sample_delay samples
				calc_delay(0, D, lpf_samples_output);	// 1 is to add delay to input signal

				// pass buffers for output to the dac
				putblockstereo(output1, D->output);
				
				break;

	 		case 2:	// COMPRESSOR ------------------------------------------------------------
				// detect RMS level
				calc_rms(V, lpf_samples_output);

				// compress
				calc_compressor(C, V->output, lpf_samples_output);

				// pass buffers for output to the dac
				putblockstereo(output1, C->output);

				break;

			case 3:	// EQ --------------------------------------------------------------------
				// adjust freq bands with equalizer
				calc_eq(Q->D1, Q->D2, Q->D3, Q, lpf_samples_output);

				// pass buffers for output to the dac
				putblockstereo(output1, Q->output);

				break;

			default:

				// invalid effect input
				flagerror(DEBUG_ERROR);	// turn on error led
				while(1);
		}

		// ---------------------------------------------------------------------------------------------------------------------


	}

}