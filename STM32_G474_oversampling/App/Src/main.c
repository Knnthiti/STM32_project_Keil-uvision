#include "main.h"

// --- Global Variables --- 
volatile uint16_t ADC2_Buffer[2]; // Buffer for ADC2 regular channels (DMA) 
volatile uint16_t ADC5_Buffer;    // Buffer for ADC5 injected channel (Interrupt) 
volatile float V0, V1, V2, V3;    // Variables to store calculated voltages

/**
@brief Configure and start DMA1 Channel 1 for ADC2
@param buffer: Pointer to the destination array
@param len: Number of data items to transfer
@retval None 
*/ 
void ADC2_DMA_Start(uint16_t *buffer, uint32_t len) 
{ 
    // 1. Disable DMA channel before configuration 
    LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_1); 
    while (LL_DMA_IsEnabledChannel(DMA1, LL_DMA_CHANNEL_1)) 
    { 
        // Wait until DMA channel is fully disabled 
    }

    // 2. Set peripheral source address and memory destination address 
    LL_DMA_SetPeriphAddress(DMA1, LL_DMA_CHANNEL_1, LL_ADC_DMA_GetRegAddr(ADC2, LL_ADC_DMA_REG_REGULAR_DATA)); 
    LL_DMA_SetMemoryAddress(DMA1, LL_DMA_CHANNEL_1, (uint32_t)buffer);

    // 3. Set the total data length to transfer 
    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_1, len);

    // 4. Enable DMA Channel to start hardware transfer 
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_1); 
}

/**
@brief This function handles ADC1 and ADC2 global interrupts 
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
@brief This function handles ADC5 global interrupt 
*/ 
void ADC5_IRQHandler(void) 
{ 
    // Check if Injected End of Sequence (JEOS) flag is active 
    if (LL_ADC_IsActiveFlag_JEOS(ADC5) != 0) 
    { 
        LL_ADC_ClearFlag_JEOS(ADC5); // Clear the sequence complete flag
        
        // Read injected conversion data (Optional: added for functional completeness)
        ADC5_Buffer = LL_ADC_INJ_ReadConversionData32(ADC5, LL_ADC_INJ_RANK_1);
				
				// Continuously update injected voltage from the interrupt buffer
        V2 = (float)(ADC5_Buffer- 2048.0f) * (3.3f / 2048.0f);
    } 
}

/**
@brief Main program function 
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
        V0 = (float)(ADC2_Buffer[0]- 2048.0f) * (3.3f / 2048.0f);
        V1 = (float)(ADC2_Buffer[1]- 2048.0f) * (3.3f / 2048.0f);
    } 
}