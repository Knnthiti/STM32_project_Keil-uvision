#include "Clock_system.h"

void Clock_system()
{
    // Enable STM32 ART Accelerator (Adaptive Real-Time Memory Accelerator)
    LL_FLASH_EnablePrefetch();  // 1. Get ready
    LL_FLASH_EnableInstCache(); // 2. Remember code
    LL_FLASH_EnableDataCache(); // 3. Keep data clos

    LL_RCC_HSE_Enable();

    // Wait HSE oscillator is stable and ready to use
    while (!LL_RCC_HSE_IsReady())
    {
    }

    // PLL Configurations (Target: 170 MHz SYSCLK from 24 MHz HSE)
    LL_UTILS_PLLInitTypeDef PLLInitStruct = {
        .PLLM = LL_RCC_PLLM_DIV_6, // HSE / 6 = 0.4 MHz
        .PLLN = 85,                // 24 MHz * 85 = 340 MHz (VCO output)
        .PLLR = LL_RCC_PLLR_DIV_2  // 340 MHz / 2 = 170 MHz (SYSCLK)
    };

    // Bus Clock Prescalers Configurations
    LL_UTILS_ClkInitTypeDef ClkInitStruct = {
        .AHBCLKDivider = LL_RCC_SYSCLK_DIV_1,
        .APB1CLKDivider = LL_RCC_APB1_DIV_2, // 170 / 2 = 85 MHz
        .APB2CLKDivider = LL_RCC_APB2_DIV_4  // 170 / 4 = 42 MHz
    };

    // Configure System Clock using
    if (LL_PLL_ConfigSystemClock_HSE(24000000, LL_UTILS_HSEBYPASS_OFF,
                                     &PLLInitStruct, &ClkInitStruct) != SUCCESS)
    {
        // Initialization Error
        while (1)
        {
        }
    }
}