#ifndef __InputCapture
#define __InputCapture

#include "Arduino.h"
#include "HardwareTimer.h"

#ifdef __cplusplus
extern "C" {
#endif
class InputCapture {

public: 
	InputCapture(uint8_t pin);

	void begin(void);
	bool getEvent(void);
	uint32_t getPeriod(void);
	float	 getFrequency(void);
	uint32_t channel;
	volatile uint32_t LastCapture = 0, CurrentCapture;
	float input_freq = 0;
	uint32_t Period;
	float Frequency;
	float FREQ_TIM;
	bool getF=false;
	HardwareTimer *MyTim;

};

#ifdef __cplusplus
}
#endif

typedef struct {
	InputCapture *ptrIC;
	HardwareTimer *ptrHT;
} sIClist;

static void InputCapture_IT_callback(HardwareTimer *Htim);
static InputCapture *getICinstance(HardwareTimer *Htim);

#endif // __InputCapture
