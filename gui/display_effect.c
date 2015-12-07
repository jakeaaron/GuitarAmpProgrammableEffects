/**
 * @file [display_effect.c]
 * 
 * @author [Jacob Allenwood]
 * @date [December 3, 2015]
 * 
 * @setup [Raspberry Pi 2 -  LED display - 1Wire
 * 				Connect 3.3V on the Pi to + on the LED Display 
 * 				Connect GND on the Pi to - on the LED Display
 * 				Connect SDA on the Pi to D on the LED Display
 * 				Connect SCL on the Pi to C on the LED Display.
 * 			]
 * @input [the input of this program is the output from a python GUI in which a user selects the effects 
 * and appropriate parameters for the GAPE unit. this program is called in the python script using subprocess.call("executable name", argv)]
 * 
 * @brief [takes input from gui (argc, argv) and writes the data to the 7segment display]
 * @details []
 * 
 * @note [some things to note:
 * 			the first element in the 'effect' array is the effect itself. (1 = delay, 2 = compressor, 3 = equalizer) This value
 * 			is treated differently than the 'parameters' for that effect. The effect val is put in the struct, and isn't read
 * 			when determining what characters to display. The char_to_7seg only reads through the parameters in the input array,
 * 			and then negative parameters are treated differently than positive ones. Instead of trying to deal with reading a negative
 * 			sign character when figuring out what hex values to write to the display, there is buffer containing the sign of the input 
 * 			parameter, and then the absolute values are written to the input buffer, which is 'snprintf-ed' to convert to characters for
 * 			the char_to_7seg function. 
 * ]
 * 
 * 
 */


// INCLUDE ----------------------------------------------------

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#include "display_effect.h"

// ------------------------------------------------------------

// DEFINE -----------------------------------------------------

// note: other hex defines for display in header file

#define I2C_LCD_DEVICE "/dev/i2c-1"

// ------------------------------------------------------------



int main(int argc, char ** argv) {

	int fd;

	// initialize i2c
	fd = init_i2c(I2C_LCD_DEVICE);
	if(fd < 0) { perror("could not initialize i2c"); return 1; }

	// initialize lcd display
	if(init_display(fd)) { perror("could not initialize display"); return 1; }
	
	// check if gui is closing so we can clear the display before we kill the program
	if(atoi(argv[1]) == -1) {
		if(clear_display(fd) == 1) { perror("could not clear display"); return 1; };
		return 0;
	}

	DISP_T * D_T = init_params(fd, argv);
	if(D_T == NULL) { perror("could not initialize struct"); return 1; }

	while(1) {
		// convert effect val to lcd vals for 7seg display
		// then write val to the lcd
		if(char_to_7seg(argc, D_T)) { perror("getting 7seg chars failed"); return 1; };		
	}

	return 0;

}


/**
 * @brief [initialize i2c]
 * @details [open i2c device file and set i/o slave address]
 * 
 * @param device_file [path to the device file]
 * @return [file descriptor from device file for use with other functions]
 */
int init_i2c(char * device_file) {

	int fd;

	// /* Open i2c device */
	fd = open(device_file, O_WRONLY);
	if(fd < 0) { perror("could not open i2c device file"); return -1; }

	/* Set slave address */
	if(ioctl(fd, I2C_SLAVE, 0x70) < 0) { perror("ioctl could not set slave address"); return -1; }

	// file is left open for other functions to write to
	return fd;
}


/**
 * @brief [initialize lcd display]
 * @details [set up oscillator, display and brightness]
 * 
 * @param fd [file descriptor to write to]
 * @return [0 on success, 1 on error]
 */
int init_display(int fd) {

	unsigned char buffer[1];

	/* Turn on oscillator */
    buffer[0] = (0x2<<4)|(0x1);
    if(write(fd, buffer, 1) < 1) { perror("could not write oscillator"); return 1; }

	/* Turn on Display, No Blink */
	buffer[0] = (0x8<<4)|(0x1);
	if(write(fd, buffer, 1) < 1) { perror("could not write display"); return 1; }

	/* Set Brightness */
	buffer[0] = (0x12<<4)|(0x1); // 10/16 duty cycle
	if(write(fd, buffer, 1) < 1) { perror("could not set brightness"); return 1; }

	return 0;
}


/**
 * @brief [brief description]
 * @details [long description]
 * 
 * @param argv [description]
 * @return [description]
 */
DISP_T * init_params(int fd, char ** argv) {

	float val;	// contains the input value and is used to determine sign

	DISP_T * D_T = malloc(sizeof(DISP_T));

	// input holds the values from the python gui to display the effect and its parameters
	D_T->input = calloc(4, sizeof(float));
	// output holds the hex values to display the appropriate character(s) on the 7seg display
	D_T->output = calloc(4, sizeof(unsigned char));
	// buffer containing the sign of each input value so we don't have to deal with reading negative vals with the character stuff
	D_T->sign_buffer = calloc(3, sizeof(int));
	// file descriptor for writing to i2c device
	D_T->fd = fd;
	// selected effect value: 1 = delay, 2 = compressor, 3 = eq
	D_T->effect = atoi(argv[1]);


	D_T->input[0] = D_T->effect;
	switch(D_T->effect) {
		case 1:
			// if the value is negative, note in the sign buffer that it is negative, and then put the absolute val of it
			// in the input buffer, so that the 7seg code isn't trying to read a negative sign because we're only
			// expecting a certain number of characters, and only numbers and decimal point

			// param 1
			// amount of delay
			if((val = atof(argv[2]) / (2.0 * 255.0)) >= 0) {
				D_T->input[1] = val;
				D_T->sign_buffer[0] = 1;
			} else {
				D_T->input[1] = abs(val);
				D_T->sign_buffer[0] = -1;
			}

			// param 2
			// amount of gain
			if((val = atof(argv[3]) / 255.0) >= 0) {
				D_T->input[2] = val;
				D_T->sign_buffer[1] = 1;
			} else {
				D_T->input[2] = abs(val);
				D_T->sign_buffer[1] = -1;
			}


			break;

		case 2:
			// if the value is negative, note in the sign buffer that it is negative, and then put the absolute val of it
			// in the input buffer, so that the 7seg code isn't trying to read a negative sign because we're only
			// expecting a certain number of characters, and only numbers and decimal point

			// param 1
			// threshold level
			if((val = atof(argv[2]) - 200.0) >= 0) {
				D_T->input[1] = val;
				D_T->sign_buffer[0] = 1;
			} else {
				D_T->input[1] = abs(val);
				D_T->sign_buffer[0] = -1;
			}

			// param 2
			// ratio level
			if((val = atof(argv[3])) >= 0) {
				D_T->input[2] = val;
				D_T->sign_buffer[1] = 1;
			} else {
				D_T->input[2] = abs(val);
				D_T->sign_buffer[1] = -1;
			}


			break;

		case 3:
			// if the value is negative, note in the sign buffer that it is negative, and then put the absolute val of it
			// in the input buffer, so that the 7seg code isn't trying to read a negative sign because we're only
			// expecting a certain number of characters, and only numbers and decimal point

			// param 1
			// low band
			if((val = atof(argv[2]) - 10.0) >= 0) {
				D_T->input[1] = val;
				D_T->sign_buffer[0] = 1;
			} else {
				D_T->input[1] = abs(val);
				D_T->sign_buffer[0] = -1;
			}


			// param 2
			// mid band
			if((val = atof(argv[3]) - 10.0) >= 0) {
				D_T->input[2] = val;
				D_T->sign_buffer[1] = 1;
			} else {
				D_T->input[2] = abs(val);
				D_T->sign_buffer[1] = -1;
			}


			// param 3
			// high band
			if((val = atof(argv[4]) - 10.0) >= 0) {
				D_T->input[3] = val;
				D_T->sign_buffer[2] = 1;
			} else {
				D_T->input[3] = abs(val);
				D_T->sign_buffer[2] = -1;
			}



			break;


		default:
			perror("incorrect effect value");
			return NULL;
	}



	return D_T;

}


/**
 * @brief [brief description]
 * @details [long description]
 * 
 * @param argc [description]
 * @param D [description]
 * 
 * @return [description]
 */
int char_to_7seg(int argc, DISP_T * D_T) {

	int i, k;
	char effect_buf[5];

	// which parameter is being displayed
	// used to determine whether to display effect name before displaying param
	D_T->display_index = 0;

	// display each value from the gui
	// [effect, 1st param, 2nd param, 3rd param]
	// (parameter)
	for(k = 1; k < argc - 1; k++) {
		
		int j = 0;

		int z;
		// clear output buffer
		for(z = 0; z < 4; z++) {
			D_T->output[z] = 0;
			printf("input is: %f\n", D_T->input[z]);
		}

		// this is 4 bytes of one passed value with null at end
		snprintf(effect_buf, 5, "%f", D_T->input[k]);

		// send each character of the param to get written
		// (character of the parameter)
		for(i = 0; i < 5; i++) {
			switch(effect_buf[i]) {
				case '0':
					D_T->output[j] = ZERO;
					printf("I'm writing 0 to the output!\n");

					break;

				case '1':
					D_T->output[j] = ONE;
					printf("I'm writing 1 to the output!\n");

					break;

				case '2':
					D_T->output[j] = TWO;
					printf("I'm writing 2 to the output!\n");

					break;

				case '3':

					D_T->output[j] = THREE;
					printf("I'm writing 3 to the output!\n");

					break;

				case '4':

					D_T->output[j] = FOUR;
					printf("I'm writing 4 to the output!\n");

					break;

				case '5':

					D_T->output[j] = FIVE;
					printf("I'm writing 5 to the output!\n");

					break;

				case '6':

					D_T->output[j] = SIX;
					printf("I'm writing 6 to the output!\n");

					break;

				case '7':

					D_T->output[j] = SEVEN;
					printf("I'm writing 7 to the output!\n");

					break;

				case '8':

					D_T->output[j] = EIGHT;
					printf("I'm writing 8 to the output!\n");

					break;

				case '9':

					D_T->output[j] = NINE;
					printf("I'm writing 9 to the output!\n");

					break;

				case '.':

					// if there's a decimal point, then take the previous number and toggle the decimal segment on
					D_T->output[j - 1] = D_T->output[j - 1] | DECIMAL;
					j = j - 1; // decimal is not its own character, so make sure next character is in the right output index

					break;


				default:

					// probably a blank character, just don't try and write it to the output!
					// this means we have no more characters to display
					// not worried about getting other invalid characters because of the error checking in the gui
					i++;

					break;

			}

			j++; // increment output buffer index
		}

		// for delay and compressor we don't have a third parameter, so lets skip over trying to write the output for it
		if(D_T->effect != 3 && k == 2) {
			k++;
		}
					printf("effect_buf is: %c %c %c \n", effect_buf[0], effect_buf[1], effect_buf[2]);
					printf("output buf is: %u %u %u \n", D_T->output[0], D_T->output[1], D_T->output[2]);

		// write to ldc
		if(lcd_write(D_T) == 1) { perror("could not write to lcd"); return 1; }	
	}

	return 0;

}


/**
 * @brief [brief description]
 * @details [long description]
 * 
 * @param fd [description]
 * @param D [description]
 * 
 * @return [description]
 */
int lcd_write(DISP_T * D_T) {

	unsigned char buffer[17];
	int i;

	// clear buffer
	for(i = 0; i <17; i++) {
		buffer[i] = 0x00;
	}

	// these bytes won't change
	buffer[0] = 0x00;  	// offset pointer
	buffer[2] = 0x00;  	// next 8 bits of column 1, not connected
	buffer[4] = 0x00;  	// next 8 bits of column 2, not connected
	buffer[5] = 0x00;  	// middle dots, column 3
	buffer[6] = 0x00;	// next 8 bits of column 4, not connected 
	buffer[10] = 0x00;	// next 8 bits of column 5, not connected

	printf("%d\n", D_T->display_index);

	// display chosen effect first if we haven't already
	if(D_T->display_index == 0) {
		switch(D_T->effect) {
			case 1:	// display delay
				buffer[1] = D;

				buffer[3] = L;

				buffer[7] = Y;

				// last digit blank
				buffer[9] = 0x00;

				break;

			case 2:	// display compressor
				buffer[1] = C;

				buffer[3] = P;

				buffer[7] = R;

				buffer[9] = S;

				break;

			case 3:	// display equalizer
				buffer[1] = E;

				buffer[3] = Q;

				// blank
				buffer[7] = 0x00;

				// last digit blank
				buffer[9] = 0x00;

				break;

			default:	// display error
				buffer[1] = E;

				buffer[3] = R;

				buffer[7] = R;

				// last digit blank
				buffer[9] = 0x00;

				perror("invalid effect val");
				return 1;
		}	
			
		// write the buffer to lcd
		if(write(D_T->fd, buffer, 17) < 0) { perror("could not write message to lcd"); return 1; }
		// increment which parameter we are displaying since now we will display the first param because we just displayed the effect
		D_T->display_index++;
		// display for 3 seconds
		usleep(2500000);

		// clear buffer
		for(i = 0; i <17; i++) {
			buffer[i] = 0x00;
		}

	} 

	printf("sign is: %d\n", D_T->sign_buffer[D_T->display_index - 1]);

	// now display parameter
	// determine whether we need a negative sign or not, and then display it appropriately 
	// sign buffer doesn't include first display val (the actual selected effect) so display_index -1 when trying to correlate the correct value
	if(D_T->sign_buffer[D_T->display_index - 1] == -1) {	
		// first digit should be negative sign
		buffer[1] = NEG;

		// second digit is first digit of param
		buffer[3] = D_T->output[0];

		// third digit is second digit of param
		buffer[7] = D_T->output[1];

		// third digit of param
		buffer[9] = D_T->output[2];	// fourth letter

	} else if(D_T->sign_buffer[D_T->display_index - 1] == 1) {
		// first digit is first digit of param
		buffer[1] = D_T->output[0];

		// second digit is second digit of param
		buffer[3] = D_T->output[1];

		// third digit
		buffer[7] = D_T->output[2];

		// last digit is blank
		buffer[9] = 0x00;	// fourth letter
	} else {
		// delay and compressor doesn't have 3 params, so because display index goes all the way to the third parameter in case of eq, we only
		// care if the effect should have a third parameter and doesn't.
		if(D_T->effect == 3) {
			// when we get input vals we should be having a sign value for each parameter determined in the char7seg function
			perror("something went wrong with determining the sign of the input value");

			return 1;	
		}

	}

	// write the buffer to lcd
	if(write(D_T->fd, buffer, 17) < 0) { perror("could not write message to lcd"); return 1; }
	// increment which parameter we are displaying
	D_T->display_index++;
	// display for 3 seconds
	usleep(2500000);


	// cause a little blink between showing parameters
	// clear buffer
	for(i = 0; i <17; i++) {
		buffer[i] = 0x00;
	}
	if(write(D_T->fd, buffer, 17) < 0) { perror("could not write message to lcd"); return 1; }
	usleep(10000);

	return 0;
}


int clear_display(int fd) {

	unsigned char buffer[17];
	int i;

	// clear buffer
	for(i = 0; i <17; i++) {
		buffer[i] = 0x00;
	}

	// clear the display
	if(write(fd, buffer, 17) < 0) { perror("could not write message to lcd"); return 1; }

	return 0;

}
