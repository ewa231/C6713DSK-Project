
#include "tonecfg.h"
#include "dsk6713.h"
#include "dsk6713_aic23.h"

/* Length of sine wave table */
#define SINE_TABLE_SIZE   96

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

/* Pre-generated sine wave data, 16-bit signed samples */
Int16 sinetable[SINE_TABLE_SIZE] = {
-6025,-1760,-89,393,2020,1146,659,-1736,-6382,-8815,-10980,-12692,-11607,-12194,-10697,-6929,-5653,-3914,-472,227,1911,1317,-244,-1613,-5307,-8585,-11596,-11598,-12947,-10838,-9808,-7129,-6891,-3611,-1027,1081,3000,1132,-18,-2992,-3986,-9236,-10692,-12967,-11814,-11622,-10949,-7155,-6011,-1997,-323,1997,1777,955,283,-2358,-5919,-7801,-11068,-11281,-12665,-11963,-8923,-8923,-6200,-3337,-769,1409,1327,2340,105,-1999,-5887,-8135,-9402,-11707,-12613,-10896,-9340,-7608,-4434,-3701,-1095,795,1590,2059,135,-1884,-5524,-8812,-10762,-12576,-12740,-11392,-8920,-7440

  };



void main()
{
    DSK6713_AIC23_CodecHandle hCodec;
    Int16 msec, sample;
    
    /* Initialize the board support library, must be called first */
    DSK6713_init();
     
    /* Start the codec */
    hCodec = DSK6713_AIC23_openCodec(0, &config);
    	
    while(1)
    {
        for (sample = 0; sample < SINE_TABLE_SIZE; sample++)
        {
            while (!DSK6713_AIC23_write(hCodec, sinetable[sample]));

            while (!DSK6713_AIC23_write(hCodec, sinetable[sample]));
        }
    }









    /* Close the codec */
    DSK6713_AIC23_closeCodec(hCodec);
}
