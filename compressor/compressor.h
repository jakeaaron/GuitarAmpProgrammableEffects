/**
 * @file compressor.h
 *
 * @author Jacob Allenwood
 * @date October 24, 2015
 *
 * @brief This file contains subroutine and data-type declarations necessary for 
 * the compressor effect.
 * 
 */

#ifndef COMPRESSOR
#define COMPRESSOR
 
#include <stdint.h>


/**
 * @brief [structure containing necessary fields for the compressor calculations]
 * 
 */
typedef struct comp_struct {
	float threshold_rms;	// rms level to pass for compressor to kick in
	float ratio;			// amount to compress by
	float * output;			// buffer containing output (compressed samples)

} COMP_T;



COMP_T * init_compressor(
	float threshold_dB,		// level in dB 
	float ratio				// amount to compress by
);



void calc_compressor(
	COMP_T * C,
	float * rms_vals,
	float * input
);


#endif