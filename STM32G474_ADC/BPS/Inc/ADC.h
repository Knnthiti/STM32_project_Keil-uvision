#include "Clock_system.h"
#include "stm32g4xx_ll_adc.h"
#include "stm32g4xx_ll_dma.h"

#define ADC_DMA

void ADC_Init(void);
void ADC_PIN_Init(void);
void ADC_DMA_Init(void);
