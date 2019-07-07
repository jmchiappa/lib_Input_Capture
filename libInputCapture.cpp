
#include "libInputCapture.h"
#include "timer.h"

// #define DEBUG(x,y)  Serial.print(x); Serial.print(y)
// #define DEBUGLN(x,y) Serial.print(x); Serial.println(y)
// #define DEBUGLED()    digitalWrite(13,!digitalRead(13))
#define DEBUG(x,y)
#define DEBUGLN(x,y)
#define DEBUGLED()

uint16_t dbg_acch;
uint16_t dbg_bb;
uint16_t dbg_ch;
uint8_t  dbg_idx;

sInputCapture ICMap[] = {
  {
    .tim = TIM5, // TIM5 CH1
    .timer_resolution = 0xffffffff,
    .tim_channel = TIM_CHANNEL_1,
    .port = PortA,
    .io = { // TIM5 CH1 IO
      .Pin = GPIO_PIN_0,
      .Mode = GPIO_MODE_AF_PP,
      .Pull = GPIO_NOPULL,
      .Speed = GPIO_SPEED_FREQ_LOW,
      .Alternate = GPIO_AF2_TIM5},    
    .irqhandler = 0x0L
  },
  {
    .tim = TIM5, // TIM5 CH2
    .timer_resolution = 0xffffffff,
    .tim_channel = TIM_CHANNEL_2,
    .port = PortA,
    .io = { // TIM5 CH1 IO
      .Pin = GPIO_PIN_1,
      .Mode = GPIO_MODE_AF_PP,
      .Pull = GPIO_NOPULL,
      .Speed = GPIO_SPEED_FREQ_LOW,
      .Alternate = GPIO_AF2_TIM5},    
    .irqhandler = 0x0L
  },
  {
    .tim = TIM8, // TIM8 CH1
    .timer_resolution = 0xffffffff,
    .tim_channel = TIM_CHANNEL_1,
    .port = PortC,
    .io = { // TIM5 CH1 IO
      .Pin = GPIO_PIN_6,
      .Mode = GPIO_MODE_AF_PP,
      .Pull = GPIO_NOPULL,
      .Speed = GPIO_SPEED_FREQ_LOW,
      .Alternate = GPIO_AF3_TIM8},    
    .irqhandler = 0x0L
  },
  {
    .tim = TIM8, // TIM8 CH2
    .timer_resolution = 0xffffffff,
    .tim_channel = TIM_CHANNEL_2,
    .port = PortC,
    .io = { // TIM5 CH1 IO
      .Pin = GPIO_PIN_7,
      .Mode = GPIO_MODE_AF_PP,
      .Pull = GPIO_NOPULL,
      .Speed = GPIO_SPEED_FREQ_LOW,
      .Alternate = GPIO_AF3_TIM8},    
    .irqhandler = 0x0L
  },
  {
    .tim = TIM8, // TIM8 CH3
    .timer_resolution = 0xffffffff,
    .tim_channel = TIM_CHANNEL_3,
    .port = PortC,
    .io = { // TIM5 CH1 IO
      .Pin = GPIO_PIN_8,
      .Mode = GPIO_MODE_AF_PP,
      .Pull = GPIO_NOPULL,
      .Speed = GPIO_SPEED_FREQ_LOW,
      .Alternate = GPIO_AF3_TIM8},    
    .irqhandler = 0x0L
  },
  {
    .tim = TIM8, // TIM8 CH4
    .timer_resolution = 0xffffffff,
    .tim_channel = TIM_CHANNEL_4,
    .port = PortC,
    .io = { // TIM5 CH1 IO
      .Pin = GPIO_PIN_9,
      .Mode = GPIO_MODE_AF_PP,
      .Pull = GPIO_NOPULL,
      .Speed = GPIO_SPEED_FREQ_LOW,
      .Alternate = GPIO_AF3_TIM8},    
    .irqhandler = 0x0L
  }
};

/* Cross reference table TIM Active Channel to TIM Channel
  Input : Active Channel assigned in timer irq handler
  Output : TIM channel
*/
const uint16_t ActiveChannelMap[]{
  TIM_CHANNEL_1  ,
  TIM_CHANNEL_2  ,
  TIM_CHANNEL_3  ,
  TIM_CHANNEL_4  ,
  TIM_CHANNEL_5  ,
  TIM_CHANNEL_6  ,
  TIM_CHANNEL_ALL
};

/* shared variables between IC objects and irq handler */
sICdata ICdata[10];

/* reach the first bit set to 1
  Input : uint32_t number
  Output : bit position between 0 to 31
*/
static uint8_t bitbang(uint32_t v) {
  uint8_t i;
  for( i=0;(i<32) && (!(v & (1<<i)));i++);
    return i;
}



/* Find the index in Input Captture Map
  Input : tim : TIM_TypeDef pointer
          channel : channel number TIM_CHANNEL_x
  Ouput : index > 0 : find in the table
          -1 : don't find any occurence
*/
static int8_t getIndex(TIM_TypeDef *tim,uint16_t channel)
{
  for(uint8_t i=0; i<sizeof(ICMap);i++)
    if((ICMap[i].tim == tim) && (ICMap[i].tim_channel == channel))
      return i;

  return -1;
}



void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
  uint16_t ch =ActiveChannelMap[bitbang(htim->Channel)];
  // dbg_acch = htim->Channel;
  // dbg_bb = bitbang(htim->Channel);
  // dbg_ch = ActiveChannelMap[bitbang(htim->Channel)];;
  uint8_t idx = getIndex(htim->Instance,ch);
  // dbg_idx = idx;  
  uint32_t t = HAL_TIM_ReadCapturedValue(htim, ch);
  if(t>ICdata[idx].Previous_value)
    ICdata[idx].Period = t-ICdata[idx].Previous_value;
  else
    ICdata[idx].Period = (ICMap[idx].timer_resolution-ICdata[idx].Previous_value)+t +1;
  ICdata[idx].Previous_value=t;
  ICdata[idx].Frequency = ICdata[idx].FREQ_TIM/(float)ICdata[idx].Period;
  //Frequency=Period;
  digitalWrite(13,dbg_bb);
  ICdata[idx].getF=true;
}

InputCapture::InputCapture(void) {};

void InputCapture::begin(uint8_t idx,uint32_t edge, uint32_t prescaler){

  this->idx = idx;
  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  htim.Instance = ICMap[idx].tim; // get predefined timer configuration
  htim.Init.Prescaler = 0;
  htim.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim.Init.Period = ICMap[idx].timer_resolution;
  htim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

  // htim.Base_MspInitCallback = HAL_IC_TIM5_Base_MspInit;
  // htim.Base_MspDeInitCallback = HAL_IC_TIM5_Base_MspDeInit;

  if (HAL_TIM_Base_Init(&htim) != HAL_OK)
  {
    Error_Handler();
  }
  // call specific io init
  HAL_IC_TIMx_Base_MspInit(&ICMap[idx]);

  if (HAL_TIM_IC_Init(&htim) != HAL_OK)
  {
    Error_Handler();
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = edge;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim, &sConfigIC, ICMap[idx].tim_channel) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_Start_IT(&htim,ICMap[idx].tim_channel) != HAL_OK)
  {
    Error_Handler();
  }

  // Store the timer clk
  ICdata[idx].FREQ_TIM = getTimerClkFreq(ICMap[idx].tim);
  ICdata[idx].Period = 0;
  ICdata[idx].Previous_value = 0;
  ICdata[idx].Frequency = 0;
  ICdata[idx].getF = false;
  //pinMode(LED_BUILTIN,OUTPUT);
  DEBUGLN("InputCapture::begin:fin de l'init","");
}

// void TIM5_IRQHandler(void)
// {
//   HAL_TIM_IRQHandler(&htim);
// }


void InputCapture::HAL_IC_TIMx_Base_MspInit(sInputCapture *ic)
{

  /* clock enable */
  timer_enable_clock(&htim);
  /* IO Peripheral clock enable */
  set_GPIO_Port_Clock(ic->port);
  HAL_GPIO_Init(GPIOPort[ic->port], &(ic->io));

  /* TIM interrupt Init */
  HAL_NVIC_SetPriority((IRQn_Type)getTimerIrq(ic->tim), 0, 0);
  HAL_NVIC_EnableIRQ((IRQn_Type)getTimerIrq(ic->tim));
  DEBUGLN("InputCapture::begin:fin de MspInit","");
}

bool InputCapture::getCaptureEvent(void){
  DEBUG("-idx=",idx);
//  DEBUG("-Previous_value=",ICdata[idx].Previous_value);
  DEBUG("-Frequency=",ICdata[idx].Frequency);
  bool tmp=ICdata[idx].getF;
  ICdata[idx].getF=false;
  return tmp;
}

uint32_t InputCapture::getCapturePeriod(void){
  return ICdata[idx].Period;
}

float  InputCapture::getFrequency(void){
  return ICdata[idx].Frequency;
}

uint32_t InputCapture::getCapturePinNumber(void){
  return 0;
}
void dbg_print_ch() {
  DEBUG("-acch=",dbg_acch);
  DEBUG("-bb=",dbg_bb);
  DEBUG("-ch=",dbg_ch);
  DEBUGLN("-idx=",dbg_idx);
}