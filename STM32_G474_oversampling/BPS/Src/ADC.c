#include "ADC.h"

/******************************************************************************
  * @brief Configure GPIO Pins for ADC2 Input
  ******************************************************************************/
void ADC2_PIN_Init(void) 
{ 
    // 1. Enable GPIO port A clock
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);

    // 2. Configure pins to Analog mode
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    // Setup Pins: PA0 (ADC12_IN1), PA1 (ADC12_IN2), PA6 (ADC2_IN3), PA7 (ADC2_IN4)
    GPIO_InitStruct.Pin = LL_GPIO_PIN_0 | LL_GPIO_PIN_1 | LL_GPIO_PIN_6 | LL_GPIO_PIN_7;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct); 
}

/******************************************************************************
  * @brief Initialize and configure ADC2 peripheral
  ******************************************************************************/
void ADC2_Init(void) 
{ 
    // 1. Configure clock source and enable hardware clock
    LL_RCC_SetADCClockSource(LL_RCC_ADC12_CLKSOURCE_SYSCLK); // Use SYSCLK for ADC
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_ADC12);    // Enable ADC hardware clock

    // 2. Setup GPIO pins and DMA transfer
    ADC2_PIN_Init();
    ADC2_DMA_Init();

    // 3. Setup ADC Common clock and multi-mode configurations
    LL_ADC_CommonInitTypeDef ADC_CommonInitStruct = {
        .CommonClock = LL_ADC_CLOCK_SYNC_PCLK_DIV4,
        .Multimode = LL_ADC_MULTI_INDEPENDENT,
        .MultiDMATransfer = LL_ADC_MULTI_REG_DMA_EACH_ADC,
        .MultiTwoSamplingDelay = LL_ADC_MULTI_TWOSMP_DELAY_1CYCLE
    };
    LL_ADC_CommonInit(__LL_ADC_COMMON_INSTANCE(ADC2), &ADC_CommonInitStruct);

    // 4. Power up the ADC internal voltage regulator
    LL_ADC_DisableDeepPowerDown(ADC2);
    LL_ADC_EnableInternalRegulator(ADC2);
    for (volatile uint32_t wait_loop = 0; wait_loop < 50000; wait_loop++) {}

    // 5. Configure basic ADC configurations
    LL_ADC_InitTypeDef ADC_InitStruct = {
        .Resolution = LL_ADC_RESOLUTION_12B,
        .DataAlignment = LL_ADC_DATA_ALIGN_RIGHT,
        .LowPowerMode = LL_ADC_LP_MODE_NONE
    };
    LL_ADC_Init(ADC2, &ADC_InitStruct);

    // 6. Setup Regular group queue and hardware trigger source
    LL_ADC_REG_InitTypeDef ADC_REG_InitStruct = {
        .TriggerSource = LL_ADC_REG_TRIG_EXT_TIM3_CH4,
        .SequencerLength = LL_ADC_REG_SEQ_SCAN_ENABLE_2RANKS,
        .SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_DISABLE,
        .ContinuousMode = LL_ADC_REG_CONV_CONTINUOUS,
        .DMATransfer = LL_ADC_REG_DMA_TRANSFER_UNLIMITED,
        .Overrun = LL_ADC_REG_OVR_DATA_OVERWRITTEN
    };
    LL_ADC_REG_Init(ADC2, &ADC_REG_InitStruct);

    // 7. Additional hardware configurations for ADC2
    LL_ADC_SetGainCompensation(ADC2, 0);
    LL_ADC_REG_SetDMATransfer(ADC2, LL_ADC_REG_DMA_TRANSFER_UNLIMITED);
    LL_ADC_REG_SetTriggerEdge(ADC2, LL_ADC_REG_TRIG_EXT_RISING);

    // 8. Assign channels to regular sequencer ranks
    LL_ADC_REG_SetSequencerRanks(ADC2, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_1);
    LL_ADC_SetChannelSamplingTime(ADC2, LL_ADC_CHANNEL_1, LL_ADC_SAMPLINGTIME_6CYCLES_5);
    LL_ADC_SetChannelSingleDiff(ADC2, LL_ADC_CHANNEL_1, LL_ADC_DIFFERENTIAL_ENDED);

    LL_ADC_REG_SetSequencerRanks(ADC2, LL_ADC_REG_RANK_2, LL_ADC_CHANNEL_3);
    LL_ADC_SetChannelSamplingTime(ADC2, LL_ADC_CHANNEL_3, LL_ADC_SAMPLINGTIME_6CYCLES_5);
    LL_ADC_SetChannelSingleDiff(ADC2, LL_ADC_CHANNEL_3, LL_ADC_DIFFERENTIAL_ENDED);

    // 9. Configure Regular group hardware Oversampling
    LL_ADC_ConfigOverSamplingRatioShift(ADC2, LL_ADC_OVS_RATIO_4, LL_ADC_OVS_SHIFT_RIGHT_2);
    LL_ADC_SetOverSamplingScope(ADC2, LL_ADC_OVS_GRP_REGULAR_CONTINUED);
    LL_ADC_SetOverSamplingDiscont(ADC2, LL_ADC_OVS_REG_CONT);

    // 10. Run ADC calibration
    LL_ADC_StartCalibration(ADC2, LL_ADC_DIFFERENTIAL_ENDED);
    while (LL_ADC_IsCalibrationOnGoing(ADC2) != 0) {}

    // 11. Configure Analog Watchdog 1
    LL_ADC_SetAnalogWDMonitChannels(ADC2, LL_ADC_AWD1, LL_ADC_AWD_CHANNEL_1_REG);
    LL_ADC_ConfigAnalogWDThresholds(ADC2, LL_ADC_AWD1, 2048, 0);
    LL_ADC_SetAWDFilteringConfiguration(ADC2, LL_ADC_AWD1, LL_ADC_AWD_FILTERING_NONE);
    LL_ADC_EnableIT_AWD1(ADC2);

    // 12. Enable interrupts
    NVIC_SetPriority(ADC1_2_IRQn, 0);
    NVIC_EnableIRQ(ADC1_2_IRQn);

    // 13. Enable ADC2
    LL_ADC_ClearFlag_ADRDY(ADC2);
    LL_ADC_Enable(ADC2);
    while (LL_ADC_IsActiveFlag_ADRDY(ADC2) == 0) {}
}

/******************************************************************************
  * @brief Configure GPIO Pins for ADC5 Injected Input
  ******************************************************************************/
void ADC5_PIN_Init(void) 
{ 
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    // Setup Pins: PA8 (ADC5_IN1), PA9 (ADC5_IN2)
    GPIO_InitStruct.Pin = LL_GPIO_PIN_8 | LL_GPIO_PIN_9;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct); 
}

/******************************************************************************
  * @brief Initialize and configure ADC5 peripheral (Injected Channel)
  ******************************************************************************/
void ADC5_Init(void) 
{ 
    // 1. Clock configuration
    LL_RCC_SetADCClockSource(LL_RCC_ADC345_CLKSOURCE_SYSCLK);
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_ADC345);

    // 2. Setup GPIO pins
    ADC5_PIN_Init();

    // 3. Setup ADC Common clock
    LL_ADC_CommonInitTypeDef ADC_CommonInitStruct = {
        .CommonClock = LL_ADC_CLOCK_SYNC_PCLK_DIV4,
        .Multimode = LL_ADC_MULTI_INDEPENDENT,
        .MultiDMATransfer = LL_ADC_MULTI_REG_DMA_EACH_ADC,
        .MultiTwoSamplingDelay = LL_ADC_MULTI_TWOSMP_DELAY_1CYCLE
    };
    LL_ADC_CommonInit(__LL_ADC_COMMON_INSTANCE(ADC5), &ADC_CommonInitStruct);

    LL_ADC_DisableDeepPowerDown(ADC5);
    LL_ADC_EnableInternalRegulator(ADC5);
    for (volatile uint32_t wait_loop = 0; wait_loop < 50000; wait_loop++) {}

    // 4. ADC configurations
    LL_ADC_InitTypeDef ADC_InitStruct = {
        .Resolution = LL_ADC_RESOLUTION_12B,
        .DataAlignment = LL_ADC_DATA_ALIGN_RIGHT,
        .LowPowerMode = LL_ADC_LP_MODE_NONE
    };
    LL_ADC_Init(ADC5, &ADC_InitStruct);

    // 5. Injected group setup
    LL_ADC_INJ_InitTypeDef ADC_INJ_InitStruct = {
        .TriggerSource = LL_ADC_INJ_TRIG_EXT_TIM2_TRGO,
        .SequencerLength = LL_ADC_INJ_SEQ_SCAN_DISABLE,
        .SequencerDiscont = LL_ADC_INJ_SEQ_DISCONT_DISABLE,
        .TrigAuto = LL_ADC_INJ_TRIG_INDEPENDENT
    };
    LL_ADC_INJ_Init(ADC5, &ADC_INJ_InitStruct);
    LL_ADC_INJ_SetTriggerEdge(ADC5, LL_ADC_INJ_TRIG_EXT_RISING);

    // 6. Channel setup
    LL_ADC_INJ_SetSequencerRanks(ADC5, LL_ADC_INJ_RANK_1, LL_ADC_CHANNEL_1);
    LL_ADC_SetChannelSamplingTime(ADC5, LL_ADC_CHANNEL_1, LL_ADC_SAMPLINGTIME_6CYCLES_5);
    LL_ADC_SetChannelSingleDiff(ADC5, LL_ADC_CHANNEL_1, LL_ADC_DIFFERENTIAL_ENDED);

    // 7. Oversampling
    LL_ADC_ConfigOverSamplingRatioShift(ADC5, LL_ADC_OVS_RATIO_8, LL_ADC_OVS_SHIFT_RIGHT_3);
    LL_ADC_SetOverSamplingScope(ADC5, LL_ADC_OVS_GRP_INJECTED);

    // 8. Calibration
    LL_ADC_StartCalibration(ADC5, LL_ADC_DIFFERENTIAL_ENDED);
    while (LL_ADC_IsCalibrationOnGoing(ADC5) != 0) {}

    // 9. Interrupts
    LL_ADC_ClearFlag_JEOS(ADC5);
    LL_ADC_EnableIT_JEOS(ADC5);

    NVIC_SetPriority(ADC5_IRQn, 0);
    NVIC_EnableIRQ(ADC5_IRQn);

    // 10. Enable ADC5
    LL_ADC_ClearFlag_ADRDY(ADC5);
    LL_ADC_Enable(ADC5);
    while (LL_ADC_IsActiveFlag_ADRDY(ADC5) == 0) {}
}