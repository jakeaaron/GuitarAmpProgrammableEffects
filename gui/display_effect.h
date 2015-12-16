

#ifndef DISP_H
#define DISP_H


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




typedef struct display_struct {
	float * input;			// param vals to write input from python gui
	unsigned char * output;	// buffer containing segs to write to display
	int * sign_buffer;		// this contains the sign of each input value, so that we don't have to deal with negative signs when trying to do string stuff
	int effect;				// 1 = delay, 2 = compressor, 3 = eq; used for writing effect to display
	int display_index;		// index of which param from the input is being displayed
	int fd;					// file descriptor for i2c to talk to display
} DISP_T;



// function declarations ----------------
DISP_T * init_params(int fd, char ** argv);
int init_i2c(char * device_file);
int init_display(int fd);
int char_to_7seg(int argc, DISP_T * D_T);
int lcd_write(DISP_T * D_T);
int clear_display(int fd);
// --------------------------------------


#endif