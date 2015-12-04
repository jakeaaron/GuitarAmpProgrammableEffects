



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

#define D 0x4E
#define L 0x38
#define Y 0x6E

#define C 0x39
#define P 0x73
#define R 0x31
#define S 0x6D

#define E 0x79
#define Q 0x67




typedef struct display_struct {
	char * input;			// param vals to write input from python gui
	unsigned char * output;	// buffer containing segs to write to display
	int effect;
	int display_index;		// index of which param from the input is being displayed
} DISP_T;



// function declarations ----------------
int init_i2c(char * device_file);
int init_display(int fd);
unsigned char * char_to_7seg(double temp, unsigned char * output);
int lcd_write(int fd, double temp, unsigned char * temp_string);
// --------------------------------------