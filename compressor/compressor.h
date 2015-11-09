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


// HEADER DEFINITION -------------------------------------------------

#ifndef COMPRESSOR
#define COMPRESSOR
 
// -------------------------------------------------------------------


// INCLUDE -----------------------------------------------------------

#include <stdint.h>

// -------------------------------------------------------------------




/**
 * @brief [structure containing necessary fields for the compressor calculations]
 * 
 */
typedef struct comp_struct {
	float threshold_rms;	// rms level to pass for compressor to kick in
	float ratio;			// amount to compress by
	float * output;			// buffer containing output (compressed samples)
	int block_size;
} COMP_T;


/**
 * @brief [initialize compressor structure necessary for compressor calculation]
 * @details [long description]
 * 
 * @param threshold_db [level in db to pass for compressor to kick in]
 * @param ratio [amount to compress by once the threshold is passed]
 * @param block_size [amount of samples to work on]
 * @return [pointer to the compressor struct]
 */
COMP_T * init_compressor(
	float threshold_dB,		// level in dB 
	float ratio,			// amount to compress by
	int block_size			// number of samples to work on
);


/**
 * @brief [compresses the input signal once the rms level passes the threshold entered into the initialize function]
 * @details [long description]
 * 
 * @param C [pointer to the compressor struct]
 * @param rms_vals [the rms values of the input, to check and see if the input needs to be compressed]
 * @param input [buffer containing samples to work on]
 */
void calc_compressor(
	COMP_T * C,				// pointer to comp struct
	float * rms_vals,		// block_size number of rms values from rms routine
	float * input			// buffer containing input samples from adc
);	


#endif