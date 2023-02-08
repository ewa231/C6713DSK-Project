function sinusy()

filefolder=uigetdir;


Fs = 8000;            			% Sampling frequency
T = 1/Fs;             			% Sampling period
L = 256;             			% Length of signal
t = (0:L-1)*T;        			% Time vector

Amp1=6400/4;

Freq1=1500;

sin1 = Amp1*sin(2*pi*Freq1*t);		% Sine vector
sin1=skaluj(sin1);
figure(1)
plot(1000*t(1:50),sin1(1:50))		% Plot the first 50 samples
title('My Sinus')
xlabel('t [ms]')
ylabel('sin(t)')

NazwaPliku=[filefolder '\sin1.txt'];
fileID = fopen(NazwaPliku,'w');
dlmwrite(NazwaPliku,sin1,'precision','%.0f');
fclose(fileID);

Amp2=6400/8;
Freq2=1000;

sin2 = Amp2*sin(2*pi*Freq2*t);		% Sine vector
sin2=skaluj(sin2);
figure(2)
plot(1000*t(1:50),sin2(1:50))		% Plot the first 50 samples
title('My Sinus')
xlabel('t [ms]')
ylabel('sin(t)')

NazwaPliku=[filefolder '\sin2.txt'];
fileID = fopen(NazwaPliku,'w');
dlmwrite(NazwaPliku,sin2,'precision','%.0f');
fclose(fileID);

sinus=sin1+sin2;
sinus=skaluj(sinus);
figure(3)
plot(1000*t(1:50),sinus(1:50))		% Plot the first 50 samples
title('My Sinus')
xlabel('t [ms]')
ylabel('sin(t)')

NazwaPliku=[filefolder '\sin.txt'];
fileID = fopen(NazwaPliku,'w');
dlmwrite(NazwaPliku,sinus,'precision','%.0f');
fclose(fileID);

noise=150*randn(1, L);
sinnoise=sinus+noise;
sinnoise=skaluj(sinnoise);
figure(4)
plot(1000*t(1:50),sinnoise(1:50))		% Plot the first 50 samples
title('My Sinus')
xlabel('t [ms]')
ylabel('sin(t)')

NazwaPliku=[filefolder '\sinnoise.txt'];
fileID = fopen(NazwaPliku,'w');
dlmwrite(NazwaPliku,sinnoise,'precision','%.0f');



window=ones([1 96]);
NazwaPliku=[filefolder '\window.txt'];
fileID = fopen(NazwaPliku,'w');
dlmwrite(NazwaPliku,window,'precision','%.0f');
fclose(fileID);
end


function przeskalowany=skaluj(A)

minimum=min(min(A));
A=A-min(min(A));

maximum=max(max(A));
A=A/maximum*6000;

przeskalowany=A-3000;

end