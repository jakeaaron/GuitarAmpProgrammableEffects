%% Delay Effect
% basic delay structure, allowing for user control of delay amount and
% delay volume
% 
% parameters:
%   seconds_delay - how many seconds the signal is delayed before being
%   output
%   echo_volume - the volume of the delayed output as a factor of the input
%   volume
%   

% Jacob Allenwood
% July 18, 2015

%% Filter Design




% keep in mind for dsp board:
% % % % % % 
% To increase the delay of a signal, either the buffer size must be 
% increased to store more samples, or the sampling rate can be decreased 
% to increase the delay. Tradeoffs must be considered when choosing longer 
% delay times. Sometimes a DSP only has a limited amount of memory 
% available. The higher the bandwidth requirement of the incoming signal, 
% the more memory storage required by the DSP. But, by decreasing the 
% sampling rate, the bandwidth is reduced. In some cases this is not a 
% problem. For example, human voices or stringed instruments have a 
% bandwidth of only up to 6 kHz. In such cases, a smaller sampling rate 
% will not limit the with the frequency range of the instrument.
% % % % % % 



% create unit step
% x = zeros(10,1);   
% x(1:10) = 1;   % array of 99 zeros and a 1 at the origin

% input signal to delay
infile = 'file.wav';
outfile = 'outfile.wav';

[x,Fs] = audioread(infile);

% set up parameters for delay
seconds_delay = .5;
% decay_factor = ;
echo_volume = 1.5;

M = seconds_delay * Fs; % sample delay

% allocate memory for delay buffer. increasing amount of values stored in
% buffer will increase the delay of the signal
delayline = zeros(M,1); % delayline is buffer to store delayed values
% create output array
y = zeros(length(x), 1);

% hold the current sample for M samples long, then add to the current
% sample
factor = 1;
for n=1:length(x) 
    y(n) = x(n) + (echo_volume * delayline(M));
    delayline = [x(n); delayline(1:M-1)]; % shift in the delayed sample
end

% write output
audiowrite(outfile,y,Fs);