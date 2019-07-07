#ifndef __InputCapture
#define __InputCapture

#include "Arduino.h"

#define TIM5_CH1	0

typedef struct {
	uint32_t Period;
	uint32_t Previous_value;
	float Frequency;
	float FREQ_TIM;
	bool getF=false;
} sICdata;

typedef struct struct_ic {
	TIM_TypeDef			*tim;
	uint32_t			timer_resolution;
	uint16_t			tim_channel;
	PortName 			port;	// cf PortNames.h (index of port)
	GPIO_InitTypeDef    io;
	void (*irqhandler)(void);
} sInputCapture;

#ifdef __cplusplus
extern "C" {
#endif

class InputCapture{

public:
	InputCapture(void);

	void begin(uint8_t idx, uint32_t edge, uint32_t prescaler);
	void HAL_IC_TIMx_Base_MspInit(sInputCapture *ic);

	// specify a delay so that period will be reset 
	uint32_t setTimeout(uint32_t delay);
	// set to 1 when an event occurs
	bool getCaptureEvent(void);
	uint32_t getCapturePeriod(void);
	float	 getFrequency(void);
	uint32_t getCaptureAbsoluteValue(void);
	uint32_t getCapturePinNumber(void);	// expressed in Arduino space

private:
	TIM_HandleTypeDef htim;
	uint8_t idx;
};

#ifdef __cplusplus
}
#endif

#endif // __InputCapture
