#ifndef __InputCapture
#define __InputCapture

#include "Arduino.h"
#include "HardwareTimer.h"

#if defined(STM32_CORE_VERSION)
#	if (STM32_CORE_VERSION  < 0x01090000)
#		define HWTIMER_ARG(cb,param)	(cb)
#	else
# 	define HWTIMER_ARG(cb,param)	std::bind(cb,param)
#	endif
#else
# error "Library works with STM32 only"
#endif

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


#endif // __InputCapture
