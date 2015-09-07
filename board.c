/*!
 * @file
 * 
 * @brief ECE 486 Spring 2015 Lab 2 Real-time implementation
 * 
 * @author ECE486 Lab Group 2
 * @author Jacob Allenwood, Travis Russell, Jeremiah Simonsen
 * 
 * @date Feb 13, 2015
 * 
 * This file contains the real-time implementation of the FIR and IIR filter
 * subroutines on the STM32F407xx dev board.
 *
 * An input is taken on pin AIN1, the FIR filtered output is sent to pin
 * AOUT1, and the IIR (BIQUAD) filtered output is sent to pin AOUT2
 *
 * The IIR filter demonstrated is the 4th order IIR filter from HW2. The 
 * sampling rate is 48ksps so it will reject 7.2 kHz and 12 kHz.
 *
 * The FIR filter demonstrated is a 20 coefficient lowpass filter with a 3db
 * cutoff frequency of 5 kHz.
 * 
 */

#include "stm32f4xx_hal.h"

#ifdef STM32F407xx
#include "stm32f4_discovery.h"
#endif /* STM32F407xx */

#ifdef STM32F429xx
#include "stm32f429i_discovery.h"
#include "stm32f429i_discovery_lcd.h"
#endif /* STM32F429xx */

#include "ece486.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "ece486_fir.h"
#include "ece486_biquad.h"

int main(void)
{
  int nsamp,i;
  float *input, *output1, *output2;
  static float sign=1.0;
  static int button_count = 0;
  char outstr[100];

  initialize(FS_48K, MONO_IN, STEREO_OUT); 	// Set up the DAC/ADC interface
  
  /*
   * Allocate Required Memory
   */
  nsamp = getblocksize();
	
  input = (float *)malloc(sizeof(float)*nsamp);
  output1 = (float *)malloc(sizeof(float)*nsamp);
  output2 = (float *)malloc(sizeof(float)*nsamp);
  
  if (input==NULL || output1==NULL || output2==NULL) {
    flagerror(MEMORY_ALLOCATION_ERROR);
    while(1);
  }
  
  float a[2][3] = {{1.0, -0.6088, 0.9702},
           {1.0, -0.3500, -0.4250}};

  float b[2][3] = {{2.2044, 0.0, 2.2044},
           {2.9658, -3.4865, 2.9658}};

  float h[20] = {0.000168, 
                  -0.000883, 
                  -0.004735, 
                  -0.010728, 
                  -0.013639, 
                  -0.004205, 
                  0.025992, 
                  0.077462, 
                  0.138625, 
                  0.188861, 
                  0.208333, 
                  0.188861, 
                  0.138625, 
                  0.077462, 
                  0.025992, 
                  -0.004205, 
                  -0.013639, 
                  -0.010728, 
                  -0.004735, 
                  -0.000883};

  FIR_T * f1;
  f1 = init_fir(h,20,nsamp);

  BIQUAD_T *f2;
  f2 = init_biquad(2, 0.01718740, a, b, nsamp);

  /*
   * Infinite Loop to process the data stream, "nsamp" samples at a time
   */
  while(1){
    /*
     * Ask for a block of ADC samples to be put into the working buffer
     *   getblock() will wait until the input buffer is filled...  On return
     *   we work on the new data buffer.
     */
    getblock(input);	// Wait here until the input buffer is filled... Then process	
    // for(i=0;i<nsamp;i++) {
    //   input[i]++;
    // }

    /*
     * signal processing code to calculate the required output buffers
     */
    
    DIGITAL_IO_SET(); 	// Use a scope on PC4 to measure execution time
    // for(i=0;i<nsamp;i++) {
    //   output2[i] = input[i];
    // }  
    calc_fir(f1,input,output1);
    calc_biquad(f2,input,output2);
    
    DIGITAL_IO_RESET();	// (falling edge....  done processing data )
    
    // for(i=0;i<nsamp;i++) {
    //   input[i]--;
    // }
    /*
     * pass the processed working buffer back for DAC output
     */
    putblockstereo(output1, output2);
    
    if(UserButtonPressed==Button_Pressed) {		// Button Press?
      
      sign *= -1.0;					// Invert output2
      
      UserButtonPressed = Button_Ready;

      sprintf(outstr,"Button Pressed!  count = %x\n",button_count++); // %d, %f seem to be buggy
      UART_putstr(outstr);
      
    }
  }
}
