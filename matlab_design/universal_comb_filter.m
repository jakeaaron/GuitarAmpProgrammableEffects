%% Universal Comb Filter
% basic delay structure, using a combination of FIR and IIR comb filters
% 
% parameters:
%   FF - feedforward multiplier
%   FB - feedback multiplier
%   BL - blend
% with these parameters, this universal filter can form any comb filter, an
% allpass filter, or a delay:
% ______________________________
% ------------------------------
% | Filter Type	| BL | FB | FF |
% ______________________________
% ------------------------------
% | FIR Comb	| 1  | 0  | G  |
% ------------------------------
% | IIR Comb    | 1  | G  | 0  |
% ------------------------------
% | Allpass	    | a  | -a | 1  |
% ------------------------------
% | Delay	    | 0  | 0  | 1  |
% ------------------------------
% 
% Jacob Allenwood
% May 19, 2015

%% Filter Design

% create unit step
% x = zeros(10,1);   
% x(1) = 1;   % array of 99 zeros and a 1 at the origin

% input signal to delay
infile = 'file.wav';
outfile = 'outfile.wav';

[x,Fs] = audioread(infile);

% set up parameters for specific filter type
BL = 0;     % blend
FB = 0;     % feedback
FF = 1;     % feedforward
M = 50000;      % sample delay

% allocate memory for delay buffer. increasing amount of values stored in
% buffer will increase the delay of the signal
delayline = zeros(M,1); % delayline is buffer to store delayed values
% create output array
y = zeros(length(x), 1);

for n=1:length(x) 
    x_h = x(n) + FB*delayline(M);
    y(n) = x(n) + FF*delayline(M) + BL*x_h;
    delayline = [x_h; delayline(1:M-1)];
end

% write output
audiowrite(outfile,y,Fs);