/******************************************************************************
  * @FileName    : bsp_adccfg.h
  * @Author      : by Haribabu.P
  * @Version     : v1.0
  * @Created     : 07-Jun-2024
  * @Target      : STM32C031F6P3 - TSSOP-20
  * @ToolChain   : Keil MDK_ARM
  * @ProjectName : PMZ250Z54V1B_Secondary
  * @Description : --
  * Copyright (c) Delta Electronics (Thailand) Public Co., Ltd.
  * All rights reserved. 
  ******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BSP_ADCCFG_H 
#define __BSP_ADCCFG_H

#ifdef __cplusplus 
extern "C" { 
#endif 

/* ------------------------------ Includes ------------------------------------ */
#if defined(USE_FULL_ASSERT) 
#include "stm32_assert.h" 
#endif /* USE_FULL_ASSERT */

#include "stm32c0xx_ll_adc.h" 

/* ---------------------------- Private macro --------------------------------- */ 
#define BSP_MAX_ADC_CHANNEL    4 

/* ----------------------------- Exported macro ------------------------------- */ 
/* * Delay between ADC end of calibration and ADC enable. 
 * Delay estimation in CPU cycles: Case of ADC enable done 
 * immediately after ADC calibration, ADC clock setting slow 
 * (LL_ADC_CLOCK_ASYNC_DIV32). Use a higher delay if ratio 
 * (CPU clock / ADC clock) is above 32. 
 */ 
#define ADC_DELAY_CALIB_ENABLE_CPU_CYCLES (LL_ADC_DELAY_CALIB_ENABLE_ADC_CYCLES * 32)

/* --------------------------- Exported types --------------------------------- */

/* -------------------------- Exported constants ------------------------------ */

/* ---------------------------- External variables ---------------------------- */ 
extern __IO uint16_t u16ADCxConvertedData[BSP_MAX_ADC_CHANNEL]; 
extern volatile uint8_t u8ShortCircuitCounter; 

/* ---------------------- Exported functions prototypes ----------------------- */ 
typedef void (*fpADC_Receive_CallBack_t)(void);

void vBsp_ADC1TrigTimerDMAInit(fpADC_Receive_CallBack_t pFucHandler); 
void vBsp_ADC_Activate(void); 
void vBsp_ADCStartConversion(void);

#ifdef __cplusplus 
} 
#endif

#endif /* __BSP_ADCCFG_H */