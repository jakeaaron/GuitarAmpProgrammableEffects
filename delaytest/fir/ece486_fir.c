/*!
 * @file
 * 
 * @brief ECE 486 Spring 2015 Lab 2 FIR Subroutine Definitions
 * 
 * @author ECE486 Lab Group 2
 * @author Jacob Allenwood, Travis Russell, Jeremiah Simonsen
 * 
 * @date Feb 9, 2015
 * 
 * This file contains the subroutine definitions necessary for performing the
 * FIR filter calculations on a block of samples.
 *
 * The init_fir() function initializes an FIR_T variable, allocating memory for
 * all fields. The function takes an array of impulse response coefficients in 
 * the form of 'float *fir_coefs'. The argument 'n_coefs' denotes the number of
 * non-zero terms in the impulse response. Samples will be processed in chunks 
 * 'blocksize' samples. 'history' is a circular buffer for storing the previous
 * n_coefs samples for performing the convolution. 'histInd' keeps track of the
 * current index in the history  buffer. The function returns a pointer to an 
 * FIR_T structure.
 *
 * The calc_fir() function performs the FIR filtering on 'blocksize' samples of
 * input 'x', storing the corresponding outputs in 'y'. The information needed
 * to perform the calculation is passed to the function with the FIR_T struct
 * 's'.
 *
 * The destroy_fir() function de-allocates all memory pointed to by FIR_T *'s' 
 * allocated by init_fir().
 * 
 */

#include "ece486_fir.h"
#include <stdio.h>
#include <stdlib.h>

FIR_T * init_fir(float * fir_coefs, int n_coefs, int blocksize){
  // Allocate memory for an FIR_T structure
  FIR_T * s;
  s = (FIR_T *)malloc(sizeof(FIR_T));
  if(s == NULL) return NULL;

  // Initialize variables
  s->fir_coefs = fir_coefs;
  s->n_coefs = n_coefs;
  s->blocksize = blocksize;
  s->history = (float *)malloc((n_coefs)*sizeof(float));
  if(s->history == NULL) return NULL;

  int i;
  for (i = 0; i < n_coefs; i++) {
    s->history[i] = 0.0;
  }

  s->histInd = 0;

  return s;
}

// This code DOES function when x and y point to the same location.
// In order to have this work when x and y point to the same location, the 
// FIR_T struct will have to store the past 'n_coefs' values of x(n), in a
// circular buffer. This is really only more efficient when n_coefs < blocksize
void calc_fir(FIR_T * s, float * x, float * y){
  int k,n;  // Indices corresponding to the convolution sum

  // For all samples in the block
  for (n = 0;n < s->blocksize; n++) {
    s->history[s->histInd] = x[n];     // Copy x[n] to history buffer
    y[n] = 0.0;                        // zero the output
    // For all terms in the convolution sum
    for (k = 0; k < s->n_coefs; k++) {
        int index = s->histInd - k;    // calculate n-k
        // Handle looping off the end of the array
        index = index>=0 ? index : index+s->n_coefs;
        y[n] += (s->fir_coefs[k] * s->history[index]);  // calculate output
    }
    s->histInd++;                      // move index of history buffer
    // Handle looping off the end of the array
    if(s->histInd == (s->n_coefs)) {
      s->histInd = 0;
    }
  }
}

// De-allocate memory for all dynamically allocated variables
void destroy_fir(FIR_T *s){
  // free any dynamically allocated elements of s first
  free(s->history);
  // Free structure
  free(s);
  // NULL pointer for safety
  s = NULL;
}