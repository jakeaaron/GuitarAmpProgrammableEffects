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
#include <stdlib.h>
#include <stdio.h>

#include "calc_rms.h"

// ---------------------------------------------------------------------




// this is a very minimal test, but I did the calculation by hand, 
 // the matlab test script, and this routine, and found the same answer of 
 // 0.5339
 // More extensive testing, for multiple blocks should probably be implemented.

int main(int argc, char const *argv[]) {


	int block_size, i, j;
	block_size = 10;
	float input[10] = {0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9};


	// setup rms struct
	RMS_T * V = init_rms(10, block_size);

	calc_rms(V, input);

	// for(i = 0; i < block_size; i++) {
	// 	printf("%f,", V->output[i]);
	// }

	printf("%f", V->output[block_size - 1]);


}




