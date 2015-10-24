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
	float threshold;		// level to pass for compressor to kick in
	float ratio;			// amount to compress by

} COMP_T;




#endif