#include "ADC.h"

/******************************************************************************
@brief Configure GPIO Pins for ADC2 Input 
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
@brief Initialize DMA1 Channel 1 for ADC2 data transfer 
******************************************************************************/ 
void ADC2_DMA_Init(void) 
{ 
    // 1. Enable DMA1 and DMAMUX1 peripheral clocks 
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1); 
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMAMUX1);

    // 2. Route ADC2 request line to DMA1 Channel 1 
    LL_DMA_SetPeriphRequest(DMA1, LL_DMA_CHANNEL_1, LL_DMAMUX_REQ_ADC2);

    // 3. Set data direction from Peripheral (ADC) to Memory (RAM) 
    LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_CHANNEL_1, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);

    // 4. Set DMA channel software priority level 
    LL_DMA_SetChannelPriorityLevel(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PRIORITY_LOW);

    // 5. Enable Circular mode to loop data transfer automatically 
    LL_DMA_SetMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MODE_CIRCULAR);

    // 6. Set address increment mode 
    LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PERIPH_NOINCREMENT); // ADC address stays same 
    LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MEMORY_INCREMENT);   // RAM address moves forward

    // 7. Set data transfer size to 16-bit (HalfWord) 
    LL_DMA_SetPeriphSize(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PDATAALIGN_HALFWORD); 
    LL_DMA_SetMemorySize(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MDATAALIGN_HALFWORD); 
}

/******************************************************************************
@brief Initialize and configure ADC2 peripheral 
******************************************************************************/ 
void ADC2_Init(void) 
{ 
    // 1. Configure clock source and enable hardware clock 
    LL_RCC_SetADCClockSource(LL_RCC_ADC12_CLKSOURCE_SYSCLK); // Use SYSCLK for ADC 
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_ADC12);     // Enable ADC hardware clock

    // 2. Setup GPIO pins and DMA transfer 
    ADC2_PIN_Init(); 
    ADC2_DMA_Init();

    // 3. Setup ADC Common clock and multi-mode configurations 
    LL_ADC_CommonInitTypeDef ADC_CommonInitStruct = { 
        .CommonClock = LL_ADC_CLOCK_SYNC_PCLK_DIV4,       // ADC Clock = PCLK / 4 
        .Multimode = LL_ADC_MULTI_INDEPENDENT,            // Standalone independent mode 
        .MultiDMATransfer = LL_ADC_MULTI_REG_DMA_EACH_ADC, // Standard DMA mode 
        .MultiTwoSamplingDelay = LL_ADC_MULTI_TWOSMP_DELAY_1CYCLE // 1 cycle delay between samples 
    }; 
    LL_ADC_CommonInit(__LL_ADC_COMMON_INSTANCE(ADC2), &ADC_CommonInitStruct);

    // 4. Power up the ADC internal voltage regulator 
    LL_ADC_DisableDeepPowerDown(ADC2); 
    LL_ADC_EnableInternalRegulator(ADC2); 
    for (volatile uint32_t wait_loop = 0; wait_loop < 50000; wait_loop++) 
    { 
        // Wait for internal voltage regulator stability (~20us) 
    }

    // 5. Configure basic ADC configurations 
    LL_ADC_InitTypeDef ADC_InitStruct = { 
        .Resolution = LL_ADC_RESOLUTION_12B,   // 12-bit ADC resolution 
        .DataAlignment = LL_ADC_DATA_ALIGN_RIGHT, // Right-aligned data 
        .LowPowerMode = LL_ADC_LP_MODE_NONE     // No low power mode 
    }; 
    LL_ADC_Init(ADC2, &ADC_InitStruct);

    // 6. Setup Regular group queue and hardware trigger source 
    LL_ADC_REG_InitTypeDef ADC_REG_InitStruct = { 
        .TriggerSource = LL_ADC_REG_TRIG_EXT_TIM3_CH4,       // Triggered by Timer 3 Channel 4 
        .SequencerLength = LL_ADC_REG_SEQ_SCAN_ENABLE_2RANKS, // Scan 2 channels in sequence 
        .SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_DISABLE,   // Convert all ranks continuously 
        .ContinuousMode = LL_ADC_REG_CONV_CONTINUOUS, 
        .DMATransfer = LL_ADC_REG_DMA_TRANSFER_UNLIMITED,     // Keep sending data to DMA 
        .Overrun = LL_ADC_REG_OVR_DATA_OVERWRITTEN            // Overwrite old data on overrun 
    }; 
    LL_ADC_REG_Init(ADC2, &ADC_REG_InitStruct);

    // 7. Additional hardware configurations for ADC2 
    LL_ADC_SetGainCompensation(ADC2, 0);                                 // Disable gain compensation 
    LL_ADC_REG_SetDMATransfer(ADC2, LL_ADC_REG_DMA_TRANSFER_UNLIMITED);  // Enable regular DMA requests 
    LL_ADC_REG_SetTriggerEdge(ADC2, LL_ADC_REG_TRIG_EXT_RISING);         // Trigger on rising edge

    // 8. Assign channels to regular sequencer ranks 
    // Rank 1 -> Channel 1 (Differential Mode) 
    LL_ADC_REG_SetSequencerRanks(ADC2, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_1); 
    LL_ADC_SetChannelSamplingTime(ADC2, LL_ADC_CHANNEL_1, LL_ADC_SAMPLINGTIME_6CYCLES_5); 
    LL_ADC_SetChannelSingleDiff(ADC2, LL_ADC_CHANNEL_1, LL_ADC_DIFFERENTIAL_ENDED);

    // Rank 2 -> Channel 3 (Differential Mode) 
    LL_ADC_REG_SetSequencerRanks(ADC2, LL_ADC_REG_RANK_2, LL_ADC_CHANNEL_3); 
    LL_ADC_SetChannelSamplingTime(ADC2, LL_ADC_CHANNEL_3, LL_ADC_SAMPLINGTIME_6CYCLES_5); 
    LL_ADC_SetChannelSingleDiff(ADC2, LL_ADC_CHANNEL_3, LL_ADC_DIFFERENTIAL_ENDED);

    // 9. Configure Regular group hardware Oversampling 
    LL_ADC_ConfigOverSamplingRatioShift(ADC2, LL_ADC_OVS_RATIO_4, LL_ADC_OVS_SHIFT_RIGHT_2); // 4x ratio, shift right by 2 
    LL_ADC_SetOverSamplingScope(ADC2, LL_ADC_OVS_GRP_REGULAR_CONTINUED);                    // Enable for regular group 
    LL_ADC_SetOverSamplingDiscont(ADC2, LL_ADC_OVS_REG_CONT);                               // 4 conversions run continuously

    // 10. Run ADC calibration 
    LL_ADC_StartCalibration(ADC2, LL_ADC_DIFFERENTIAL_ENDED); 
    while (LL_ADC_IsCalibrationOnGoing(ADC2) != 0) 
    { 
        // Wait until calibration is finished 
    }

    // 11. Configure Analog Watchdog 1 to monitor Channel 1 
    LL_ADC_SetAnalogWDMonitChannels(ADC2, LL_ADC_AWD1, LL_ADC_AWD_CHANNEL_1_REG); 
    LL_ADC_ConfigAnalogWDThresholds(ADC2, LL_ADC_AWD1, 2048, 0);                 // High threshold = 2048, Low = 0 
    LL_ADC_SetAWDFilteringConfiguration(ADC2, LL_ADC_AWD1, LL_ADC_AWD_FILTERING_NONE); // Disable filtering for fast trip 
    LL_ADC_EnableIT_AWD1(ADC2);                                                   // Enable Watchdog 1 interrupt

    // 12. Enable ADC regular group interrupts in CPU 
    NVIC_SetPriority(ADC1_2_IRQn, 0); 
    NVIC_EnableIRQ(ADC1_2_IRQn);

    // 13. Enable ADC2 peripheral 
    LL_ADC_ClearFlag_ADRDY(ADC2); 
    LL_ADC_Enable(ADC2); 
    while (LL_ADC_IsActiveFlag_ADRDY(ADC2) == 0) 
    { 
        // Wait until ADC is ready 
    } 
}

/******************************************************************************
@brief Configure GPIO Pins for ADC5 Injected Input 
******************************************************************************/ 
void ADC5_PIN_Init(void) 
{ 
    // 1. Enable GPIO port A clock 
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);

    // 2. Configure pins to Analog mode 
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    // Setup Pins: PA8 (ADC5_IN1 / Positive VINP), PA9 (ADC5_IN2 / Negative VINN) 
    GPIO_InitStruct.Pin = LL_GPIO_PIN_8 | LL_GPIO_PIN_9; 
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG; 
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO; 
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct); 
}

/******************************************************************************
@brief Initialize and configure ADC5 peripheral (Injected Channel) 
******************************************************************************/ 
void ADC5_Init(void) 
{ 
    // 1. Configure clock source and enable hardware clock for ADC345 group 
    // [FIX 1] Change to SYNC Clock to match TIM2 TRGO precisely and eliminate trigger miss 
    LL_RCC_SetADCClockSource(LL_RCC_ADC345_CLKSOURCE_SYSCLK); 
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_ADC345);

    // 2. Setup GPIO pins 
    ADC5_PIN_Init();

    // 3. Setup ADC Common clock configurations 
    LL_ADC_CommonInitTypeDef ADC_CommonInitStruct = { 
        .CommonClock = LL_ADC_CLOCK_SYNC_PCLK_DIV4, // Share clock domain with Timers 
        .Multimode = LL_ADC_MULTI_INDEPENDENT, 
        .MultiDMATransfer = LL_ADC_MULTI_REG_DMA_EACH_ADC, 
        .MultiTwoSamplingDelay = LL_ADC_MULTI_TWOSMP_DELAY_1CYCLE 
    }; 
    LL_ADC_CommonInit(__LL_ADC_COMMON_INSTANCE(ADC5), &ADC_CommonInitStruct);

    // [FIX 2] CRITICAL: Wake up ADC5 from Deep Power Down and turn on Voltage Regulator 
    LL_ADC_DisableDeepPowerDown(ADC5); 
    LL_ADC_EnableInternalRegulator(ADC5);

    // Wait ~20us for ADC internal power to become stable 
    for (volatile uint32_t wait_loop = 0; wait_loop < 50000; wait_loop++) 
    { 
    }

    // 4. Configure basic ADC configurations 
    LL_ADC_InitTypeDef ADC_InitStruct = { 
        .Resolution = LL_ADC_RESOLUTION_12B, 
        .DataAlignment = LL_ADC_DATA_ALIGN_RIGHT, 
        .LowPowerMode = LL_ADC_LP_MODE_NONE 
    }; 
    LL_ADC_Init(ADC5, &ADC_InitStruct);

    // 5. Setup Injected group queue and hardware trigger source 
    LL_ADC_INJ_InitTypeDef ADC_INJ_InitStruct = { 
        .TriggerSource = LL_ADC_INJ_TRIG_EXT_TIM2_TRGO, 
        .SequencerLength = LL_ADC_INJ_SEQ_SCAN_DISABLE, 
        .SequencerDiscont = LL_ADC_INJ_SEQ_DISCONT_DISABLE, 
        .TrigAuto = LL_ADC_INJ_TRIG_INDEPENDENT 
    }; 
    LL_ADC_INJ_Init(ADC5, &ADC_INJ_InitStruct); 
    LL_ADC_INJ_SetTriggerEdge(ADC5, LL_ADC_INJ_TRIG_EXT_RISING);

    // 6. Assign channel to Injected sequencer Rank 1 (Differential Mode) 
    LL_ADC_INJ_SetSequencerRanks(ADC5, LL_ADC_INJ_RANK_1, LL_ADC_CHANNEL_1); 
    LL_ADC_SetChannelSamplingTime(ADC5, LL_ADC_CHANNEL_1, LL_ADC_SAMPLINGTIME_6CYCLES_5); 
    LL_ADC_SetChannelSingleDiff(ADC5, LL_ADC_CHANNEL_1, LL_ADC_DIFFERENTIAL_ENDED);

    // 7. Configure Injected group hardware Oversampling 
    LL_ADC_ConfigOverSamplingRatioShift(ADC5, LL_ADC_OVS_RATIO_8, LL_ADC_OVS_SHIFT_RIGHT_3); 
    LL_ADC_SetOverSamplingScope(ADC5, LL_ADC_OVS_GRP_INJECTED);

    // 8. Run ADC calibration 
    LL_ADC_StartCalibration(ADC5, LL_ADC_DIFFERENTIAL_ENDED); 
    while (LL_ADC_IsCalibrationOnGoing(ADC5) != 0) 
    { 
        // Wait until calibration is finished 
    }

    // 10. Enable peripheral interrupts for ADC5 (JEOS is safer than JEOC for injected group) 
    LL_ADC_ClearFlag_JEOS(ADC5); 
    LL_ADC_EnableIT_JEOS(ADC5); // Changed to End of Sequence for robust interrupt handling

    // 11. Enable ADC5 system interrupt in CPU 
    NVIC_SetPriority(ADC5_IRQn, 0); 
    NVIC_EnableIRQ(ADC5_IRQn);

    // 12. Enable ADC5 peripheral 
    LL_ADC_ClearFlag_ADRDY(ADC5); 
    LL_ADC_Enable(ADC5);

    while (LL_ADC_IsActiveFlag_ADRDY(ADC5) == 0) 
    { 
        // Now it won't get stuck here anymore! 
    } 
}