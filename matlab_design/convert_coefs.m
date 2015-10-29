%% Format Coefs
% format coefs for use with arm iir routine.
% exports coefs from fda tool into workspace and formats
% coefs:
% { b0, b1, b2, a1, a2 }
% 


% take iir coefs and format them for 
for row = 1:length(SOS(:,1))
   line = sprintf('%f, %f, %f, %f, %f,', G(row) * SOS(row,1:3), SOS(row,5:6));
   disp(line);
end