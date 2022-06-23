% ------------------------------------------------------------------------­
% 2.161 Classroom Example - iirtdf2 - Demonstration Transposed IIR Direct 
% Form II implementation. 
% Usage : 1) Initialization: 
% y = iirtdf2('initial', b, a) 
% where b, a are the numerator and denominator polynomial 
% coefficients. Example: 
% [b,a] = butter(7,0.4); 
% y = iirtdf2(’initial’,b,a); 
% Note: iirdf2 returns y = 0 for initialization 
% 2) Filtering: 
% y_out = iirtdf2(f_{in}; 
% where f_in is a single input value, and 
% y_out is the computed output value. 
% Example: To compute the step response: 
% for j=1:100 
%   y(j) = iirtdf2(1);
% end 
% ------------------------------------------------------------------------­
% 

function y_n = iirtdf2(f_n, B, A) 
persistent reg_fix register Bx Ax N

% 
% The following is initialization, and is executed once 
% 
if (ischar(f_n) && strcmp(f_n,'initial')) 
    N = length(A);
    Ax = A; 
    Bx = B; 
    register = zeros(1, N - 1);
    reg_fix;
    y_n = 0;
    else
    % Filtering: (Note that a Transposed Direct Form II filter needs only a single 
    % register.) Also note that this is not strictly a shift register. 
    y_n = register(1) + Bx(1) * f_n;
    % Update for the next iteration 
    for J = 1 : N - 2 
    register(J) = register(J + 1) + Bx(J + 1) * f_n - Ax(J + 1) * y_n;
    end 
    register(N - 1) = Bx(N) * f_n - Ax(N) * y_n;
    reg_fix = split(fi(register, 1, 23, 18).dec);
end 
