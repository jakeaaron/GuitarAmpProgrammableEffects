/**
 * @file send_effect.h
 *
 * @author Jacob Allenwood
 * @date December 14, 2015
 *
 * @brief This file contains the protoypes for setting the GPIO pins on the Pi, and then the STM board given the input
 * received from the GUI.
 * 
 * @setup [
 *  	GPIO MAPPING
 * 	
 * 		RPi	  --->	STM
 * 		
 * 		------------------
 * 		valid send bit
 * 		------------------
 * 		gpio 25		PB3
 * 		
 * 		------------------
 * 		effect selection
 * 		------------------
 * 		gpio 5		PD0
 * 		gpio 6		PD1
 * 		------------------
 * 		
 * 		preset selection
 * 		------------------
 * 		gpio 13		PD2
 * 		gpio 19		PD3
 * 		gpio 26		PD4
 * 		gpio 16		PD5
 * 		gpio 20		PD6
 * 		gpio 21		PD7
 * ]
 * 
 * @details [
 * 		init_gpio() - enables and initializes the gpio pins on the Pi to zero. these pins are used to talk to the STM board
 * 		
 * 		send_effect() - sets the gpio pins high specific to the effect and preset received from the gui. see the function for 
 * 			protocol.
 * 			
 * 		write_to_file() - actually writes the values from the send_effect() to the corresponding gpio files. this is called
 * 			by the send_effect() 
 * ]
 * 
 */


#ifndef SEND_EFFECT 
#define SEND_EFFECT


// DEFINES -------------------------------------------------

#define ENABLE_GPIO_FILE "/sys/class/gpio/export"

#define SET_DIRECTION_GPIO25 "/sys/class/gpio/gpio25/direction"
#define SET_VALUE_GPIO25 "/sys/class/gpio/gpio25/value"

#define SET_DIRECTION_GPIO5 "/sys/class/gpio/gpio5/direction"
#define SET_VALUE_GPIO5 "/sys/class/gpio/gpio5/value"

#define SET_DIRECTION_GPIO6 "/sys/class/gpio/gpio6/direction"
#define SET_VALUE_GPIO6 "/sys/class/gpio/gpio6/value"

#define SET_DIRECTION_GPIO13 "/sys/class/gpio/gpio13/direction"
#define SET_VALUE_GPIO13 "/sys/class/gpio/gpio13/value"

#define SET_DIRECTION_GPIO19 "/sys/class/gpio/gpio19/direction"
#define SET_VALUE_GPIO19 "/sys/class/gpio/gpio19/value"

#define SET_DIRECTION_GPIO26 "/sys/class/gpio/gpio26/direction"
#define SET_VALUE_GPIO26 "/sys/class/gpio/gpio26/value"

#define SET_DIRECTION_GPIO16 "/sys/class/gpio/gpio16/direction"
#define SET_VALUE_GPIO16 "/sys/class/gpio/gpio16/value"

#define SET_DIRECTION_GPIO20 "/sys/class/gpio/gpio20/direction"
#define SET_VALUE_GPIO20 "/sys/class/gpio/gpio20/value"

#define SET_DIRECTION_GPIO21 "/sys/class/gpio/gpio21/direction"
#define SET_VALUE_GPIO21 "/sys/class/gpio/gpio21/value"

// ---------------------------------------------------------




/**
 * @brief [initializes all the gpio pins for talking to the STM board to low]
 * @details [check out the gpio mapping between the rpi and the stm board]
 * @return [0 success, 1 error]
 */
int init_gpio(
	void
);


/**
 * @brief [this function writes to the correct gpio pins to tell the STM board which effect and what parameters were 
 * selected by the gui]
 * @details [note the mapping below]
 * 
 * @param argv [arguments passed from the gui through the command line]
 * @return [0 success, 1 error]
 */
int send_effect(
	char ** argv	// input from the GUI
);


/**
 * @brief [Open file, write to file and close file]
 * @details [Given the parameters, this function will open the specified file, write the specified value, and close the file]
 * 
 * @param file [string containing path to file to operate on]
 * @param value [what to write to the file]
 * @return [0 on success, 1 on error]
 */
int write_to_file(
	char * file, 	// file name to write to
	char * value 	// value to write to file
);



#endif