/**
 * @file test_delay.c
 *
 * @author Jacob Allenwood
 * @date October 22, 2015
 *
 * @brief This file contains the main program to test the delay effect.
 * 
 */

// include files -------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>

#include "delay.h"

// ---------------------------------------------------------------------

#define FS 10



int main(int argc, char const *argv[]) {


	int block_size, i, j;
	block_size = 10;
	float input[10] = {0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9};


	// setup delay struct
	DELAY_T * D = init_delay(FS, 0.1, 1.0, block_size);

	calc_delay(D, input);

	for(i = 0; i < block_size; i++) {
		printf("%f,", D->output[i]);
	}
	printf("\n\n");

	calc_delay(D, input);

	for(j = 0; j < block_size; j++) {
		printf("%f,", D->output[j]);
	}

	printf("\n\n");

}




