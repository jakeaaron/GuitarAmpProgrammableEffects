/**
 * @file [display_temp.c]
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

#include "lcd_defines.h"

// ------------------------------------------------------------

// DEFINE -----------------------------------------------------
// note: other hex defines for display
#define I2C_LCD_DEVICE "/dev/i2c-1"
#define TEMP_SENSOR_DEVICE "/sys/bus/w1/devices/28-000006b59a2f/w1_slave"

// ------------------------------------------------------------



int main(int argc, char ** argv) {

	int fd;
	unsigned char output[4]; // buffer used for holding temp_string

	// initialize i2c
	fd = init_i2c(I2C_LCD_DEVICE);
	if(fd < 0) { perror("could not initialize i2c"); return 1; }

	// initialize lcd display
	if(init_display(fd)) { perror("could not initialize display"); return 1; }
	
	DISP_T * D = init_params(argv);
	if(D == NULL) { perror("could not initialize struct"); return 1; }

	while(1) {

		// convert effect val to lcd vals for 7seg display
		// then write val to the lcd
		char_to_7seg(argc, D);

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
DISP_T * init_params(char ** argv) {
	DISP_T * D = malloc(sizeof(DISP_T));

	D->input = calloc(4, sizeof(char));			
	D->output = calloc(4, sizeof(unsigned char));

	D->effect = atoi(argv[1])

	D->input[0] = D->effect;
	switch(D->effect) {
		case 1:
			// amount of delay
			D->input[1] = atoi(argv[2]) / (2.0 * 255.0);
			// amount of gain
			D->input[2] = atoi(argv[3]) / 255.0;

			break;

		case 2:
			// threshold level
			D->input[1] = atoi(argv[2]) - 200;
			// ratio level
			D->input[2] = atoi(argv[3]);

			break;

		case 3:
			// low band
			D->input[1] = atoi(argv[2]) - 10;
			// mid band
			D->input[2] = atoi(argv[3]) - 10;
			// high band
			D->input[3] = atoi(argv[4]) - 10;

			break;

		default:
			perror("incorrect effect value");
			return NULL;
	}

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
int char_to_7seg(argc, DISP_T * D) {

	int i, k;
	int j = 0;
	char effect_buf[4];

	// which parameter is being displayed
	// used to determine whether to display effect name before displaying param
	D->display_index = 0;

	// display each value from the gui
	// [effect, 1st param, 2nd param, 3rd param]
	for(k = 1; k < argc - 1, k++) {

		// this is 4 bytes with null at end
		snprintf(effect_buf, 4, "%c", D->input[k]);

		// send each character of the param to get written
		for(i = 1; i < 4; i++) {
			switch(effect_buf[i]) {
				case '0':

					D->output[j] = ZERO;

					break;

				case '1':

					D->output[j] = ONE;

					break;

				case '2':

					D->output[j] = TWO;

					break;

				case '3':

					D->output[j] = THREE;

					break;

				case '4':

					D->output[j] = FOUR;

					break;

				case '5':

					D->output[j] = FIVE;

					break;

				case '6':

					D->output[j] = SIX;

					break;

				case '7':

					D->output[j] = SEVEN;

					break;

				case '8':

					D->output[j] = EIGHT;

					break;

				case '9':

					D->output[j] = NINE;

					break;

				case '.':

					// if there's a decimal point, then take the previous number and toggle the decimal segment on
					D->output[j - 1] = D->output[j - 1] | DECIMAL;
					j = j - 1; // decimal is not its own character, so make sure next character is in the right output index

					break;

				default:

					printf("invalid character\n");
					return 1;
			}

			j++; // increment output buffer index
		}

		// write to ldc
		if(lcd_write(fd, D) == 1) { perror("could not write to lcd"); return 1; }	
		D->display_index++;
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
int lcd_write(int fd, DISP_T * D) {

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


	// display chosen effect first if we haven't already
	if(D->display_index == 0) {
		switch(D->effect) {
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
			// if displaying parameter, determine whether we need a negative sign or not, and then display it appropriately 
	} else if(D->input[D->display_index] < 0) {
		// first digit should be negative sign
		buffer[1] = NEG;

		// second digit is first digit of param
		buffer[3] = D->output[1];

		// third digit is second digit of param
		buffer[7] = D->output[2];

		// last digit is blank
		buffer[9] = 0x00;	// fourth letter

	} else if(D->input[D->display_index] >= 0) {
		// first digit is first digit of param
		buffer[1] = D->output[1];

		// second digit is second digit of param
		buffer[3] = D->output[2];

		// third digit is blank
		buffer[7] = 0x00;

		// last digit is blank
		buffer[9] = 0x00;	// fourth letter
	}


	// write the buffer to lcd
	if(write(fd, buffer, 17) < 0) { perror("could not write message to lcd"); return 1; }

	// display for 1 second
	usleep(1000000);

	return 0;
}
