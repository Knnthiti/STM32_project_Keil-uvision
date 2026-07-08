#include "main.h"

// --- Global Variables --- 
volatile uint16_t ADC2_Buffer[2]; // Buffer for ADC2 regular channels (DMA) 
volatile uint16_t ADC5_Buffer;    // Buffer for ADC5 injected channel (Interrupt) 
volatile float V0, V1, V2, V3;    // Variables to store calculated voltages

/**
  * @brief  This function handles ADC1 and ADC2 global interrupts 
  */ 
void ADC1_2_IRQHandler(void) 
{ 
    // Check if Analog Watchdog 1 flag is active 
    if (LL_ADC_IsActiveFlag_AWD1(ADC2) != 0) 
    { 
        LL_ADC_ClearFlag_AWD1(ADC2); // Clear Watchdog flag 
    } 
}

/**
  * @brief  This function handles ADC5 global interrupt 
  */ 
void ADC5_IRQHandler(void) 
{ 
    // Check if Injected End of Sequence (JEOS) flag is active 
    if (LL_ADC_IsActiveFlag_JEOS(ADC5) != 0) 
    { 
        LL_ADC_ClearFlag_JEOS(ADC5); // Clear the sequence complete flag
    } 
}

/**
  * @brief  Main program function 
  */ 
int main(void) 
{ 
    // 1. Initialize system clock 
    Clock_system();

    // 2. Initialize Timers for PWM and hardware triggers 
    TIM2_Init(); 
    TIM3_Init();

    // 3. Initialize ADC2 (Regular Group) 
    ADC2_Init();

    // 4. Start DMA and Arm the ADC2 regular conversion 
    ADC2_DMA_Start((uint16_t *)ADC2_Buffer, 2); 
    LL_ADC_REG_StartConversion(ADC2); // Wait for Timer hardware trigger

    // 5. Initialize ADC5 (Injected Group) 
    ADC5_Init(); 
    LL_ADC_INJ_StartConversion(ADC5); // Wait for Timer hardware trigger

    // --- Infinite Loop --- 
    while (1) 
    { 
        // Continuously update regular voltages from the DMA buffer 
        V0 = (float)(ADC2_Buffer[0] * 3.3f / 4095.0f); 
        V1 = (float)(ADC2_Buffer[1] * 3.3f / 4095.0f); 
    } 
}