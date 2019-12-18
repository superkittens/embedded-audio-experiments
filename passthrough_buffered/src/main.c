#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_timer.h"
#include "em_adc.h"
#include "em_vdac.h"
#include "arm_math.h"

#define NUM_BUFFERS 4
#define BUFFER_SIZE 256
#define QUEUE_SIZE (NUM_BUFFERS)


//  Create buffers
volatile static float32_t buffer[NUM_BUFFERS][BUFFER_SIZE];

//  Create queues
volatile static float32_t *adcQueue[QUEUE_SIZE];
volatile static float32_t *processingQueue[QUEUE_SIZE];
volatile static float32_t *dacQueue[QUEUE_SIZE];

//  Create pointers to available buffers and available slots in each respective queue
volatile static uint32_t adcQueueHead = QUEUE_SIZE - 1;
volatile static uint32_t adcQueueTail = QUEUE_SIZE - 1;
volatile static uint32_t processingQueueHead = QUEUE_SIZE - 1;
volatile static uint32_t processingQueueTail = QUEUE_SIZE - 1;
volatile static uint32_t dacQueueHead = QUEUE_SIZE - 1;
volatile static uint32_t dacQueueTail = QUEUE_SIZE - 1;


volatile uint32_t adcBufferIndex;
volatile uint32_t dacBufferIndex;


static void setupSamplingTimer(){

  //  Enable clocks to Timer
  CMU_ClockEnable(cmuClock_TIMER0, true);

  //  Configure the timer
  //  We want an up-counting timer that goes up to a number that corresponds to 25 usec
  //  For a 40 MHz clock (no prescaling), that value is 1000
  TIMER_Init_TypeDef init = TIMER_INIT_DEFAULT;

  //  Don't start the timer after initialization yet
  init.enable = false;

  TIMER_Init(TIMER0, &init);

  //  Calculate the number of pulses needed for each sampling period
  float32_t samplingPeriod = 1 / 40000.0f;
  float32_t numPulsesf =   samplingPeriod * 40000000.0f;
  uint32_t numPulses = (uint32_t)numPulsesf;

  //  Set counter limit
  TIMER_TopSet(TIMER0, numPulses);

  //  Enable overflow interrupt
  NVIC_EnableIRQ(TIMER0_IRQn);
  TIMER_IntEnable(TIMER0, TIMER_IF_OF);
}


static void setupADC(){

  //	Enable clock to ADC
  CMU_ClockEnable(cmuClock_ADC0, true);

  ADC_Init_TypeDef init = ADC_INIT_DEFAULT;

  init.timebase = ADC_TimebaseCalc(0);
  init.prescale = ADC_PrescaleCalc(10000000, 0);

  ADC_Init(ADC0, &init);

  ADC_InitSingle_TypeDef sInit = ADC_INITSINGLE_DEFAULT;

  //	Setup single channel mode parameters
  sInit.reference = adcRefVDD;
  sInit.acqTime = adcAcqTime8;			//  Take 8 ADC clock cycles to capture sample
  sInit.posSel= adcPosSelAPORT0XCH0;	//  ADC Input = Port PI0
  sInit.negSel = adcNegSelVSS;	        //  Single-ended ADC input
  sInit.rep = false;                    //  Disable repeated mode

  ADC_InitSingle(ADC0, &sInit);

  NVIC_EnableIRQ(ADC0_IRQn);
  ADC_IntEnable(ADC0, ADC_IF_SINGLE);
  ADC_IntEnable(ADC0, ADC_IF_SINGLEOF);
}


static void setupDAC(){
  //	Enable VDAC clock
  CMU_ClockEnable(cmuClock_VDAC0, true);

  VDAC_Init_TypeDef vdac_init = VDAC_INIT_DEFAULT;
  VDAC_InitChannel_TypeDef vdac_init_channel =      VDAC_INITCHANNEL_DEFAULT;

  vdac_init.prescaler = VDAC_PrescaleCalc(1000000, true, 0);
  VDAC_Init(VDAC0, &vdac_init);

  vdac_init_channel.enable = true;
  VDAC_InitChannel(VDAC0, &vdac_init_channel, 0);
}


void transferBufferToQueue(volatile float32_t *outgoingBuffer, volatile float32_t **ingoingQueue, volatile uint32_t *ingoingIndex){

  //  If the slot pointed to by ingoingIndex is NOT NULL, it means that it is already occupied by a buffer
  if (ingoingQueue[*ingoingIndex] == NULL){
    ingoingQueue[*ingoingIndex] = outgoingBuffer;

    *ingoingIndex = (*ingoingIndex + 1) % QUEUE_SIZE;
  }
}



int main(void)
{
  /* Chip errata */
  CHIP_Init();

  setupSamplingTimer();
  setupADC();
  setupDAC();

  //  Initialize buffers with all 0s
  for (int i = 0; i < NUM_BUFFERS; ++i)
	  arm_fill_f32(0.0, (float32_t *)buffer[i], BUFFER_SIZE);

  //  Initialize all slots in each queue to NULL pointers
  //  Except for the adcQueue.  That queue should contain all the empty buffers
  for (int i = 0; i < QUEUE_SIZE; ++i){
	  adcQueue[i] = buffer[i];
      adcQueue[i] = NULL;
      adcQueue[i] = NULL;
  }

  adcBufferIndex = 0;
  dacBufferIndex = 0;

  TIMER_Enable(TIMER0, true);


  /* Infinite loop */
  while (1){

	  //  Check to make sure there is a buffer available for processing
	  if (processingQueue[processingQueueHead] != NULL){

		  //  Fancy processing code here

	      transferBufferToQueue(processingQueue[processingQueueHead], dacQueue, &dacQueueTail);

	      processingQueue[processingQueueHead] = NULL;
	      processingQueueHead = (processingQueueHead + 1) % QUEUE_SIZE;
	  }
  }
}


//  Your ISR functions MUST be named as such
void TIMER0_IRQHandler(){

  //  Clear interrupt flags
  TIMER_IntClear(TIMER0, TIMER_IFC_OF);

  //  Kick off ADC sampling
  ADC_Start(ADC0, adcStartSingle);


  //  Check to make sure there is a buffer to consume
  if (dacQueue[dacQueueHead] != NULL){

	  //  Write an output sample to DAC
      VDAC_Channel0OutputSet(VDAC0, (uint32_t)dacQueue[dacQueueHead][dacBufferIndex]);

      dacBufferIndex++;

      if (dacBufferIndex >= BUFFER_SIZE){

        dacBufferIndex = 0;
        transferBufferToQueue(dacQueue[dacQueueHead], adcQueue, &adcQueueTail);

        dacQueue[dacQueueHead] = NULL;
        dacQueueHead = (dacQueueHead + 1) % QUEUE_SIZE;
      }
  }
}


//  ISR for when ADC finishes sampling
void ADC0_IRQHandler(){

  //  Check to make sure there is an empty buffer available
    if (adcQueue[adcQueueHead] != NULL){
      adcQueue[adcQueueHead][adcBufferIndex] = (float32_t)ADC0->SINGLEDATA;
      adcBufferIndex++;

      if (adcBufferIndex >= BUFFER_SIZE){
        adcBufferIndex = 0;

        transferBufferToQueue(adcQueue[adcQueueHead], processingQueue, &processingQueueTail);

        adcQueue[adcQueueHead] = NULL;
        adcQueueHead = (adcQueueHead += 1) % QUEUE_SIZE;
      }
    }
}
