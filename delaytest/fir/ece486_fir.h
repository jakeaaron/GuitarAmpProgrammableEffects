/*!
 * @file
 * 
 * @brief ECE 486 Spring 2015 Lab 2 FIR header file
 * 
 * @author ECE486 Lab Group 2
 * @author Jacob Allenwood, Travis Russell, Jeremiah Simonsen
 * 
 * @date Feb 9, 2015
 * 
 * This file contains subroutine and data type declarations necessary for 
 * FIR filter calculations.
 *
 * The FIR_T data-type is a struct containing the information necessary for the
 * filter calculations.
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

#ifndef ECE486_FIR_H
#define ECE486_FIR_H

#include <stdint.h>

 /*!
  * @brief Structure for FIR implementation containing coefficients and filter 
  * state
  */

typedef struct fir_struct {

  float *fir_coefs;   /*!< Array of filter coefficients */
  int n_coefs;        /*!< Number of filter coefficients */
  int blocksize;      /*!< Number of samples */
  float *history;     /*!< Buffer to store last n_coefs samples */
  int histInd;        /*!< Index of current sample in history */

} FIR_T;

/*!
 * @brief Initializes an FIR_T structure.
 *
 * @returns A pointer to a structure of type FIR_T is returned containing the
 *          fields necessary for FIR filter implementation
 */

FIR_T * init_fir(
  float *fir_coefs,   /*!< Array of filter coefficients */
  int n_coefs,        /*!< Number of filter coefficients */
  int blocksize       /*!< Number of samples */
);

/*!
 * @brief Performs FIR filter calculation on block of samples
 *
 * @returns From input sample array @x, the output array @y is calculated using
 *          the FIR filter information in @s.
 */

void calc_fir(
  FIR_T *s,           /*!< Structure containing necessary FIR filter info */
  float *x,           /*!< Array of input samples */
  float *y            /*!< Array of outputs */
);

/*!
 * @brief De-allocates the memory required for an FIR_T struct.
 *
 */

void destroy_fir(
  FIR_T *s            /*!< Pointer to a FIR_T struct to be destroyed */
);

#endif