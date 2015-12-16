/**
 * @file read_effect.c
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
 


#include "stm32f4xx_hal.h"
#include "stm32f4_discovery.h"

#include "arm_math.h"
#include "ece486.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "read_effect.h"

// PRIVATE FUNCTION PROTOTYPES -----------------


// ---------------------------------------------





 FX_T * init_effects_read(void) {

	int i, j;

	FX_T * F = (FX_T *)malloc(sizeof(FX_T));

	F->effect = 0;
	F->pin_states = (int *)malloc(sizeof(int) * 8);		// state of 8 PD pins
	F->effect_params = (int *)malloc(sizeof(int) * 3); 	// holds the effect parameters
	if(F->pin_states == NULL || F->effect_params == NULL) {
		BSP_LED_Toggle(ERROR_LED);
		while(1);
	}
	for(i = 0; i < 8; i++) F->pin_states[i] = 0;	
	for(j = 0; j < 3; j++) F->effect_params[j] = 0;


	// initialize LEDs for waiting for valid send
	BSP_LED_Init(LED3);
	BSP_LED_Init(LED6);


	// initialize each of the PD pins (PD0 - PD7) and the valid send pin (PB3)
	init_gpio();

	// blink while we wait until send is valid 
	while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3) != GPIO_PIN_SET) {
    	BSP_LED_Toggle(LED6);
    	HAL_Delay(450);	
    	BSP_LED_Toggle(LED3);
    	HAL_Delay(450);
	}
	// now that the send is valid, let's read the pin states
	read_gpio(F);

	return F;
}


void init_gpio(void) {
	
	GPIO_InitTypeDef GPIO_InitStruct;
 
	__GPIOD_CLK_ENABLE();	// enable clock for all the PD gpio pins
	__GPIOB_CLK_ENABLE();	// enable clock for valid send bit gpio PB3
  

  	// set up PD gpios for effect selection and params/presets ---------
 	GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;	 
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;		// input floating mode
	GPIO_InitStruct.Pull = GPIO_NOPULL;			// no pull up or pull down activation
	GPIO_InitStruct.Speed = GPIO_SPEED_FAST;	

	// init gpio
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);


	// set up valid send bit gpio (PB3) ----------------------------------
	GPIO_InitStruct.Pin = GPIO_PIN_3;

	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}


void read_gpio(FX_T * F) {
/**
 * /// GPIO MAPPING ///
 * 
 * 	RPi	  --->	STM
 * 	------------------
 * 	valid send bit
 * 	------------------
 * 	gpio 12		PB3
 * 	
 * 	------------------
 * 	effect selection
 * 	------------------
 * 	gpio 5		PD0
 * 	gpio 6		PD1
 */

	// PD0
	if(HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_0) == GPIO_PIN_SET) {
		F->pin_states[0] = 1;
		// UART_putstr("1\n");
	} else {
		F->pin_states[0] = 0;
		// UART_putstr("0\n");
	}

	// PD1
	if(HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_1) == GPIO_PIN_SET) {
		F->pin_states[1] = 1;
		// UART_putstr("1\n");
	} else {
		F->pin_states[1] = 0;
		// UART_putstr("0\n");
	}

	// // PD2
	// if(HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_2) == GPIO_PIN_SET) {
	// 	F->pin_states[2] = 1;
	// } else {
	// 	F->pin_states[2] = 0;
	// }

	// // PD3
	// if(HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_3) == GPIO_PIN_SET) {
	// 	F->pin_states[3] = 1;
	// } else {
	// 	F->pin_states[3] = 0;
	// }

	// // PD4
	// if(HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_4) == GPIO_PIN_SET) {
	// 	F->pin_states[4] = 1;
	// } else {
	// 	F->pin_states[4] = 0;
	// }

	// // PD5
	// if(HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_5) == GPIO_PIN_SET) {
	// 	F->pin_states[5] = 1;
	// } else {
	// 	F->pin_states[5] = 0;
	// }

	// // PD6
	// if(HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_6) == GPIO_PIN_SET) {
	// 	F->pin_states[6] = 1;
	// } else {
	// 	F->pin_states[6] = 0;
	// }

	// // PD7
	// if(HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_7) == GPIO_PIN_SET) {
	// 	F->pin_states[7] = 1;
	// } else {
	// 	F->pin_states[7] = 0;
	// }

}


void read_effect(FX_T * F) {

	/** figure out which effect was selected 
	 *	
	 *	PD1		PD0		effect
	 *	-----------------------
	 *	0		1		delay
	 *	1		0		compressor
	 *	1		1		equalizer
	 */

	// effect selection
	if(F->pin_states[0] == 1 && F->pin_states[1] == 0) {		// 01 delay
		F->effect = 1;
	} else if(F->pin_states[0] == 0 && F->pin_states[1] == 1) {	// 10 compressor
		F->effect = 2;
	} else if(F->pin_states[0] == 1 && F->pin_states[1] == 1) {	// 11 equalizer
		F->effect = 3;
	} else {													// invalid pin states for effect selection
		UART_putstr("im in da error :(\n");
		BSP_LED_Toggle(ERROR_LED);
		while(1);
	}

}
