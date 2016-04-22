# Guitar Amplifier with Programmable Effects
Computer Engineering Capstone 2016: GUI controlled modeling of audio effects using digital signal processing with continuous user feedback. 

## Digital Signal Processing Algorithms

The basic idea of DSP is to convert real-life continuous-time signals into digital discrete-time signals to be mathematically manipulated and used for another purpose. This is done by 'sampling' (taking a finite number of snapshots of the signal) a continuous-time signal and representing it with '1's and '0's. These algorithms were developed to apply digital guitar effects to the incoming guitar signal.

The main.c file grabs a block of input samples, filters the input to reject any high frequency noise, and then applies the user-selected guitar affect on the input samples. This is the basic workflow:

1. Initialize clock, timers, etc.
2. Wait for the 'valid send' pin to be set, meaning there is now effect and parameter information to be read
3. Read and update the effect and parameter values for the DSP
4. Initialize the ADC and DAC for streaming input and output data
5. Declare and initialize variables for calculations and input/output buffers
6. Initialize FIR filter for low-pass filtering the guitar signal out to 10kHz (high end of the frequency range of guitars)
7. Initialize the selected effect (set up data structures, etc)
8. Continuously:
    * Get a block of input samples
    * Run the FIR low-pass out to 10kHz on the block
    * Run the selected effect calculation


### Audio Delay           (delay.c/h)

This algorithm uses circular buffering with a buffer of *N* samples long to delay the guitar signal by a user-defined amount of time *t*. 

This is found by:
` N = t * Fs `
As seen, multiplying the desired time delay by the sampling frequency *Fs* results in the amount of samples *N* that represents that time delay *t*.

### Audio Compression     (calc_rms.c/h && compressor.c/h)

### Three-Band Equalizer  (eq.c/h)
