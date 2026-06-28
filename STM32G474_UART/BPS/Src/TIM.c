#include "TIM.h"

// ====================================================================== 
// Function: TIM2_Init 
// Purpose: Generate PWM on PB3 (Channel 2) 
// Send Trigger Output (TRGO) from Channel 2 
// ====================================================================== 
void TIM2_Init(void) 
{ 
    // 1. Turn on clocks 
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB); // Power up Port B 
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);  // Power up Timer 2

    // 2. Set up the pin (GPIO)
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = LL_GPIO_PIN_3;                  // Use PB3 for TIM2_CH2
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;        // Let Timer control the pin
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;      // Fast signal speed
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL; // Standard output
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;               // No internal resistor
    GPIO_InitStruct.Alternate = LL_GPIO_AF_1;             // Connect PB3 to Timer 2 (AF1)
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // 3. Set timer speed and counting limit
    LL_TIM_InitTypeDef TIM_InitStruct = {0};
    TIM_InitStruct.Prescaler = 31;                         // Divide clock by 32
    TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;   // Count from 0 up to top
    TIM_InitStruct.Autoreload = 4095;                     // Top limit (Period)
    TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1; 
    LL_TIM_Init(TIM2, &TIM_InitStruct);

    LL_TIM_EnableARRPreload(TIM2);                        // Allow smooth period updates
    LL_TIM_SetClockSource(TIM2, LL_TIM_CLOCKSOURCE_INTERNAL); // Use internal clock

    // 4. Set PWM signal properties
    LL_TIM_OC_InitTypeDef TIM_OC_InitStruct = {0};
    TIM_OC_InitStruct.OCMode = LL_TIM_OCMODE_PWM1;         // Standard PWM mode
    TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_ENABLE;     // Turn on the output
    TIM_OC_InitStruct.OCNState = LL_TIM_OCSTATE_DISABLE;   // No negative output
    TIM_OC_InitStruct.CompareValue = 2048;                 // Pulse width (50% Duty Cycle)
    TIM_OC_InitStruct.OCPolarity = LL_TIM_OCPOLARITY_HIGH;   // Signal is HIGH when active

    // Apply PWM settings to Channel 2
    LL_TIM_OC_Init(TIM2, LL_TIM_CHANNEL_CH2, &TIM_OC_InitStruct);
    LL_TIM_OC_EnablePreload(TIM2, LL_TIM_CHANNEL_CH2);
    LL_TIM_OC_DisableFast(TIM2, LL_TIM_CHANNEL_CH2);

    // 5. Set up the trigger signal (TRGO)
    LL_TIM_SetTriggerOutput(TIM2, LL_TIM_TRGO_OC2REF);     // Send signal when CH2 fires
    LL_TIM_DisableMasterSlaveMode(TIM2);                   // Standard mode

    // 6. Turn on the channel
    LL_TIM_CC_EnableChannel(TIM2, LL_TIM_CHANNEL_CH2);

    // 7. Start the timer
    LL_TIM_EnableCounter(TIM2);
}

// ====================================================================== 
// Function: TIM3_Init 
// Purpose: Generate PWM on PB1 (Channel 4) 
// Send Trigger Output (TRGO) from Channel 4 
// ====================================================================== 
void TIM3_Init(void) 
{ 
    // 1. Turn on clocks 
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB); // Power up Port B 
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3);  // Power up Timer 3

    // 2. Set up the pin (GPIO)
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = LL_GPIO_PIN_1;                  // Use PB1 for TIM3_CH4
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;        // Let Timer control the pin
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;      // Fast signal speed
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL; // Standard output
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;               // No internal resistor
    GPIO_InitStruct.Alternate = LL_GPIO_AF_2;             // Connect PB1 to Timer 3 (AF2)
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // 3. Set timer speed and counting limit
    LL_TIM_InitTypeDef TIM_InitStruct = {0};
    TIM_InitStruct.Prescaler = 2;                          // Divide clock by 3
    TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;   // Count from 0 up to top
    TIM_InitStruct.Autoreload = 4095;                     // Top limit (Period)
    TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1; 
    LL_TIM_Init(TIM3, &TIM_InitStruct);

    LL_TIM_EnableARRPreload(TIM3);                        // Allow smooth period updates
    LL_TIM_SetClockSource(TIM3, LL_TIM_CLOCKSOURCE_INTERNAL); // Use internal clock

    // 4. Set PWM signal properties
    LL_TIM_OC_InitTypeDef TIM_OC_InitStruct = {0};
    TIM_OC_InitStruct.OCMode = LL_TIM_OCMODE_PWM1;         // Standard PWM mode
    TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_ENABLE;     // Turn on the output
    TIM_OC_InitStruct.OCNState = LL_TIM_OCSTATE_DISABLE;   // No negative output
    TIM_OC_InitStruct.CompareValue = 2048;                 // Pulse width (50% Duty Cycle)
    TIM_OC_InitStruct.OCPolarity = LL_TIM_OCPOLARITY_HIGH;   // Signal is HIGH when active

    // Apply PWM settings to Channel 4
    LL_TIM_OC_Init(TIM3, LL_TIM_CHANNEL_CH4, &TIM_OC_InitStruct);
    LL_TIM_OC_EnablePreload(TIM3, LL_TIM_CHANNEL_CH4);
    LL_TIM_OC_DisableFast(TIM3, LL_TIM_CHANNEL_CH4);

    // 5. Turn on the channel
    LL_TIM_CC_EnableChannel(TIM3, LL_TIM_CHANNEL_CH4);

    // 6. Start the timer
    LL_TIM_EnableCounter(TIM3);
}