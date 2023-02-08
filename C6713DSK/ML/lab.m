Fs = 8000; % Sampling frequency
 T = 1/Fs; % Sampling period
 L = 256; % Length of signal
 t = (0:L-1)*T; % Time vector
 mysin1 = 8000*sin(2*pi*1000*t); % Sine vector
 mysin2 = 24000*sin(2*pi*2000*t); % Sine vector
 mysin = mysin1+mysin2;
 plot(1000*t(1:50),mysin(1:50)) % Plot the first 50 samples
 title('My Sinus')
 xlabel('t [ms]')
 ylabel('sin(t)')