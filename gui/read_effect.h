/**
 * @file read_effect.h
 *
 * @author Jacob Allenwood
 * @date December 13, 2015
 *
 * @brief This file contains the prototypes for acquiring the gpio states set by the GUI, in turn 
 * determining which effect was selected and the parameters for the effect.
 * 
 */
 

#ifndef READ_EFFECT 
#define READ_EFFECT


 typedef struct effect {
 	int * pin_states;		// buffer containing the state of each PD pin (pin_states[0] -> PD0)
 	int * effect_params;	// buffer containing the values to set for the selected effect
 	int preset;				// contains the preset value from the gui (1 - 11)
 	int effect;				// 1 = delay, 2 = compressor, 3 = equalizer
 } FX_T;



FX_T * init_effects_read(
		void
	);


/**
 * @brief [initialize the gpio pins on the STM board, to receive effect and preset values from the gui]
 */
 void init_gpio(
 		void
 	);


/**
 * @brief [read the state of each gpio pin and place the values in the F->pin_states buffer]
 * @details [this is used to determine which effect and which preset has been selected from the gui]
 * 
 * @param F [pointer to effect structure]
 */
void read_gpio(
		FX_T * F 	// pointer to effect struct
	);


void read_effect(
		FX_T * F
	);


void free_fx(
		FX_T * F
	);


#endif