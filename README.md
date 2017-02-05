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
As seen, multiplying the desired time delay by the sampling frequency *Fs* results in the amount of samples *N* that represents that time delay *t*. This makes sense when one realizes that the sampling frequency is how many samples are taken in one second.

### Audio Compression     (calc_rms.c/h && compressor.c/h)

The compressor was designed to attenuate the gain of the input signal (or ’compress’ the dynamic range of the signal) if the Root-Mean-Square (RMS, a common averaging value found in audio compression) value surpassed a user defined threshold value (specific gain value limit).

The RMS routine takes a parameter window size, which is the number of samples to average over. The number of samples to average over is a significant part of the design, as a window too small will not fully represent a signal correctly, but a window too large introduces a large amount of delay in the compressor (which sometimes is desired, but should be controlled with the attack and release parameters).

![](https://qph.ec.quoracdn.net/main-qimg-37ad04e22ce5926e093f12111859005f)

The RMS routine uses a running square so that the entire RMS (square root of the mean of the squares) does not need to be calculated every time for every new sample (meaning summing up all the squares from all of the previous input values back to window size values away, which would be re-calculating all of the same values except the new one); rather there is always a running total of the squares so for each output value, only one new RMS value needs to be calculated.



### Three-Band Equalizer  (eq.c/h)

The basic idea of the equalizer algorithm is to use FIR filters to control different frequency bands, and use the delay algorithm to keep the input signal in phase with each filter output.

The basic Finitie Impulse Response algorithm is shown as:
![](https://upload.wikimedia.org/wikipedia/commons/thumb/9/9b/FIR_Filter.svg/300px-FIR_Filter.svg.png)

FIR filters inherently apply a delay to the guitar signal, which can be calculated by ` D = M−1 / 2 `
where M is the number of filter coefficients. The delay blocks themselves also delay the input signal the exact same amount as the FIR filters, to keep each signal in phase with each other so they can be added or subtracted, to achieve the desired band. For instance, to achieve the mid band of the equalizer, the input is filtered with a cutoff frequency (the frequency at which the input is ’cutoff’, meaning the signal is virtually non-existent beyond the cutoff frequency) of 350Hz (this output is the low frequency band). That filter output is then subtracted from the delayed input signal to get the rest of the frequency band that was not filtered. With each filter, the output is the frequency band below the cutoff, and then the rest of the signal beyond the cutoff is retrieved by subtracting the (delayed, or in-phase) input by the filter output.

In this design, the equalizer is a 3-band equalizer. The 3 bands:

1. Low band; found by low-pass filtering the input out to 350Hz and then delaying to stay in phase for the final addition of each band output
2. Mid band; found by low-pass filtering the in-phase (delayed) input subtracted by the low band, out to 1050Hz
3. High band; found by subtracting the in-phase (delayed, again) signal going into the mid-band FIR filter, by the mid-band filter output (remember that the mid-band filter input is the in-phase input signal subtracted by the low-band filter output)
  
Since each band is separated and in-phase, then simply scaling the corresponding bands and adding them up will result in the ’equalized’ guitar signal. The ’scaling’ value is found by using ` Gain = 10^x/20 `, where x is the gain value in decibels, which is selected by the user.
