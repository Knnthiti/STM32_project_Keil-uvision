#include "Timer.h"

/**
  * @brief  Initialize TIM2 for PWM output
  */
void TIM_PWM_int(void)
{
    // 1. Enable peripheral clocks
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);

    // 2. Configure GPIO (PA0)
    LL_GPIO_InitTypeDef GPIO_InitStruct = {
        .Pin        = LL_GPIO_PIN_0,
        .Mode       = LL_GPIO_MODE_ALTERNATE,
        .Speed      = LL_GPIO_SPEED_FREQ_HIGH,
        .OutputType = LL_GPIO_OUTPUT_PUSHPULL,
        .Pull       = LL_GPIO_PULL_NO,
        .Alternate  = LL_GPIO_AF_1
    };
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // 3. Configure Timer base
    LL_TIM_InitTypeDef TIM_InitStruct = {
        .Prescaler     = 2,
        .CounterMode   = LL_TIM_COUNTERMODE_CENTER_UP_DOWN,
        .Autoreload    = 4095,
        .ClockDivision = LL_TIM_CLOCKDIVISION_DIV1
    };
    LL_TIM_Init(TIM2, &TIM_InitStruct);

    LL_TIM_EnableARRPreload(TIM2);
    LL_TIM_SetClockSource(TIM2, LL_TIM_CLOCKSOURCE_INTERNAL);

    // 4. Configure Output Compare for PWM channel 1
    LL_TIM_OC_InitTypeDef TIM_OC_InitStruct = {
        .OCMode      = LL_TIM_OCMODE_PWM1,
        .OCState     = LL_TIM_OCSTATE_ENABLE,
        .OCNState    = LL_TIM_OCSTATE_DISABLE,
        .CompareValue = 0,
        .OCPolarity  = LL_TIM_OCPOLARITY_HIGH
    };
    LL_TIM_OC_Init(TIM2, LL_TIM_CHANNEL_CH1, &TIM_OC_InitStruct);

    LL_TIM_OC_EnablePreload(TIM2, LL_TIM_CHANNEL_CH1);
    LL_TIM_OC_DisableFast(TIM2, LL_TIM_CHANNEL_CH1);

    // 5. Enable channel and start counter
    LL_TIM_CC_EnableChannel(TIM2, LL_TIM_CHANNEL_CH1);
    LL_TIM_EnableCounter(TIM2);
}

/**
  * @brief  Initialize TIM3 for Input Capture
  */
void TIM_InputCapture_Init(void)
{
    // 1. Enable peripheral clock
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3);

    // 2. Configure GPIO (PA6)
    LL_GPIO_InitTypeDef GPIO_InitStruct = {
        .Pin        = LL_GPIO_PIN_6,
        .Mode       = LL_GPIO_MODE_ALTERNATE,
        .Speed      = LL_GPIO_SPEED_FREQ_HIGH,
        .Pull       = LL_GPIO_PULL_NO,
        .Alternate  = LL_GPIO_AF_2
    };
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // 3. Configure Timer base
    LL_TIM_InitTypeDef TIM_InitStruct = {
        .Prescaler     = 0,
        .CounterMode   = LL_TIM_COUNTERMODE_UP,
        .Autoreload    = 65535,
        .ClockDivision = LL_TIM_CLOCKDIVISION_DIV1
    };
    LL_TIM_Init(TIM3, &TIM_InitStruct);

    LL_TIM_EnableARRPreload(TIM3);
    LL_TIM_SetClockSource(TIM3, LL_TIM_CLOCKSOURCE_INTERNAL);

    // 4. Configure Input Capture channel 1
    LL_TIM_IC_InitTypeDef TIM_IC_InitStruct = {
        .ICPolarity    = LL_TIM_IC_POLARITY_RISING,
        .ICActiveInput = LL_TIM_ACTIVEINPUT_DIRECTTI,
        .ICPrescaler   = LL_TIM_ICPSC_DIV1,
        .ICFilter      = LL_TIM_IC_FILTER_FDIV1
    };
    LL_TIM_IC_Init(TIM3, LL_TIM_CHANNEL_CH1, &TIM_IC_InitStruct);

    // 5. Configure Interrupts
    LL_TIM_EnableIT_CC1(TIM3);
    NVIC_SetPriority(TIM3_IRQn, 0);
    NVIC_EnableIRQ(TIM3_IRQn);

    // 6. Enable channel and start counter
    LL_TIM_CC_EnableChannel(TIM3, LL_TIM_CHANNEL_CH1);
    LL_TIM_EnableCounter(TIM3);
}