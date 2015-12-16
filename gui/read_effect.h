/**
 * @file read_effect.h
 *
 * @author Jacob Allenwood
 * @date December 13, 2015
 *
 * @brief This file contains the prototypes for acquiring the gpio states set by the GUI, in turn 
 * determining which effect was selected and the parameters for the effect.
 * @details [
 * ]
 * 
 */
 

#ifndef READ_EFFECT 
#define READ_EFFECT


 typedef struct effect {
 	int * pin_states;
 	int * effect_params;
 	int preset;
 	int effect;
 } FX_T;



FX_T * init_effects_read(
		void
	);


 void init_gpio(
 		void
 	);


void read_gpio(
		FX_T * F 	// pointer to effect struct
	);


void read_effect(
		FX_T * F
	);


#endif