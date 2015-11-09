/**
 * @file compressor.c
 *
 * @author Jacob Allenwood
 * @date October 24, 2015
 *
 * @brief This file contains the functions for the compressor effect portion 
 * of the GAPE suite.
 * 
 * @details
 * 
 */


// INCLUDE ----------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "ece486.h"
 
#include "compressor.h"

// ------------------------------------------------------------




/**
 * @brief [initialize compressor structure necessary for compressor calculation]
 * @details [long description]
 * 
 * @param threshold_db [level in db to pass for compressor to kick in]
 * @param ratio [amount to compress by once the threshold is passed]
 * @param block_size [amount of samples to work on]
 * @return [pointer to the compressor struct]
 */
 COMP_T * init_compressor(float threshold_db, float ratio, int block_size) {

 	int i;


 	// initialize compressor struct ----------------------------------------
 	COMP_T * C = (COMP_T *)malloc(sizeof(COMP_T));
 	if(C == NULL) return NULL;

	// dB = 20log10(RMS)
 	C->threshold_rms = pow(10, (threshold_db / 20.0));
 	C->ratio = ratio;
 	C->block_size = block_size;

 	// initialize compressor output buffer
 	C->output = (float *)malloc(sizeof(float) * block_size);
 	if(C->output == NULL) return NULL;
 	for(i = 0; i < block_size; i++) {
 		C->output[i] = 0.0;
 	}


 	// return pointer to comp struct ---------------------------------------
 	return C;

 }


/**
 * @brief [compresses the input signal once the rms level passes the threshold entered into the initialize function]
 * @details [long description]
 * 
 * @param C [pointer to the compressor struct]
 * @param rms_vals [the rms values of the input, to check and see if the input needs to be compressed]
 * @param input [buffer containing samples to work on]
 */
 void calc_compressor(COMP_T * C, float * rms_vals, float * input) {

 	int i;
 	

 	// for every sample, compress if rms value breaches threshold -----------------------------------------
 	for(i = 0; i < C->block_size; i++) {
 		if(rms_vals[i] > C->threshold_rms) {
 			// C->output[i] = 20 * log10((C->threshold_rms + ((input[i] - C->threshold_rms) / C->ratio)));
 			C->output[i] = input[i] * 0.75;
 			// C->output[i] = 0;
 		} else {
 			C->output[i] = input[i];
 			// C->output[i] = 0;
 		}
 	}

 }