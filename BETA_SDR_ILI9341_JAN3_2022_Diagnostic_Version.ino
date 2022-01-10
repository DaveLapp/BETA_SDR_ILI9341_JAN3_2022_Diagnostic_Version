/*                       Happy New Year !!!
 *  
 *  First the usual disclaimer ..You are building at your ownn risk!!
 *  I will hold no liability if you damage a componentinvolved with this build
 *  if you are good with this ..enjoy!
 *  
 *  This code is for non-commercial use and not for resale!!!
 *  
 *  it was build as an educational platform to learn and dsevelope SDR code with and
 *  is very open ended ..
 *  
 *  All code is inn the public domain .
 *   *  
 *  
 * Big Thanks to Bob  W7PUA for a super DSP library 
 * Big Thanks to Mike K7MDL for the super spectrum waterfall display code for the RA8875 
 * Big Thanks to Giuseppe CAllipo IK8YFW for the Convolutional filter code!! 
 *  
 * THIS BUILD has a convolutional filter 
 *   
 * Bandwidth filtering is accomplished wiht variable filter and 2 mechanical encoders
 * 
 * Build environment Arduino1.8.16 teensyduino 1.55
 * 
 * Required Libraries ...
 * 
 *        ili9488_T3  https://github.com/mjs513/ILI9488_t3
 *        openaudio https://github.com/chipaudette/OpenAudio_ArduinoLibrary
 *        
 *       si5351  NT7s Library available in your arduino ide library tool..
 * 
 * Support cann be found in 2 locations 
 * 
 * FACEBOOK group Keiths_SDR ( the fun and not so formal group)
 * 
 * Keith_Sdr@groups.io ( the more formal) leave requsets for support in the messages section
 *  
 * Note the following pinouts have changed 
 * 
 * for ILI9488 TFT display portion
 * 
 * teensy pin   Display
 * 13 sck       SCK
 * 12 miso      MISO
 * 11 mosi      MOSI
 * 10 CS        CS
 *  9 DC        DC
 *  3.3v        Reset  new pinout
 *  
 *  for touch portion
 *  teensy pin  Display
 *  
 *  13 SCK      t_CLK
 *  6           T_CS   new pinout
 *  5           T_IRQ  new pinout
 *  11 MOSI     T_DIN  
 *  12 MISO     R_D0
 * 
 * Tuninng encoder 
 * teensy pins 4,3     new pinout
 * 
 * Center Frequenncy encoder
 * Teensy pins 14,15   sets the center frequency of the filter  new pinout
 * 
 * Bandwidth encoder 2,1 sets the filter bandwidth  new pinout
 *
 *
 *NOTES ......
 *
 *      1 . the Smeter needs to be calibrated divide or multiply S-level in line#146                                 
 *          of Display_ILI9341 to correct
 *                  
 *
 *          void displayLevel()
                  { 
                  tft.useFrameBuffer(0);
                  tft.fillRect(80, 35, 240, 5,ILI9341_BLACK);
                  tft.fillRect(80, 35, int(sLevel), 5,ILI9341_ORANGE);// modify this line to set slevel
                  tft.updateScreenAsync();
                  }
      2. in DSP.H the FFT spectrum display will need to be adjusted void Spectrum_ILI9341()
{
  float* pPwr;
  int bar;
  FFT.setOutputType(FFT_RMS);
 // Select the wndow function
  //FFT256iq1.windowFunction(AudioWindowNone);
  //FFT256iq1.windowFunction(AudioWindowHanning256);
  //FFT256iq1.windowFunction(AudioWindowKaiser256, 55.0f);
  FFT.windowFunction(AudioWindowBlackmanHarris256);
  FFT.setXAxis(0X03);
  FFT.setNAverage(15); 
  
 if(FFT.available())
 {
      pPwr = FFT.getData();
     for(int i=1                                                                                                            ; i<256; i++)
     {
      bar= int(*(pPwr + i)*100);        //calculates power in emission
     // bar=bar/6;                      //scaling factor
      if (bar >60) bar=60;              //sets bar height 

       tft.drawFastVLine((i*1)+32, 145-bar,bar,ILI9341_YELLOW);//modify these line
       tft.drawFastVLine((i*1)+32, 80,64-bar,ILI9341_BLACK);   //modify these lines
     }
    
 }

}                                
 *
 *
 *
 *
 */
#include "ILI9341_Display.h"
#include "Encoder_Control.h"
#include "Touch_Controller.h"
#include "VFO.h"
#include "DSP.h"
#include <Metro.h>

volatile uint32_t Freq =7076000; // default power up frequency//////////
volatile uint32_t Step =1000; // default power up frequency//////////

long oldTuning=-999;
long newTuning;
///////////////////////////////////////////////////////////
long oldFilter=-999;
long newFilter;
int xlF=240;
int filterCenter=1650;
int fcUpper=2800;
int fcLower=250;
////////////////////////////////////////////////////////////
long oldBandwidth=-999;
long newBandwidth;
int xlB=240;
int filterBandwidth=2800;
int BwUpper=4000;
int BwLower=250;
/////////////////////////////////////////////////////////////
String Mode="";
float sLevel;
long I;
long Q;
Metro touch=Metro(300);
Metro meter=Metro(250);
void setup()
{
Serial.begin(115000);

initTouch();
initTemplate();
initVfo();
initDsp();  

displayFreq();
displayMode();
displayTs();
displayFc();
displayBw();
displayLevel();
meterScale();
spectrumScale();
SetFrequency();
selectModeUSB();  
                                                           
}


void loop()
{
 
Encoder_Controller();


  if(touch.check()==1)
 {
  if(ts.touched())  
   {  
    selectTouch();
   }
 }  
 
if(meter.check()==1)
{
 getRMS();
 displayLevel();
 Serial.println("S-meter:  
 "+ String(sLevel));
}

Spectrum_ILI9341();

}
