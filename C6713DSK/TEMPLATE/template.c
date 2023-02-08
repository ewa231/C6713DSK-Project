#include "tonecfg.h"
#include "dsk6713.h"
#include "dsk6713_aic23.h"
#include "ledcfg.h"
#include "dsk6713_led.h"
#include "dsk6713_dip.h"
#include <math.h>


#define SINE_TABLE_SIZE   256
#define PI 3.14159265358979
#define W 10
#define F 5

int sample = 0;
int i = 0;
float FFTsine[SINE_TABLE_SIZE];
float FFTsineN[SINE_TABLE_SIZE];
float FFTsineW[SINE_TABLE_SIZE];
float FFTsineF[SINE_TABLE_SIZE];
float sineN[SINE_TABLE_SIZE];
float sineDec[SINE_TABLE_SIZE];
float sineInt[SINE_TABLE_SIZE];
float sineF[SINE_TABLE_SIZE];
typedef struct { float real, imag; } COMPLEX;
void FFT(COMPLEX* Y, int n);	//FFT prototype
void switches();
float uNoise();
float nNoise();
void Welch(float* sigFFT, float* WFFT);
void myFFT(float* sig, float* sigFFT);
void decimation(float* sig, float* sigDec);
void interpolation(float* sig, float* sigInt);
float filtration(float* sig, float* sigF);
int SwitchesState = 0;
int SwitchesStateOld = 0;
COMPLEX sFFTsine[SINE_TABLE_SIZE];
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
0.00,29656.85,8000.00,-18343.15,-0.00,18343.15,-8000.00,-29656.85,-0.00,29656.85,8000.00,-18343.15,-0.00,18343.15,-8000.00,-29656.85,-0.00,29656.85,8000.00,-18343.15,-0.00,18343.15,-8000.00,-29656.85,-0.00,29656.85,8000.00,-18343.15,-0.00,18343.15,-8000.00,-29656.85,-0.00,29656.85,8000.00,-18343.15,-0.00,18343.15,-8000.00,-29656.85,-0.00,29656.85,8000.00,-18343.15,-0.00,18343.15,-8000.00,-29656.85,-0.00,29656.85,8000.00,-18343.15,0.00,18343.15,-8000.00,-29656.85,-0.00,29656.85,8000.00,-18343.15,-0.00,18343.15,-8000.00,-29656.85,-0.00,29656.85,8000.00,-18343.15,0.00,18343.15,-8000.00,-29656.85,-0.00,29656.85,8000.00,-18343.15,-0.00,18343.15,-8000.00,-29656.85,-0.00,29656.85,8000.00,-18343.15,0.00,18343.15,-8000.00,-29656.85,-0.00,29656.85,8000.00,-18343.15,-0.00,18343.15,-8000.00,-29656.85,-0.00,29656.85,8000.00,-18343.15,0.00,18343.15,-8000.00,-29656.85,0.00,29656.85,8000.00,-18343.15,-0.00,18343.15,-8000.00,-29656.85,-0.00,29656.85,8000.00,-18343.15,0.00,18343.15,-8000.00,-29656.85,-0.00,29656.85,8000.00,-18343.15,-0.00,18343.15,-8000.00,-29656.85,-0.00,29656.85,8000.00,-18343.15,-0.00,18343.15,-8000.00,-29656.85,0.00,29656.85,8000.00,-18343.15,-0.00,18343.15,-8000.00,-29656.85,-0.00,29656.85,8000.00,-18343.15,-0.00,18343.15,-8000.00,-29656.85,-0.00,29656.85,8000.00,-18343.15,-0.00,18343.15,-8000.00,-29656.85,-0.00,29656.85,8000.00,-18343.15,-0.00,18343.15,-8000.00,-29656.85,0.00,29656.85,8000.00,-18343.15,-0.00,18343.15,-8000.00,-29656.85,-0.00,29656.85,8000.00,-18343.15,-0.00,18343.15,-8000.00,-29656.85,-0.00,29656.85,8000.00,-18343.15,-0.00,18343.15,-8000.00,-29656.85,-0.00,29656.85,8000.00,-18343.15,-0.00,18343.15,-8000.00,-29656.85,0.00,29656.85,8000.00,-18343.15,-0.00,18343.15,-8000.00,-29656.85,0.00,29656.85,8000.00,-18343.15,-0.00,18343.15,-8000.00,-29656.85,-0.00,29656.85,8000.00,-18343.15,-0.00,18343.15,-8000.00,-29656.85,-0.00,29656.85,8000.00,-18343.15,-0.00,18343.15,-8000.00,-29656.85,0.00,29656.85,8000.00,-18343.15,-0.00,18343.15,-8000.00,-29656.85,-0.00,29656.85,8000.00,-18343.15,-0.00,18343.15,-8000.00,-29656.85,-0.00,29656.85,8000.00,-18343.15,-0.00,18343.15,-8000.00,-29656.85
};

const int BL = 32;
const double B[32] = {
    0.01586973020855, -0.02001783677786, -0.02428382646759,  0.02860889716013,
    0.03293120886419, -0.03718704840262, -0.04131205778646,  0.04524249876178,
    0.04891652478868, -0.05227543120271,  -0.0552648545357,  0.05783589293569,
    0.05994612130494,  -0.0615604771294, -0.06265199594641,  0.06320237891383,
    0.06320237891383, -0.06265199594641,  -0.0615604771294,  0.05994612130494,
    0.05783589293569,  -0.0552648545357, -0.05227543120271,  0.04891652478868,
    0.04524249876178, -0.04131205778646, -0.03718704840262,  0.03293120886419,
    0.02860889716013, -0.02428382646759, -0.02001783677786,  0.01586973020855
};

void main()
{
	/*=======================Inicjalizacja p³ytki============================*/
	DSK6713_init();
	/*=======================Inicjalizacja kodeka============================*/
	hCodec = DSK6713_AIC23_openCodec(0, &config);

	/*=======================Inicjalizacja prze³¹czników i diod============================*/
	DSK6713_LED_init();
	DSK6713_DIP_init();



	/*=======================Gotowy fragment obliczaj¹cy wspó³czynnik W do FFT============================*/
	for (sample = 0; sample < SINE_TABLE_SIZE; sample++)
	{
		w[sample].real = cos(2 * PI * sample / 512.0);
		w[sample].imag = -sin(2 * PI * sample / 512.0);
	}
	/*=======================Fragment kodu do obliczenia FFT dla tablicy "sine"============================*/
	for (sample = 0; sample < SINE_TABLE_SIZE; sample++)
	{
		sFFTsine[sample].real = sine[sample]; // Okreœlenie Re
		sFFTsine[sample].imag = 0.0;	    //Okreœlenie Im = 0
	}
	FFT(sFFTsine, SINE_TABLE_SIZE);              //Wywo³anie funkcji FFT.c

	for (sample = 0; sample < SINE_TABLE_SIZE; sample++)
	{
		FFTsine[sample] = sqrt(sFFTsine[sample].real * sFFTsine[sample].real
			+ sFFTsine[sample].imag * sFFTsine[sample].imag) / (-16);

		// tu wstawiæ wartoœæ próbki zerowej FFTsine[0]	tak, aby wyznacza³a pr¹¿ek zerowy
		if (sample == SINE_TABLE_SIZE - 1)
		{
			FFTsine[1] = -32000;
			FFTsine[0] = FFTsine[1] / 2;
			FFTsine[2] = FFTsine[1] / 2;
		}
		// usunaæ efekt Gibbsa	
	}

	decimation(sine, sineDec);

	interpolation(sineDec, sineInt);

	/*===================Pêtla nieskoñczona=======================*/
	while (1)
	{
		sineN[sample] = 0.8 * sine[sample] + 0.2 * nNoise();
		switches();
		if (!DSK6713_DIP_get(0)) //je¿eli wciœniêty jest prze³¹cznik 1
		{
			if (!DSK6713_DIP_get(1))
			{
				while (!DSK6713_AIC23_write(hCodec, sineDec[sample]));
				while (!DSK6713_AIC23_write(hCodec, sineInt[sample]));
			}
			else if (!DSK6713_DIP_get(2))
			{
				while (!DSK6713_AIC23_write(hCodec, sine[sample]));
				while (!DSK6713_AIC23_write(hCodec, sineInt[sample]));
			}
			else if (!DSK6713_DIP_get(3))
			{
				if (sample == SINE_TABLE_SIZE - 1) 
				{
					filtration(sineN, sineF);
				}
				while (!DSK6713_AIC23_write(hCodec, sineN[sample]));
				while (!DSK6713_AIC23_write(hCodec, sineF[sample]));
			}
			else
			{
				while (!DSK6713_AIC23_write(hCodec, sine[sample]));
				while (!DSK6713_AIC23_write(hCodec, sineN[sample]));
			}
		}
		else
		{
			if (!DSK6713_DIP_get(1))
			{
				if (sample == SINE_TABLE_SIZE - 1) 
				{
					myFFT(sineN, FFTsineN);
				}
				while (!DSK6713_AIC23_write(hCodec, FFTsine[sample]));
				while (!DSK6713_AIC23_write(hCodec, FFTsineN[sample]));
			}
			else if (!DSK6713_DIP_get(2))
			{
				if (sample == SINE_TABLE_SIZE - 1) 
				{
					myFFT(sineN, FFTsineN);
				}
				while (!DSK6713_AIC23_write(hCodec, FFTsineN[sample]));
				while (!DSK6713_AIC23_write(hCodec, FFTsineW[sample]));
			}
			else if (!DSK6713_DIP_get(3))
			{
				while (!DSK6713_AIC23_write(hCodec, sine[sample]));
				while (!DSK6713_AIC23_write(hCodec, sineDec[sample]));
			}
			else
			{
				while (!DSK6713_AIC23_write(hCodec, 0)); //na kanale CH1 wyœwietl 0
				while (!DSK6713_AIC23_write(hCodec, 0)); //na kanale CH1 wyœwietl 0
			}
		}
		sample = (sample + 1) % SINE_TABLE_SIZE; //zwiêksz numer próbki

	}// koniec pêtli nieskoñczonej
}//koniec funkcji main


void switches()
{

	if (!DSK6713_DIP_get(0))
	{
		SwitchesState |= (1 << 0);
	}
	else
	{
		SwitchesState &= ~(1 << 0);
	}

	if (!DSK6713_DIP_get(1))
	{
		SwitchesState |= (1 << 1);
	}
	else
	{
		SwitchesState &= ~(1 << 1);
	}

	if (!DSK6713_DIP_get(2))
	{
		SwitchesState |= (1 << 2);
	}
	else
	{
		SwitchesState &= ~(1 << 2);
	}

	if (SwitchesState != SwitchesStateOld)
	{
		DSK6713_LED_off(SwitchesStateOld - 1);
		if (SwitchesState != 0 && SwitchesState <= 4)
		{
			DSK6713_LED_off(SwitchesState - 1);
		}
		SwitchesStateOld = SwitchesState;
	}
}

float uNoise()
{
	static int X = 1;
	static int h = 0;
	float ret = 0;
	h = X / 44488;
	X = 48271 * (X - 44488 * h) - 3399 * h;
	if (X < 0)
	{
		X = X + 2147483647;
	}
	ret = (float)(X >> 16);
	return ret;
}

float nNoise()
{
	return uNoise() + uNoise() + uNoise();
}

void Welch(float* sigFFT, float* WFFT)
{
	static float FFTs[SINE_TABLE_SIZE][W];
	static int i = 0;
	int sample2 = 0;
	int j = 0;
	for (sample2 = 0; sample2 < SINE_TABLE_SIZE; sample2++)
	{
		FFTs[sample2][i] = sigFFT[sample2];
		for (j = 0; j < W; j++)
		{
			WFFT[sample2] += FFTs[sample2][j];
		}
		WFFT[sample2] = WFFT[sample2]/W;
	}
	i++;
}

void myFFT(float* sig, float* sigFFT) 
{
	if (sample == SINE_TABLE_SIZE - 1)
	{
		for (sample = 0; sample < SINE_TABLE_SIZE; sample++)
		{
			sFFTsine[sample].real = sig[sample]; // Okreœlenie Re
			sFFTsine[sample].imag = 0.0;	    //Okreœlenie Im = 0
		}
		FFT(sFFTsine, SINE_TABLE_SIZE);
		for (sample = 0; sample < SINE_TABLE_SIZE; sample++)
		{
			sigFFT[sample] = sqrt(sFFTsine[sample].real * sFFTsine[sample].real
				+ sFFTsine[sample].imag * sFFTsine[sample].imag) / (-16);
		}
		sigFFT[1] = -32000;
		sigFFT[0] = sigFFT[1] / 2;
		sigFFT[2] = sigFFT[1] / 2;
		sample = SINE_TABLE_SIZE - 1;
	}
}

void decimation(float* sig, float* sigDec)
{
	for (sample = 0; sample < SINE_TABLE_SIZE/2; sample++) 
	{
		sigDec[sample] = sig[2*sample];
		sigDec[sample+SINE_TABLE_SIZE/2] = sig[2 * sample];
	}
}

void interpolation(float* sig, float* sigInt)
{
	for (sample = 0; sample < SINE_TABLE_SIZE; sample++)
	{
		if (sample == 0)
		{
			sigInt[sample] = sig[sample];
		}
		else 
		{
			sigInt[sample] = (sig[sample - 1] + sig[sample]) / 2;
		}
	}
}

float filtration(float* sig, float* sigF)
{
	int i = 0;
	int j = 0;
	for (j=0; j <SINE_TABLE_SIZE/2 ; j++)
	{
		sigF[j] = 0;
		sigF[j+SINE_TABLE_SIZE/2] = 0;
		for (i = 0; i < BL; i++)
		{
			sigF[j] += sig[j+i]*(float)B[i];
			sigF[j+SINE_TABLE_SIZE/2] += sig[j+i]*(float)B[i];
		}
	}
}