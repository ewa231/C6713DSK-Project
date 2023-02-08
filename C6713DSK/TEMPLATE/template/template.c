#include "tonecfg.h"
#include "dsk6713.h"
#include "dsk6713_aic23.h"
#include "ledcfg.h"
#include "dsk6713_led.h"
#include "dsk6713_dip.h"
#include <math.h>


#define SINE_TABLE_SIZE   256
#define PI 3.14159265358979

int sample = 0;
int samp;
float Fsample[3] = {0, 0.33333333333333, 0.66666666666666};
int counter;
float incr;
int NAvg = 100;
float FFTsine[SINE_TABLE_SIZE];
float FFTsineN[SINE_TABLE_SIZE];
float FFTsineNAvg[SINE_TABLE_SIZE];
typedef struct {float real,imag;} COMPLEX;
void FFT(COMPLEX *Y, int n);	    		//FFT prototype
float rnd_uniform();
float rnd_normal();
void switches();
int SwitchesState = 0;
COMPLEX sFFTsine[SINE_TABLE_SIZE];
COMPLEX sFFTsineN[SINE_TABLE_SIZE];
COMPLEX sFFTsineNAvg[SINE_TABLE_SIZE]; 
COMPLEX w[SINE_TABLE_SIZE];       		    //twiddle constants stored in w 
DSK6713_AIC23_CodecHandle hCodec;
Uint32 fs = DSK6713_AIC23_FREQ_8KHZ;
/* Codec configuration settings */
DSK6713_AIC23_Config config = { \
    0x0017,  /* 0 DSK6713_AIC23_LEFTINVOL  Left line input channel volume */ \
    0x0017,  /* 1 DSK6713_AIC23_RIGHTINVOL Right line input channel volume */\
    0x00d8,  /* 2 DSK6713_AIC23_LEFTHPVOL  Left channel headphone volume */  \
    0x00d8,  /* 3 DSK6713_AIC23_RIGHTHPVOL Right channel headphone volume */ \
    0x0011,  /* 4 DSK6713_AIC23_ANAPATH    Analog audio path control */      \
    0x0000,  /* 5 DSK6713_AIC23_DIGPATH    Digital audio path control */     \
    0x0000,  /* 6 DSK6713_AIC23_POWERDOWN  Power down control */             \
    0x0043,  /* 7 DSK6713_AIC23_DIGIF      Digital audio interface format */ \
    0x0081,  /* 8 DSK6713_AIC23_SAMPLERATE Sample rate control */            \
    0x0001   /* 9 DSK6713_AIC23_DIGACT     Digital interface activation */   \
};

float sine[SINE_TABLE_SIZE] = {
-0,2866,3000,570,-1757,-1915,-515,381,-0,-381,515,1915,1757,-570,-3000,-2866,-0,2866,3000,570,-1757,-1915,-515,381,-0,-381,515,1915,1757,-570,-3000,-2866,-0,2866,3000,570,-1757,-1915,-515,381,-0,-381,515,1915,1757,-570,-3000,-2866,-0,2866,3000,570,-1757,-1915,-515,381,-0,-381,515,1915,1757,-570,-3000,-2866,-0,2866,3000,570,-1757,-1915,-515,381,-0,-381,515,1915,1757,-570,-3000,-2866,-0,2866,3000,570,-1757,-1915,-515,381,0,-381,515,1915,1757,-570,-3000,-2866,-0,2866,3000,570,-1757,-1915,-515,381,-0,-381,515,1915,1757,-570,-3000,-2866,-0,2866,3000,570,-1757,-1915,-515,381,-0,-381,515,1915,1757,-570,-3000,-2866,-0,2866,3000,570,-1757,-1915,-515,381,0,-381,515,1915,1757,-570,-3000,-2866,0,2866,3000,570,-1757,-1915,-515,381,0,-381,515,1915,1757,-570,-3000,-2866,-0,2866,3000,570,-1757,-1915,-515,381,-0,-381,515,1915,1757,-570,-3000,-2866,-0,2866,3000,570,-1757,-1915,-515,381,-0,-381,515,1915,1757,-570,-3000,-2866,-0,2866,3000,570,-1757,-1915,-515,381,0,-381,515,1915,1757,-570,-3000,-2866,0,2866,3000,570,-1757,-1915,-515,381,0,-381,515,1915,1757,-570,-3000,-2866,-0,2866,3000,570,-1757,-1915,-515,381,0,-381,515,1915,1757,-570,-3000,-2866,-0,2866,3000,570,-1757,-1915,-515,381,0,-381,515,1915,1757,-570,-3000,-2866
  };

float sineN[SINE_TABLE_SIZE];

float window[SINE_TABLE_SIZE] = {
0.000000,0.000134,0.000154,-0.002298,-0.008312,-0.011358,0.000514,0.029577,0.055254,0.044257,-0.016170,-0.092886,-0.122843,-0.066350,0.049343,0.142688,0.142688,0.049343,-0.066350,-0.122843,-0.092886,-0.016170,0.044257,0.055254,0.029577,0.000514,-0.011358,-0.008312,-0.002298,0.000154,0.000134,0.000000
};


void main()
{    
    incr = 18.5 * 6 * SINE_TABLE_SIZE / 48000;

    /* Initialize the board support library, must be called first */
    DSK6713_init();    
    /* Start the codec */
    hCodec = DSK6713_AIC23_openCodec(0, &config);

    /* Initialize the LED and DIP switch modules of the BSL */
    DSK6713_LED_init();
    DSK6713_DIP_init();
    
	for (sample = 0 ; sample<SINE_TABLE_SIZE ; sample++)
	{
	   w[sample].real = cos(2*PI*sample/512.0); //Re component of twiddle constants
	   w[sample].imag = -sin(2*PI*sample/512.0); //Im component of twiddle constants
	   sineN[sample] = sine[sample] / 2 + rnd_normal() / 2;
	   FFTsineNAvg[sample] = 0;			  
	}	
	/*=======================Calcultae FFT============================*/
	for (sample = 0 ; sample < SINE_TABLE_SIZE ; sample++)
	{
	     sFFTsine[sample].real=sine[sample]; //buffer with new data
		 sFFTsine[sample].imag = 0.0;	    //imag components = 0
		 sFFTsineN[sample].real=sineN[sample]; //buffer with new data
		 sFFTsineN[sample].imag = 0.0;	    //imag components = 0
	} 
	FFT(sFFTsine,SINE_TABLE_SIZE);              //call function FFT.c
	FFT(sFFTsineN,SINE_TABLE_SIZE);              //call function FFT.c
			/*============== Welsh =======================*/
	for(counter = 0; counter < NAvg; counter++){
		for (sample = 0 ; sample < SINE_TABLE_SIZE ; sample++)
		{
		   sFFTsineNAvg[sample].real = sine[sample] + rnd_normal(); //buffer with new data
		   sFFTsineNAvg[sample].imag = 0.0;	    //imag components = 0			  
		}
		FFT(sFFTsineNAvg,SINE_TABLE_SIZE);
		for (sample = 0 ; sample < SINE_TABLE_SIZE ; sample++)
		 {    
			FFTsineNAvg[sample] = FFTsineNAvg[sample] + sqrt(sFFTsineNAvg[sample].real*sFFTsineNAvg[sample].real + sFFTsineNAvg[sample].imag*sFFTsineNAvg[sample].imag)/(-16);	
		 } 
	}
	for(sample = 0; sample < SINE_TABLE_SIZE; sample++){
		FFTsineNAvg[sample] = FFTsineNAvg[sample] / NAvg;  
	}
			/* ===================== Welsh end=============*/ 
	for (sample = 0 ; sample < SINE_TABLE_SIZE ; sample++)
	{    
	    FFTsine[sample] = sqrt(sFFTsine[sample].real*sFFTsine[sample].real 
		     + sFFTsine[sample].imag*sFFTsine[sample].imag)/(-16);
		FFTsineN[sample] = sqrt(sFFTsineN[sample].real*sFFTsineN[sample].real 
		     + sFFTsineN[sample].imag*sFFTsineN[sample].imag)/(-16);
			switch (sample){
			case 0:
				FFTsine[sample] = 16000.0;
				FFTsineN[sample] = 16000.0;
				FFTsineNAvg[sample] = 16000.0;
				break;
			case 1:
				FFTsine[sample] = 32000.0;
				FFTsineN[sample] = 32000.0;
				FFTsineNAvg[sample] = 32000.0;
				break;
			case 2:
				FFTsine[sample] = 16000.0;
				FFTsineN[sample] = 16000.0;
				FFTsineNAvg[sample] = 16000.0;
				break;
			}//end of switch(sample)
	}
    /*===================end of FFT caculating=======================*/

    while(1)
    {         
        
        switches();
        
        switch (SwitchesState){
			case 1: //only DIP(0) pressed
				while (!DSK6713_AIC23_write(hCodec, sine[sample]));
				while (!DSK6713_AIC23_write(hCodec, sineN[sample]));
				break;
			case 2: //only DIP(1) pressed
				while (!DSK6713_AIC23_write(hCodec, FFTsine[sample]));
				while (!DSK6713_AIC23_write(hCodec, FFTsineN[sample]));
				break;
			case 4: //only DIP(2) pressed
				while (!DSK6713_AIC23_write(hCodec, FFTsineN[sample]));
				while (!DSK6713_AIC23_write(hCodec, FFTsineNAvg[sample]));
				break;
			case 8: //only DIP(3) pressed
				while (!DSK6713_AIC23_write(hCodec, sine[sample]));
				while (!DSK6713_AIC23_write(hCodec, sine[(int) Fsample[0]]));
				break;
			case 3: //DIP(0) and DIP(1) pressed
				while (!DSK6713_AIC23_write(hCodec, sine[(int) Fsample[0]]));
				while (!DSK6713_AIC23_write(hCodec, sine[(int) Fsample[0]]/3 + sine[(int) Fsample[1]]/3 + sine[(int) Fsample[2]]/3));
				break;
			case 5: // DIP(0) DIP(2) and  pressed
				while (!DSK6713_AIC23_write(hCodec, sine[sample]));
				while (!DSK6713_AIC23_write(hCodec, sine[sample] * window[sample]));;
				break;
			case 9: // DIP(0) DIP(3) and  pressed
				while (!DSK6713_AIC23_write(hCodec, sine[sample]));
				while (!DSK6713_AIC23_write(hCodec, sineN[sample]));
				break;
			default : // another DIPs  pressed
				while (!DSK6713_AIC23_write(hCodec, 0));
				while (!DSK6713_AIC23_write(hCodec, 0));
				break;
		}//end of switch(sample)	 
		sample = (sample + 1) % SINE_TABLE_SIZE;
		for (samp = 0; samp < 3; samp++)
		{
			Fsample[samp] = (Fsample[samp] + incr);
			if (Fsample[samp] >= SINE_TABLE_SIZE)	
				Fsample[samp] = Fsample[samp] - SINE_TABLE_SIZE;
		}	   
	}// end of infinite loop
}//end of main




/******************************************************
Generuje liczbe pseudolosowa z zakresu -32768 - 32767.
Generator o rozkladzie ROWNOMIERNYM.
******************************************************/
float rnd_uniform()
{
	#define a 48271
	#define q 44488
	#define r 3399
	static Int32 h, X;
    h = X / q;
	X = a * (X - q * h) - r * h;
	if (X < 0) X += 2147483647;
	return (float)(X >> 16);
}

/******************************************************
Generuje liczbe pseudolosowa z zakresu -32768 - 32767.
Generator o rozkladzie NORMALNYM.
******************************************************/
float rnd_normal()
{
	Int16 i;
	float xsum = 0;
	for (i = 0; i < 12; ++i) xsum += rnd_uniform();
	return (xsum / 12);
}


void switches()
{  
	int i;
	SwitchesState = 0;
	for (i = 0 ; i < 4 ; i++)
	{
		DSK6713_LED_off(i);
		if (!DSK6713_DIP_get(i))
		{
			SwitchesState = SwitchesState | (0x0001 << i);
			DSK6713_LED_on(i);
		}	
	}	
}
