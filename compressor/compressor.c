/**
 * @file compressor.c
 *
 * @author Jacob Allenwood
 * @date October 24, 2015
 *
 * @brief This file contains the functions for the compressor effect portion 
 * of the GAPE suite.
 * 
 */

#include <stdio.h>
#include <stdlib.h>
 
#include "compressor.h"




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



 void calc_compressor(COMP_T * C, float * rms_vals, float * input) {

 	int i;

 	// for every sample, compress if rms value breaches threshold
 	for(i = 0; i < C->block_size; i++) {
 		if(rms_vals[i] > C->threshold_rms) {
 			C->output[i] = input[i] * C->ratio;
 		} else {
 			C->output[i] = input[i];
 		}
 	}

 }