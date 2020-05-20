#include "libInputCapture.h"

/* A5 is driving by a potentiometer allowing low frequency waveform
 it remaps the period between 0 and 400 ms
 BF_GENERATOR provides the low frequency waveform
 Connect BF_GENERATOR output to your TIMx_CHx input
*/

#define SERIAL(x,y)	{Serial.print(x); Serial.println(y);}
InputCapture ic1(2);
InputCapture ic2(3);


void setup()
{
	Serial.begin(115200);
	ic1.begin();	
	ic2.begin();	
  Serial.println("Setup completed");
}

void loop()
{
  uint32_t f = map(analogRead(A0),0,1023,1500, 10000);
//  SERIAL("pot=",f);
  analogWrite(4,127);
  analogWriteFrequency(f);
  //analogWriteFrequency(3,map(analogRead(A1),0,1023,1500, 10000));
  if(ic1.getEvent())
    SERIAL("",ic1.getFrequency());
  if(ic2.getEvent())
    SERIAL("",ic2.getFrequency());
  // ic1.getCaptureEvent();
  // ic2.getCaptureEvent();

//  dbg_print_ch();
}