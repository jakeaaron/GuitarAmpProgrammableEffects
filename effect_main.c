/**
* effects_main.c
*/

// include files -------------------------------------------------------

#include "stm32f4xx_hal.h"

#ifdef STM32F407xx
#include "stm32f4_discovery.h"
#endif /* STM32F407xx */

#ifdef STM32F429xx
#include "stm32f429i_discovery.h"
#include "stm32f429i_discovery_lcd.h"
#endif /* STM32F429xx */

#include "arm_math.h"
#include "ece486.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "delay.h"

// ---------------------------------------------------------------------



/**
 * @brief [brief description]
 * @details [long description]
 * 
 * @param argc [description]
 * @param argv [description]
 * 
 * @return [description]
 */
int main(int argc, char const *argv[]) {

	// set up serial buffer
	char outstr[100];

	// Set up the DAC/ADC interface
	initialize(FS_48K, MONO_IN, STEREO_OUT); 

	int block_size, i;
	float *input, *output1, *output2;

	/**
	 * Allocate memory -------------------------------------------------------- 
	 */ 
	block_size = getblocksize();

	input = (float *)malloc(sizeof(float)*block_size);
	output1 = (float *)malloc(sizeof(float)*block_size);
	output2 = (float *)malloc(sizeof(float)*block_size);

	if (input == NULL || output1 == NULL || output2 == NULL) {
		flagerror(MEMORY_ALLOCATION_ERROR);
		while(1);
	} 
	// ----------------------------------------------------------------------


	// setup delay impulse response / fir filter coefs
	DELAY_T * D = init_delay(FS_48K, 0.25, 0.5);
	if(D == NULL) {
		flagerror(MEMORY_ALLOCATION_ERROR);
		while(1);
	}

	// setup state variable array used by arm_fir routine
	float * state = (float *)malloc(sizeof(float) * (D->sample_delay + block_size - 1));

	// initialize arm_fir struct
	arm_fir_instance_f32 S;
	arm_fir_init_f32(&S, D->sample_delay, &(D->delay_coefs[0]), state, block_size);

	/*
	* Infinite Loop to process the data stream, "block_size" samples at a time
	*/
	while(1) {
		/*
		 * Ask for a block of ADC samples to be put into the working buffer
		 *   getblock() will wait until the input buffer is filled...  On return
		 *   we work on the new data buffer.
		 */
		getblock(input);	// Wait here until the input buffer is filled... Then process	

		/*
		 * signal processing code to calculate the required output buffers
		 */
		DIGITAL_IO_SET(); 	// Use a scope on PC4 to measure execution time

		// sprintf(outstr,"YAYAYYAYY"); // %d, %f seem to be buggy
		// UART_putstr(outstr);

		for (i = 0; i < block_size; ++i) {
			output1[i] = input[i];
		}

    	// Call the arm provided FIR filter routine
    	arm_fir_f32(&S, input, output2, block_size);
		
		
		DIGITAL_IO_RESET();	// (falling edge....  done processing data )

		/*
		 * pass the processed working buffer back for DAC output
		 */
		putblockstereo(output1, output2);

	}
}




