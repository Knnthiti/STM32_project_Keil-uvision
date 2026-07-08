#include "main.h"

// Define timer frequency and Auto-Reload value
#define TIMER_CLOCK_FREQ 170000000
#define TIM3_ARR_VALUE   65535

// Global variables for frequency measurement
volatile uint32_t time_diff    = 0;
volatile uint32_t last_time    = 0;
volatile uint32_t current_time = 0;
volatile uint32_t frequency     = 0;

/**
  * @brief  Interrupt handler for TIM3 input capture
  */
void TIM3_IRQHandler(void)
{
    if (LL_TIM_IsActiveFlag_CC1(TIM3))
    {
        LL_TIM_ClearFlag_CC1(TIM3); // Clear the capture/compare 1 interrupt flag

        current_time = LL_TIM_IC_GetCaptureCH1(TIM3); // Read captured value

        if (current_time >= last_time)
        {
            time_diff = current_time - last_time;
        }

        if (time_diff > 0)
        {
            frequency = TIMER_CLOCK_FREQ / time_diff;
        }

        last_time = current_time;
    }
}

/**
  * @brief  Main program
  */
int main(void)
{
    // 1. Initialize system
    Clock_system();
    TIM_PWM_int();
    TIM_InputCapture_Init();
    Button_PC13_Init();

    // 2. Infinite loop
    while (1)
    {
        // Set PWM duty cycle
        LL_TIM_OC_SetCompareCH1(TIM2, 2048);

        // Check user button to change PWM frequency
        if (LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_13) == 1)
        {
            LL_TIM_SetAutoReload(TIM2, 8192);
        }
        else
        {
            LL_TIM_SetAutoReload(TIM2, 4095);
        }
    }
}