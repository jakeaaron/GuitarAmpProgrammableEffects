/**
 * @file eq.c
 *
 * @author Jacob Allenwood
 * @date October 28, 2015
 *
 * @brief This file contains the functions for the 3 band equalizer portion 
 * of the GAPE suite.
 * 
 * @details The low band is 0 to 350 Hz, the mid band is 350 to 1050 Hz
 * and the high band is 1050 to 10K (might need to add another filter to cut at 5K).
 * The idea is to use two fir filters to split the bands into bands below and above the cutoff
 * frequencies for the filters. By subtracting the input samples by the filtered samples, we access
 * all the info in the rest of the frequency spectrum that isn't being filtered by that filter. 
 * The low band filter sections off the low band, and subtracting the input to the filter by those filtered 
 * samples gives the rest of the spectrum (350Hz to 10Khz). Then by adding another lowpass filter that cuts off 
 * at 1050Hz it allows us to manipulate the samples between 350 and 1050Hz. Subtracting the input to the filter by 
 * this filter's output then gives the rest of the frequency band (1050Hz to 10Khz: 10K because of the fir already 
 * filtering the input guitar signal).
 * Every time the samples go through a filter, the input to the filter also goes through a delay equal to the 
 * amount of delay caused by the filtering routine. This makes it so that when we subtract the input samples by the 
 * filtered samples to get the high band for that filter, the signals are still in phase with each other.
 * 
 * Implementing the equalizer with fir filters rather than iir filters allows for "perfect reconstruction", where
 * when the bands are put back together there is no distortion or odd constructive or destructive interference in
 * the transition bands. Basically, when all bands are set to a flat response, it should output an 
 * untainted and undistorted flat response because each band was calculated from the other bands.
 * 
 */


// INCLUDE -----------------------------------------------------------------

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "arm_math.h"

#include "delay.h"
#include "eq.h"

#include "eq_low_coefs.h"
#include "eq_mid_coefs.h"

// -------------------------------------------------------------------------




/**
 * @brief [initialize eq struct for equalizer routines]
 * 
 * @param low_gain [bass gain in dB]
 * @param mid_gain [mid gain in dB]
 * @param high_gain [treble gain in dB]
 * @param block_size [number of samples to work on]
 * @param FS [sampling frequency used in the delay routine]
 * @return [pointer to the eq struct]
 */
EQ_T * init_eq(float low_gain, float mid_gain, float high_gain, int block_size, int FS) {

	int i, j;

	// set up struct for eq -------------------------------------------------------------------------------------
	EQ_T * Q = (EQ_T *)malloc(sizeof(EQ_T));	// allocate struct
	if(Q == NULL) return NULL;					// errcheck malloc call
	
	Q->block_size = block_size;

	// pow(dB / 20) = gain
	Q->low_scale = pow(10, (low_gain / 20.0));
	Q->mid_scale = pow(10, (mid_gain / 20.0));
	Q->high_scale = pow(10, (high_gain / 20.0));


	// initialize delays for keeping the outputs in phase with each other ---------------------------------------
	// delay the same amount as the delay caused by the fir routine
	// both filters have the same number of coefs, so the delays will be the same
	int sample_delay = ((eq_low_num - 1) / 2);	// delay for fir is (M-1)/2
	Q->D1 = init_delay(0, FS, sample_delay, 1, block_size); // 0 is delay in samples instead of in seconds
	Q->D2 = init_delay(0, FS, sample_delay, 1, block_size);
	Q->D3 = init_delay(0, FS, sample_delay, 1, block_size);
	if(Q->D1 == NULL || Q->D2 == NULL || Q->D3 == NULL) return NULL; 


	// declare and initialize variables necessary for arm fir routines ------------------------------------------
	float * low_state = (float *)malloc(sizeof(float) * (eq_low_num + block_size - 1));
	float * mid_state = (float *)malloc(sizeof(float) * (eq_mid_num + block_size - 1));
	if(low_state == NULL || mid_state == NULL) return NULL; 

	// declare arm struct
	arm_fir_instance_f32 S_low;
	arm_fir_instance_f32 S_mid;

	// initialize arm struct
	arm_fir_init_f32(&S_low, eq_low_num, &(eq_low_coefs[0]), low_state, block_size);
	arm_fir_init_f32(&S_mid, eq_mid_num, &(eq_mid_coefs[0]), mid_state, block_size);

	Q->S_low = S_low;
	Q->S_mid = S_mid;


	// initialize band output buffers --------------------------------------------------------------------------
	Q->low_band_out = (float *)malloc(sizeof(float) * block_size);
	Q->mid_band_out = (float *)malloc(sizeof(float) * block_size);
	Q->high_band_out = (float *)malloc(sizeof(float) * block_size);
	if(Q->low_band_out == NULL || Q->mid_band_out == NULL || Q->high_band_out == NULL) return NULL; 
	for(i = 0; i < block_size; i++) {
		Q->low_band_out[i] = 0.0;
		Q->mid_band_out[i] = 0.0;
		Q->high_band_out[i] = 0.0;
	}


	// initialize eq output buffer -----------------------------------------------------------------------------
	Q->output = (float *)malloc(sizeof(float) * block_size);
	if(Q->output == NULL) return NULL;
	for(j = 0; j < block_size; j++) {
		Q->output[j] = 0.0;
	}


	// return pointer to struct---------------------------------------------------------------------------------
	return Q;

}


/**
 * @brief [calculate equalized output samples]
 * @details [this routine uses fir lowpass filters and delays to split off the spectrum
 * into the specified bands. 
 * The input signal is low pass filtered to split the spectrum, and also separately delayed to 
 * keep the input in phase with the output of the filters. This delayed input is used to find the 
 * rest of the spectrum not being filtered, by subtracting the input by the filtered samples. So using
 * two filters allows us to split off 3 different bands: the band below and the band above the first lowpass 
 * filter, and the band below and the band above the second lowpass filter. The band above the first lowpass
 * and the band below the second lowpass is the same band.]
 * 
 * @param D1 [pointer to the delay struct]
 * @param D2 [pointer to the delay struct]
 * @param D3 [pointer to the delay struct]
 * @param Q [pointer to the eq struct]
 * @param input [buffer containing samples to work on]
 */
void calc_eq(DELAY_T * D1, DELAY_T * D2, DELAY_T * D3, EQ_T * Q, float * input) {

	int i, j, k;

	// LOW BAND ------------------------------------------------------------------------------------------------
	// calculate low band output with no gain
	// lowpass with cutoff of 350Hz
	arm_fir_f32(&(Q->S_low), input, Q->low_band_out, Q->block_size);

	// delay filter output to stay in phase with mid and high band for reconstructing output
	calc_delay(0, D1, Q->low_band_out);	// D1 now contains the low band output


	// MID BAND ------------------------------------------------------------------------------------------------
	// delay signal to stay in phase for mid band calculation
	calc_delay(0, D2, input);	// 0 is to output just the delayed signal

	// input for mid band is the delayed signal minus the low band
	// this gives the samples for the rest of the spectrum that the low band doesn't cover
	for(j = 0; j < Q->block_size; j++) {
		D2->output[j] = D2->output[j] - Q->low_band_out[j];
	}
	// lowpass with cutoff of 1050Hz
	// this contains the band from the cutoff of the low band, to 1050Hz
	arm_fir_f32(&(Q->S_mid), D2->output, Q->mid_band_out, Q->block_size);


	// HIGH BAND -----------------------------------------------------------------------------------------------
	// delay signal to stay in phase for high band calculation (note the input was already delayed once for the mid band calc)
	calc_delay(0, D3, D2->output);	// 0 is to output just the delayed signal

	// the high band is the input going into the mid filter delayed, and then subtracted from the mid filter output
	// this gives the samples for the rest of the spectrum that the low and mid band doesn't cover
	for(k = 0; k < Q->block_size; k++) {
		Q->high_band_out[k] = D3->output[k] - Q->mid_band_out[k];
	}


	// calculate block of equalized output samples -------------------------------------------------------------
	for(i = 0; i < Q->block_size; i++) {
		// output is the output of each band scaled by the band gain and added together 
		Q->output[i] = 0.5 * ((Q->low_scale * D1->output[i]) + (Q->mid_scale * Q->mid_band_out[i]) + (Q->high_scale * Q->high_band_out[i]));
	}
	
}