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
#include "ece486.h"
 
#include "compressor.h"




 COMP_T * init_compressor(float threshold_db, float ratio) {

 	COMP_T * C = (COMP_T *)malloc(sizeof(COMP_T));
 	if(C == NULL) return NULL;

	// dB = 20log10(RMS)
 	C->threshold_rms = pow(10, (threshold_db / 20.0));
 	C->ratio = ratio;

 }



 void calc_compressor(COMP_T * C, float * rms_vals, float * input) {

 }