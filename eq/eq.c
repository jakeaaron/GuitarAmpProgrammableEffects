/**
 * @file eq.c
 *
 * @author Jacob Allenwood
 * @date October 28, 2015
 *
 * @brief This file contains the functions for the 3 band equalizer portion 
 * of the GAPE suite.
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include "ece486.h"

#include "eq.h"









// void arm_scale_f32	(	float32_t * 	pSrc,
// 		float32_t 	scale,
// 		float32_t * 	pDst,
// 		uint32_t 	blockSize 
// )	

// dB = 20log10(RMS)






EQ_T * init_eq(RMS_T * R, float low_gain, float mid_gain, float high_gain) {
	// function parameters are in db
	// 10pow(dB / 20) = RMS_gain
	// RMS_gain / RMS_actual = band_scale

	// set up struct for eq --------------------------------
	EQ_T * Q = (EQ_T *)malloc(sizeof(EQ_T));	// allocate struct
	if(Q == NULL) return NULL;					// errcheck malloc call
	
	Q->low_scale = (10 * pow(10, (low_gain / 20.0))) / (R->output[R->index - 1]);
	

}

