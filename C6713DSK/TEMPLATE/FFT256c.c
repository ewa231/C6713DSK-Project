//FFT256c.c FFT implementation calling a C-coded FFT function 

#include "dsk6713_aic23.h"
Uint32 fs=DSK6713_AIC23_FREQ_8KHZ;

#include <math.h>                          
#define PTS 256			    //# of points for FFT 
#define PI 3.14159265358979
typedef struct {float real,imag;} COMPLEX;
void FFT(COMPLEX *Y, int n);	    //FFT prototype
float iobuffer[PTS];   		    //as input and output buffer
float x1[PTS];         		    //intermediate buffer 
float okno[PTS]={0.0039,0.0117,0.0195,0.0273,0.0352,0.0430,0.0508,0.0586,0.0664,0.0742,0.0820,
0.0898,0.0977,0.1055,0.1133,0.1211,0.1289,0.1367,0.1445,0.1523,0.1602,
0.1680,0.1758,0.1836,0.1914,0.1992,0.2070,0.2148,0.2227,0.2305,0.2383,
0.2461,0.2539,0.2617,0.2695,0.2773,0.2852,0.2930,0.3008,0.3086,0.3164,
0.3242,0.3320,0.3398,0.3477,0.3555,0.3633,0.3711,0.3789,0.3867,0.3945,
0.4023,0.4102,0.4180,0.4258,0.4336,0.4414,0.4492,0.4570,0.4648,0.4727,
0.4805,0.4883,0.4961,0.5039,0.5117,0.5195,0.5273,0.5352,0.5430,0.5508,
0.5586,0.5664,0.5742,0.5820,0.5898,0.5977,0.6055,0.6133,0.6211,0.6289,
0.6367,0.6445,0.6523,0.6602,0.6680,0.6758,0.6836,0.6914,0.6992,0.7070,
0.7148,0.7227,0.7305,0.7383,0.7461,0.7539,0.7617,0.7695,0.7773,0.7852,
0.7930,0.8008,0.8086,0.8164,0.8242,0.8320,0.8398,0.8477,0.8555,0.8633,
0.8711,0.8789,0.8867,0.8945,0.9023,0.9102,0.9180,0.9258,0.9336,0.9414,
0.9492,0.9570,0.9648,0.9727,0.9805,0.9883,0.9961,0.9961,0.9883,0.9805,
0.9727,0.9648,0.9570,0.9492,0.9414,0.9336,0.9258,0.9180,0.9102,0.9023,
0.8945,0.8867,0.8789,0.8711,0.8633,0.8555,0.8477,0.8398,0.8320,0.8242,
0.8164,0.8086,0.8008,0.7930,0.7852,0.7773,0.7695,0.7617,0.7539,0.7461,
0.7383,0.7305,0.7227,0.7148,0.7070,0.6992,0.6914,0.6836,0.6758,0.6680,
0.6602,0.6523,0.6445,0.6367,0.6289,0.6211,0.6133,0.6055,0.5977,0.5898,
0.5820,0.5742,0.5664,0.5586,0.5508,0.5430,0.5352,0.5273,0.5195,0.5117,
0.5039,0.4961,0.4883,0.4805,0.4727,0.4648,0.4570,0.4492,0.4414,0.4336,
0.4258,0.4180,0.4102,0.4023,0.3945,0.3867,0.3789,0.3711,0.3633,0.3555,
0.3477,0.3398,0.3320,0.3242,0.3164,0.3086,0.3008,0.2930,0.2852,0.2773,
0.2695,0.2617,0.2539,0.2461,0.2383,0.2305,0.2227,0.2148,0.2070,0.1992,
0.1914,0.1836,0.1758,0.1680,0.1602,0.1523,0.1445,0.1367,0.1289,0.1211,
0.1133,0.1055,0.0977,0.0898,0.0820,0.0742,0.0664,0.0586,0.0508,0.0430,
0.0352,0.0273,0.0195,0.0117,0.0039};

short i;               		    //general purpose index variable                  
short buffercount = 0;     	    //number of new samples in iobuffer         
short flag = 0;        		    //set to 1 by ISR when iobuffer full   
COMPLEX w[PTS];       		    //twiddle constants stored in w 
COMPLEX samples[PTS];  		    //primary working buffer                                              

main()
{

 for (i = 0 ; i<PTS ; i++)	    // set up twiddle constants in w 
  {
   w[i].real = cos(2*PI*i/512.0); //Re component of twiddle constants
   w[i].imag =-sin(2*PI*i/512.0); //Im component of twiddle constants
  }
 comm_intr();			    //init DSK, codec, McBSP 
 
 while(1)				    //infinite loop   
  {
   while (flag == 0) ;            //wait until iobuffer is full 
   flag = 0;                      //reset flag
   for (i = 0 ; i < PTS ; i++)    //swap buffers
    {
     samples[i].real=iobuffer[i]*okno[i]; //buffer with new data
     iobuffer[i] = x1[i];         //processed frame to iobuffer
    } 
   for (i = 0 ; i < PTS ; i++)
     samples[i].imag = 0.0;	    //imag components = 0

   FFT(samples,PTS);              //call function FFT.c

   for (i = 0 ; i < PTS ; i++)    //compute magnitude
    {
     x1[i] = sqrt(samples[i].real*samples[i].real 
	     + samples[i].imag*samples[i].imag)/16;
    }
   x1[0] = 32000.0;               //negative spike for reference
  }                               //end of infinite loop
} 					    //end of main

interrupt void c_int11()	    //ISR
 {
  output_sample((short)(iobuffer[buffercount]));     //out from iobuffer
  iobuffer[buffercount++]=(float)((short)input_sample()); //input to iobuffer
  if (buffercount >= PTS)				   //if iobuffer full
  {
	buffercount = 0;					   //reinit buffercount
	flag = 1;						   //set flag
  }
}


