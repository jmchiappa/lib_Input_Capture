#include "libInputCapture.h"

/* A5 is driving by a potentiometer allowing low frequency waveform
 it remaps the period between 0 and 400 ms
 BF_GENERATOR provides the low frequency waveform
 Connect BF_GENERATOR output to your TIMx_CHx input
*/

#define BF_GENERATOR	3
#define SERIAL(x,y)	{Serial.print(x); Serial.println(y);}
InputCapture ic1;
InputCapture ic2;


void setup()
{
	Serial.begin(115200);
	ic1.begin(TIM5_CH1, TIM_INPUTCHANNELPOLARITY_RISING,0);	// set TIM5 CH1
	ic2.begin(TIM5_CH2, TIM_INPUTCHANNELPOLARITY_RISING,0);	// set TIM5 CH1
	pinMode(BF_GENERATOR,OUTPUT);

}

void loop()
{
  digitalWrite(BF_GENERATOR, HIGH);
  delay(map(analogRead(A5),0,1023,0,200));
  digitalWrite(BF_GENERATOR, LOW);
  delay(map(analogRead(A5),0,1023,0,200));
  //analogWrite(2,map(analogRead(A1),0,1023,0,255));
  if(ic1.getCaptureEvent())
    SERIAL("Fic1=",ic1.getFrequency());
  if(ic2.getCaptureEvent())
    SERIAL("Fic2=",ic2.getFrequency());
  // ic1.getCaptureEvent();
  // ic2.getCaptureEvent();

//  dbg_print_ch();
}