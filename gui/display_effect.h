/**
 * @file [display_effect.h]
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
 * 			
 * @brief [this file contains the defines for the 7 segment numbers, and the function prototypes to display the 
 * effect and params on the display.]
 * 
 * 
 * 
 */

#ifndef DISP_H
#define DISP_H


// 7 SEG DEFINES -------------------------------------

#define ZERO 0x3F
#define ONE 0x06
#define TWO 0x5B
#define THREE 0x4F
#define FOUR 0x66
#define FIVE 0x6D
#define SIX 0x7D
#define SEVEN 0x07
#define EIGHT 0x7F
#define NINE 0x67

#define DECIMAL 0x80
#define DEGREE 0x63
#define NEG 0x40

#define D 0x5E
#define L 0x38
#define A 0x77
#define Y 0x6E

#define C 0x39
#define P 0x73
#define R 0x31
#define S 0x6D

#define E 0x79
#define Q 0x67

// ---------------------------------------------------




typedef struct display_struct {
	float * input;			// param vals to write input from python gui
	unsigned char * output;	// buffer containing segs to write to display
	int * sign_buffer;		// this contains the sign of each input value, so that we don't have to deal with negative signs when trying to do string stuff
	int effect;				// 1 = delay, 2 = compressor, 3 = eq; used for writing effect to display
	int display_index;		// index of which param from the input is being displayed
	int fd;					// file descriptor for i2c to talk to display
} DISP_T;


/**
 * @brief [determines the effect parameters from the gui]
 * @details [note that the values are mapped in a certain way to avoid passing negative numbers and decimals from file to file]
 * 
 * @param argv [array of command line arguments]
 * @return [pointer to display structure or NULL on error]
 */
DISP_T * init_params(
	int fd, 
	char ** argv
);


/**
 * @brief [initialize i2c]
 * @details [open i2c device file and set i/o slave address]
 * 
 * @param device_file [path to the device file]
 * @return [file descriptor from device file for use with other functions]
 */
int init_i2c(
	char * device_file
);


/**
 * @brief [initialize lcd display]
 * @details [set up oscillator, display and brightness]
 * 
 * @param fd [file descriptor to write to]
 * @return [0 on success, 1 on error]
 */
int init_display(
	int fd
);


/**
 * @brief [converts input characters to the equivalent value in 7seg form]
 * @details [7seg format is the hex values found in the header file. each bit turns a specific part 
 * of the display on. note the indexing trick when finding a decimal point. the decimal is not its
 * own character, so we just turn it on for the previous character and make sure our index remains
 * correct.]
 * 
 * @param argc [number of values to be displayed/converted into 7seg format]
 * @param D_T [pointer to display structure]
 * 
 * @return [0 success, 1 error]
 */
int char_to_7seg(
	int argc, 
	DISP_T * D_T
);


/**
 * @brief [writes the effect and the parameters determined in the init_params to the display]
 * 
 * @param D_T [pointer to display structure]
 * 
 * @return [0 on success, 1 on error]
 */
int lcd_write(
	DISP_T * D_T
);


/**
 * @brief [clears all the segments on the display for closing the application]
 * 
 * @param fd [file descriptor to the i2c display device]
 * @return [0 on success, 1 on error]
 */
int clear_display(
	int fd
);


#endif