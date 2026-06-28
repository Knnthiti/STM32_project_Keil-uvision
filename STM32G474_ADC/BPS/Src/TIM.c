#include "TIM.h"
void TIM_PWM_int(void)
{
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA); // Enable AHB2 peripherals clock.
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);  // Enable AHB1 peripherals clock.
    LL_GPIO_InitTypeDef GPIO_InitStruct = {
        .Pin = LL_GPIO_PIN_5 | LL_GPIO_PIN_2,  // Use Pin 5
        .Mode = LL_GPIO_MODE_ALTERNATE,        // Set to "Alternate Function" mode (for Timer)
        .Speed = LL_GPIO_SPEED_FREQ_HIGH,      // Set signal speed to High
        .OutputType = LL_GPIO_OUTPUT_PUSHPULL, // Use Push-Pull
        .Pull = LL_GPIO_PULL_NO,               // No internal pull-up or pull-down
        .Alternate = LL_GPIO_AF_1              // Connect Pin to Alternate Function 1 (Timer2_ch1)
    };
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct); // Initialize GPIO registers according to the specified parameters in GPIO_InitStruct.
    LL_TIM_InitTypeDef TIM_InitStruct = {
        .Prescaler = 2,                            // Prescaler
        .CounterMode = LL_TIM_COUNTERMODE_UP,      // CounterMode mode
        .Autoreload = 4095,                        // ARR
        .ClockDivision = LL_TIM_CLOCKDIVISION_DIV1 // setting used to configure the Digital Filter.
    };
    LL_TIM_Init(TIM2, &TIM_InitStruct);                       // Configure the TIMx time base unit.
    LL_TIM_EnableARRPreload(TIM2);                            // Enable auto-reload (ARR) preload. : Let the timer finish its current cycle first, then update to the new period.
    LL_TIM_SetClockSource(TIM2, LL_TIM_CLOCKSOURCE_INTERNAL); // Set the clock source of the counter clock.
    LL_TIM_OC_InitTypeDef TIM_OC_InitStruct = {
        .OCMode = LL_TIM_OCMODE_PWM1,        // active(1) as long as TIMx_CNTTIMx_CCRy
        .OCState = LL_TIM_OCSTATE_ENABLE,    // Enable output on this channel
        .OCNState = LL_TIM_OCSTATE_DISABLE,  // Disable complementary output
        .CompareValue = 2048,                // duty cycle
        .OCPolarity = LL_TIM_OCPOLARITY_HIGH // Active signal is HIGH (3.3V)
    };
    LL_TIM_OC_Init(TIM2, LL_TIM_CHANNEL_CH1, &TIM_OC_InitStruct); // Configure the TIMx output channel.
    LL_TIM_OC_Init(TIM2, LL_TIM_CHANNEL_CH3, &TIM_OC_InitStruct); // Configure the TIMx output channel.
    LL_TIM_OC_EnablePreload(TIM2, LL_TIM_CHANNEL_CH1);            // Enable compare register (TIMx_CCRx) preload for the output channel.
    LL_TIM_OC_EnableFast(TIM2, LL_TIM_CHANNEL_CH1);               // Enable fast mode for the output channel.
    LL_TIM_CC_EnableChannel(TIM2, LL_TIM_CHANNEL_CH1);            // Enable capture/compare channels_CH1.
    LL_TIM_OC_EnablePreload(TIM2, LL_TIM_CHANNEL_CH3);            // Enable compare register (TIMx_CCRx) preload for the output channel.
    LL_TIM_OC_EnableFast(TIM2, LL_TIM_CHANNEL_CH3);               // Enable fast mode for the output channel.
    LL_TIM_CC_EnableChannel(TIM2, LL_TIM_CHANNEL_CH3);            // Enable capture/compare channels_CH1.
    LL_TIM_EnableCounter(TIM2);                                   // Enable timer counter
}