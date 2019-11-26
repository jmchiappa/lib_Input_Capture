#ifndef __InputCapture
#define __InputCapture

#include "Arduino.h"
#include "HardwareTimer.h"

#define TIM5_CH1	0
#define TIM5_CH2	1
#define TIM8_CH1	2
#define TIM8_CH2	3
#define TIM8_CH3	4
#define TIM8_CH4	5


/*typedef struct struct_ic {
	TIM_TypeDef			*tim;
	uint32_t			timer_resolution;
	uint16_t			tim_channel;
	PortName 			port;	// cf PortNames.h (index of port)
	GPIO_InitTypeDef    io;
	void (*irqhandler)(void);
} sInputCapture;
*/


#ifdef __cplusplus
extern "C" {
#endif

void dbg_print_ch(void);

class InputCapture {

public: 
	InputCapture(uint8_t pin);

	void begin(void);
//	void HAL_IC_TIMx_Base_MspInit(sInputCapture *ic);
	// specify a delay so that period will be reset 
//	uint32_t setTimeout(uint32_t delay);
	// set to 1 when an event occurs
	bool getEvent(void);
	uint32_t getPeriod(void);
	float	 getFrequency(void);
//	uint32_t getCaptureAbsoluteValue(void);

//private:
	uint32_t channel;
	volatile uint32_t LastCapture = 0, CurrentCapture;
	float input_freq = 0;

	uint32_t Period;
//	uint32_t Previous_value;
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
