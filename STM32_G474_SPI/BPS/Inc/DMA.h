#include "stm32g4xx_ll_adc.h"
#include "stm32g4xx_ll_bus.h"
#include "stm32g4xx_ll_dma.h"
#include "stm32g4xx_ll_dmamux.h"


void ADC2_DMA_Init(void);
void ADC2_DMA_Start(uint16_t *buffer, uint32_t len);
void TIM2_DMA_Init(void);
void TIM2_DMA_Start(uint32_t *buffer, uint32_t len);