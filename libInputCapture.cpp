
#include "libInputCapture.h"
#include "timer.h"

// #define DEBUG(x,y)  Serial.print(x); Serial.print(y)
// #define DEBUGLN(x,y) Serial.print(x); Serial.println(y)
// #define DEBUGLED()    digitalWrite(13,!digitalRead(13))
#define DEBUG(x,y)
#define DEBUGLN(x,y)
#define DEBUGLED()

static uint8_t idx=0;

sIClist icList[10];
/*
  Return InputCapture instance related to the HardwareTimer instance
*/
static InputCapture *getICinstance(HardwareTimer *Htim) {
  for(uint8_t i=0; i<sizeof(icList);i++)
    if(icList[i].ptrHT == Htim)
      return icList[i].ptrIC;
  return (InputCapture *)0;
}

static void InputCapture_IT_callback(HardwareTimer *Htim)
{
  InputCapture *ic=getICinstance(Htim);
  ic->CurrentCapture = Htim->getCaptureCompare(ic->channel);
  /* frequency computation */
  if (ic->CurrentCapture > ic->LastCapture) {
    ic->Period = ic->CurrentCapture - ic->LastCapture;
  }
  else if (ic->CurrentCapture <= ic->LastCapture) {
    /* 0x1000 is max overflow value */
    ic->Period = 0x10000 + ic->CurrentCapture - ic->LastCapture;
  }
  ic->Frequency = (float)ic->input_freq / (float)(ic->Period);
  ic->LastCapture = ic->CurrentCapture;
  ic->getF=true;
}

InputCapture::InputCapture(uint8_t pin) {
  TIM_TypeDef *Instance = (TIM_TypeDef *)pinmap_peripheral(digitalPinToPinName(pin), PinMap_PWM);
  channel = STM_PIN_CHANNEL(pinmap_function(digitalPinToPinName(pin), PinMap_PWM));

  // Instantiate HardwareTimer object. Thanks to 'new' instantiation, HardwareTimer is not destructed when setup() function is finished.
  MyTim = new HardwareTimer(Instance);

  // Configure rising edge detection to measure frequency
  MyTim->setMode(channel, TIMER_INPUT_CAPTURE_RISING, pin);

  // With a PrescalerFactor = 1, the minimum frequency value to measure is : TIM counter clock / CCR MAX
  //  = (SystemCoreClock) / 65535
  // Example on Nucleo_L476RG with systemClock at 80MHz, the minimum frequency is around 1,2 khz
  // To reduce minimum frequency, it is possible to increase prescaler. But ic is at a cost of precision.
  // The maximum frequency depends on processing of the interruption and thus depend on board used
  // Example on Nucleo_L476RG with systemClock at 80MHz the interruption processing is around 4,5 microseconds and thus Max frequency is around 220kHz
  uint32_t PrescalerFactor = 1;
  MyTim->setPrescaleFactor(PrescalerFactor);
  MyTim->setOverflow(0x10000); // Max Period value to have the largest possible time to detect rising edge and avoid timer rollover
  MyTim->attachInterrupt(channel, HWTIMER_ARG(InputCapture_IT_callback,MyTim));
  if(idx<10) {
    icList[idx].ptrHT = MyTim;
    icList[idx].ptrIC = this;
    idx++;
  }
};

void InputCapture::begin(){
  MyTim->resume();
  // Compute ic scale factor only once
  input_freq = MyTim->getTimerClkFreq() / MyTim->getPrescaleFactor();
}

bool InputCapture::getEvent(void){
  bool tmp=getF;
  getF=false;
  return tmp;
}

uint32_t InputCapture::getPeriod(void){
  return Period;
}

float  InputCapture::getFrequency(void){
  return Frequency;
}
