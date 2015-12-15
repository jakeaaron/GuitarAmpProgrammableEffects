/**
 * @file send_effect.c
 *
 * @author Jacob Allenwood
 * @date December 14, 2015
 *
 * @brief This file contains the functions for setting the gpio pins of the RPi, to set the pins on the STM32F407-Discovery board.
 * This is done by connecting the transmitting Xbee to the RPi, and the receiving Xbee to the STM board. When the pins on the transmitting
 * Xbee are set, the receiving Xbee matches the same pin states. 
 * 
 * @details [
 * ]
 * 
 */
 

// INCLUDES ------------------------------------------------

#include <stdio.h>
#include <fcntl.h>	/* open() */
#include <unistd.h>	/* close() */
#include <string.h>

// ---------------------------------------------------------

// DEFINES -------------------------------------------------

#define ENABLE_GPIO_FILE "/sys/class/gpio/export"

#define SET_DIRECTION_GPIO5 "/sys/class/gpio/gpio5/direction"
#define SET_VALUE_GPIO5 "/sys/class/gpio/gpio5/value"

#define SET_DIRECTION_GPIO6 "/sys/class/gpio/gpio6/direction"
#define SET_VALUE_GPIO6 "/sys/class/gpio/gpio6/value"

// ---------------------------------------------------------

// FUNCTION PROTOTYPES -------------------------------------

int init_gpio(void);
int send_effect(char ** argv);
int write_to_file(char * file, char * value);	// open write and close file

// ---------------------------------------------------------




int main(int argc, char ** argv) {


	// enable and set gpios as output ---------------------------------
	if(init_gpio() == 1) {
		perror("could not initialize gpios");
		return 1;
	}

	// set gpios to tell STM board which effect we have selected
	if(send_effect(argv) == 1) {
		perror("could not send effect");
		return 1;
	}

	return 0;

}


/**
 * @brief [brief description]
 * @details [long description]
 * @return [description]
 */
int init_gpio(void) {
/**
 * 	RPi			Xbee
 * 	------------------
 * 	effect selection
 * 	------------------
 * 	gpio 5		DI/O 0
 * 	gpio 6		DI/O 1
 */

// effect selection bits -------------------------------------------------------


	// gpio5 - DI/O1 ----------------------------------------------
	// enable gpio 5 and close the file
	if(write_to_file(ENABLE_GPIO_FILE, "5")) {
		perror("could not enable gpio5");
		return 1;
	}

	// make gpio5 an output and close the file
	if(write_to_file(SET_DIRECTION_GPIO5, "out")) {
		perror("could not make gpio5 an output");
		return 1;
	}

 	// gpio6 - DI/O0 ----------------------------------------------
	// enable gpio6 and close the file
	if(write_to_file(ENABLE_GPIO_FILE, "6")) {
		perror("could not enable gpio6");
		return 1;
	}

	// make gpio6 an output and close the file
	if(write_to_file(SET_DIRECTION_GPIO6, "out")) {
		perror("could not make gpio6 an output");
		return 1;
	}

	return 0;
}


/**
 * @brief [brief description]
 * @details [long description]
 * 
 * @param argv [description]
 * @return [description]
 */
int send_effect(char ** argv) {

	/**
	 *	GPIO6	GPIO5	effect
	 *	-----------------------
	 *	0		1		delay
	 *	1		0		compressor
	 *	1		1		equalizer
	 */

 	int effect = atoi(argv[1]);

	// set gpio for effect selection ---------------------
	if(effect == 1) {			// delay
		// gpio5 - 1
		if(write_to_file(SET_VALUE_GPIO5, "1")) {
			perror("could not set gpio5 high");
			return 1;
		}

		// gpio6 - 0
		if(write_to_file(SET_VALUE_GPIO6, "0")) {
			perror("could not set gpio6 low");
			return 1;		
		}
	} else if(effect == 2) {	// compressor									
		// gpio5 - 0
		if(write_to_file(SET_VALUE_GPIO5, "0")) {
			perror("could not set gpio5 low");
			return 1;
		}

		// gpio6 - 1
		if(write_to_file(SET_VALUE_GPIO6, "1")) {
			perror("could not set gpio6 high");
			return 1;		
		}
	} else if(effect == 3) {	// equalizer
		// gpio5 - 1
		if(write_to_file(SET_VALUE_GPIO5, "1")) {
			perror("could not set gpio5 high");
			return 1;
		}

		// gpio6 - 1
		if(write_to_file(SET_VALUE_GPIO6, "1")) {
			perror("could not set gpio6 high");
			return 1;		
		}
	} else {
		perror("invalid effect selection");
		return 1;
	}

}


/**
 * @brief [Open file, write to file and close file]
 * @details [Given the parameters, this function will open the specified file, write the specified value, and close the file]
 * 
 * @param file [string containing path to file to operate on]
 * @param value [what to write to the file]
 * @return [0 on success, 1 on error]
 */
int write_to_file(char * file, char * value) {

	FILE * fp;
	// open file and error check
	fp = fopen(file, "w");	
	if(!fp) {
		perror("file open failed");
		return 1;
	}
	// write value to file and error check
	if(fprintf(fp, "%s\n", value) == 0) {
		perror("error writing to file");
		return 1;
	}
	// clear write buffer
	fflush(fp);
	// close file
	if(fclose(fp)) {
		perror("close error");
		return 1;
	}

	return 0;
}
