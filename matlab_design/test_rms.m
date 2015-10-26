%% Root Mean Square Test
% this is a very minimal test for comparing the c routines for the
% rms calculation.
% The values were set the same in the c test and the same answer of an rms
% of 0.5339 was achieved by this script, the c routine, and by hand.
%   

% Jacob Allenwood
% October 26, 2015


%% RMS Calculation

block_size = 10;
square = 0;
x = [0.0 0.1 0.2 0.3 0.4 0.5 0.6 0.7 0.8 0.9];
y = 0;

% go through entire input
for j=1:block_size    
    % sum the squares for the rms calc
    square = square + x(j)^2;  
end

% calc rms value
y = sqrt(square / block_size);
    


