/**
 * @file send_effect.c
 *
 * @author Jacob Allenwood
 * @date December 14, 2015
 *
 * @brief This file contains the functions for setting the gpio pins of the RPi, to set the pins on the STM32F407-Discovery board.
 * This is done by connecting the gpio pins on the STM board, to the gpio pins on the Raspberry Pi. When the Pi sets pins high, the 
 * connected pins on the STM board go high as well, and then the dsp side of the code determines the pin states and the sent effect
 * and preset. 
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
 

// INCLUDES ------------------------------------------------

#include <stdio.h>
#include <fcntl.h>	/* open() */
#include <unistd.h>	/* close() */
#include <string.h>

// ---------------------------------------------------------

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
 * @brief [initializes all the gpio pins for talking to the STM board to low]
 * @details [check out the gpio mapping between the rpi and the stm board]
 * @return [0 success, 1 error]
 */
int init_gpio(void) {
/**
 * /// GPIO MAPPING ///
 * 
 * 	RPi	  --->	STM
 * 	------------------
 * 	valid send bit
 * 	------------------
 * 	gpio 25		PB3
 * 	
 * 	------------------
 * 	effect selection
 * 	------------------
 * 	gpio 5		PD0
 * 	gpio 6		PD1
 * 	------------------
 * 	
 * 	preset selection
 * 	------------------
 * 	gpio 13		PD2
 * 	gpio 19		PD3
 * 	gpio 26		PD4
 * 	gpio 16		PD5
 * 	gpio 20		PD6
 * 	gpio 21		PD7
 * 	
 */


// valid send bit --------------------------------------------------------------
// when this is set, the stm board knows that it is ready to read the other pins

	// gpio25 - PB3 ----------------------------------------------
	// enable gpio 25 and close the file
	if(write_to_file(ENABLE_GPIO_FILE, "25")) {
		perror("could not enable gpio25");
		return 1;
	}

	// make gpio25 an output and close the file
	if(write_to_file(SET_DIRECTION_GPIO25, "out")) {
		perror("could not make gpio25 an output");
		return 1;
	}

	// initialize to 0
	if(write_to_file(SET_VALUE_GPIO25, "0")) {
		perror("could not set gpio25 low");
		return 1;
	}


// effect selection bits -------------------------------------------------------

	// gpio5 - PD0 ----------------------------------------------
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

	// initialize to 0
	if(write_to_file(SET_VALUE_GPIO5, "0")) {
		perror("could not set gpio5 low");
		return 1;
	}

 	// gpio6 - PD1 ----------------------------------------------
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

	// initialize to 0
	if(write_to_file(SET_VALUE_GPIO6, "0")) {
		perror("could not set gpio6 low");
		return 1;
	}


// preset selection bits -------------------------------------------------------


	// gpio13 - PD2 ----------------------------------------------
	// enable gpio 13 and close the file
	if(write_to_file(ENABLE_GPIO_FILE, "13")) {
		perror("could not enable gpio13");
		return 1;
	}

	// make gpio13 an output and close the file
	if(write_to_file(SET_DIRECTION_GPIO13, "out")) {
		perror("could not make gpio13 an output");
		return 1;
	}

	// initialize to 0
	if(write_to_file(SET_VALUE_GPIO13, "0")) {
		perror("could not set gpio13 low");
		return 1;
	}


	// gpio19 - PD3 ----------------------------------------------
	// enable gpio 19 and close the file
	if(write_to_file(ENABLE_GPIO_FILE, "19")) {
		perror("could not enable gpio19");
		return 1;
	}

	// make gpio19 an output and close the file
	if(write_to_file(SET_DIRECTION_GPIO19, "out")) {
		perror("could not make gpio19 an output");
		return 1;
	}

	// initialize to 0
	if(write_to_file(SET_VALUE_GPIO19, "0")) {
		perror("could not set gpio19 low");
		return 1;
	}


	// gpio26 - PD4 ----------------------------------------------
	// enable gpio 26 and close the file
	if(write_to_file(ENABLE_GPIO_FILE, "26")) {
		perror("could not enable gpio26");
		return 1;
	}

	// make gpio26 an output and close the file
	if(write_to_file(SET_DIRECTION_GPIO26, "out")) {
		perror("could not make gpio26 an output");
		return 1;
	}

	// initialize to 0
	if(write_to_file(SET_VALUE_GPIO26, "0")) {
		perror("could not set gpio26 low");
		return 1;
	}


	// gpio16 - PD5 ----------------------------------------------
	// enable gpio 16 and close the file
	if(write_to_file(ENABLE_GPIO_FILE, "16")) {
		perror("could not enable gpio16");
		return 1;
	}

	// make gpio16 an output and close the file
	if(write_to_file(SET_DIRECTION_GPIO16, "out")) {
		perror("could not make gpio16 an output");
		return 1;
	}

	// initialize to 0
	if(write_to_file(SET_VALUE_GPIO16, "0")) {
		perror("could not set gpio16 low");
		return 1;
	}


	// gpio20 - PD6 ----------------------------------------------
	// enable gpio 20 and close the file
	if(write_to_file(ENABLE_GPIO_FILE, "20")) {
		perror("could not enable gpio20");
		return 1;
	}

	// make gpio20 an output and close the file
	if(write_to_file(SET_DIRECTION_GPIO20, "out")) {
		perror("could not make gpio20 an output");
		return 1;
	}

	// initialize to 0
	if(write_to_file(SET_VALUE_GPIO20, "0")) {
		perror("could not set gpio20 low");
		return 1;
	}

	// gpio21 - PD7 ----------------------------------------------
	// enable gpio 21 and close the file
	if(write_to_file(ENABLE_GPIO_FILE, "21")) {
		perror("could not enable gpio21");
		return 1;
	}

	// make gpio21 an output and close the file
	if(write_to_file(SET_DIRECTION_GPIO21, "out")) {
		perror("could not make gpio21 an output");
		return 1;
	}

	// initialize to 0
	if(write_to_file(SET_VALUE_GPIO21, "0")) {
		perror("could not set gpio21 low");
		return 1;
	}


	return 0;
}


/**
 * @brief [this function writes to the correct gpio pins to tell the STM board which effect and what parameters were 
 * selected by the gui]
 * @details [long description]
 * 
 * @param argv [arguments passed from the gui through the command line]
 * @return [0 success, 1 error]
 */
int send_effect(char ** argv) {

	/**
	 * 	-----------------------
	 * 	PB3
	 *	GPIO25			valid
	 *	-----------------------
	 *	0				no
	 *	1				yes
	 *	
	 *	
	 *	-----------------------
	 *	PD1		PD0			  
	 *	GPIO6	GPIO5	effect
	 *	-----------------------
	 *	0		1		delay
	 *	1		0		compressor
	 *	1		1		equalizer
	 *	
	 *	
	 *					GPIO21	GPIO20	GPIO16	GPIO26	GPIO19	GPIO13
	 *	Preset	Effect	PD7		PD6		PD5		PD4		PD3		PD2	
	 *	-----------------------------------------------------------
	 *	1		Delay	0		0		0		0		0		1
	 *	2		Delay	0		0		0		0		1		0
	 *	
	 *	3		Comp	0		0		0		0		0		1
	 *	4		Comp	0		0		0		0		1		0
	 *	
	 *	5		EQ		0		0		0		0		0		1
	 *	6		EQ		0		0		0		0		1		0
	 *	7		EQ		0		0		0		1		0		0
	 *	8		EQ		0		0		1		0		0		0
	 *	9		EQ		0		1		0		0		0		0
	 *	10		EQ		1		0		0		0		0		0
	 *	11		EQ		0		0		0		0		1		1
	 *	
	 *	
	 */

 	int effect = atoi(argv[1]);
 	int preset = atoi(argv[2]);

	// set gpio for effect selection ------------------------------------------
	switch(effect) {	
		case 1:		// delay
			// set effect selection bits ---------------------
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

			// set preset selection bits ---------------------
			switch(preset) {
				case 1:		// preset 1 - Large Room
					if(write_to_file(SET_VALUE_GPIO13, "1")) {
						perror("could not set gpio13 high");
						return 1;
					}

					break;

				case 2:		// preset 2 - Small Room
					if(write_to_file(SET_VALUE_GPIO19, "1")) {
						perror("could not set gpio19 high");
						return 1;
					}

					break;


				default:	// error
					perror("invalid preset selection for delay");
					return 1; 
			}


			break;


		case 2:		// compressor	
			// set effect selection bits ---------------------------------- 
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

			// set preset selection bits ----------------------------------
			switch(preset) {
				case 3:		// preset 3 - Coffee Shop
					if(write_to_file(SET_VALUE_GPIO13, "1")) {
						perror("could not set gpio13 high");
						return 1;
					}

					break;

				case 4:		// preset 4 - Celestial Immolation
					if(write_to_file(SET_VALUE_GPIO19, "1")) {
						perror("could not set gpio19 high");
						return 1;
					}

					break;


				default:	// error
					perror("invalid preset selection for compressor");
					return 1; 

			}

			break;


		case 3:		// equalizer
			// set effect selection bit --------------------------
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

			// set preset selection bits -------------------------
			switch(preset) {
				case 5:			// bass boost
					if(write_to_file(SET_VALUE_GPIO13, "1")) {
						perror("could not set gpio13 high");
						return 1;
					}

					break;

				case 6:			// mid boost
					if(write_to_file(SET_VALUE_GPIO19, "1")) {
						perror("could not set gpio19 high");
						return 1;
					}

					break;

				case 7:			// treble boost
					if(write_to_file(SET_VALUE_GPIO26, "1")) {
						perror("could not set gpio26 high");
						return 1;
					}

					break;

				case 8:			// bass attenuation
					if(write_to_file(SET_VALUE_GPIO16, "1")) {
						perror("could not set gpio16 high");
						return 1;
					}

					break;

				case 9:			// mid attenuation
					if(write_to_file(SET_VALUE_GPIO20, "1")) {
						perror("could not set gpio20 high");
						return 1;
					}

					break;

				case 10:		// treble attenuation
					if(write_to_file(SET_VALUE_GPIO21, "1")) {
						perror("could not set gpio21 high");
						return 1;
					}

					break;

				case 11:		// flat response
					if(write_to_file(SET_VALUE_GPIO13, "1")) {
						perror("could not set gpio13 high");
						return 1;
					}

					// gpio6 - 1
					if(write_to_file(SET_VALUE_GPIO19, "1")) {
						perror("could not set gpio19 high");
						return 1;		
					}

					break;


				default:

					perror("invalid preset selection for equalizer");
					return 1;
			}


			break;


		default:
			perror("invalid effect selection");
			return 1;

			break;

	}


	// now that all bits are set, set valid send bit for STM board ------------
	if(write_to_file(SET_VALUE_GPIO25, "1")) {
		perror("could not set gpio25 high");
		return 1;
	}

	return 0;

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
