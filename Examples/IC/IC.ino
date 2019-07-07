#include "libInputCapture.h"

/* A1 is driving by a potentiometer allowing low frequency waveform
 it remaps the period between 0 and 400 ms
 BF_GENERATOR provides the low frequency waveform
 Connect BF_GENERATOR output to your TIMx_CHx input
*/

#define BF_GENERATOR	3

InputCapture ic;

void setup()
{
	Serial.begin(115200);
	ic.begin(TIM5_CH1, TIM_INPUTCHANNELPOLARITY_RISING,0);	// set TIM5 CH1
	pinMode(BF_GENERATOR,OUTPUT);

}

void loop()
{
  digitalWrite(BF_GENERATOR, HIGH);
  delay(map(analogRead(A1),0,1023,0,200));
  digitalWrite(BF_GENERATOR, LOW);
  delay(map(analogRead(A1),0,1023,0,200));
  //analogWrite(2,map(analogRead(A1),0,1023,0,255));
  if(ic.getCaptureEvent())
    Serial.println(ic.getFrequency());
}