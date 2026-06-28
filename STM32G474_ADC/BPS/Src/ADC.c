#include "ADC.h"
void ADC_PIN_Init(void)
{ 
    // 1. Enable GPIO clocks 
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC); 
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);

    // 2. Configure pins to Analog mode
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    // Setup PC0, PC1, PC2, PC4
    GPIO_InitStruct.Pin = LL_GPIO_PIN_0 | LL_GPIO_PIN_1 | LL_GPIO_PIN_2 | LL_GPIO_PIN_4;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    // Setup PA0, PA1, PA6, PA7
    GPIO_InitStruct.Pin = LL_GPIO_PIN_0 | LL_GPIO_PIN_1 | LL_GPIO_PIN_6 | LL_GPIO_PIN_7;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}
void ADC_DMA_Init(void)
{ 
    // 1. Enable DMA and DMAMUX clocks (CRITICAL FIX!) 
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1); 
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMAMUX1);
    // 2. Link DMA to ADC2
    LL_DMA_SetPeriphRequest(DMA1, LL_DMA_CHANNEL_1, LL_DMAMUX_REQ_ADC2);

    // 3. Set data direction (ADC to RAM)
    LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_CHANNEL_1, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);

    // 4. Set priority
    LL_DMA_SetChannelPriorityLevel(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PRIORITY_LOW);

    // 5. Set loop mode (Circular)
    LL_DMA_SetMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MODE_CIRCULAR);

    // 6. Set address increment mode
    LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PERIPH_NOINCREMENT); // ADC address stays same
    LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MEMORY_INCREMENT);   // RAM address moves forward

    // 7. Set data size (16-bit / HalfWord)
    LL_DMA_SetPeriphSize(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PDATAALIGN_HALFWORD);
    LL_DMA_SetMemorySize(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MDATAALIGN_HALFWORD);
}

void ADC_Init(void)
{
    LL_RCC_SetADCClockSource(LL_RCC_ADC12_CLKSOURCE_SYSCLK); // Use SYSCLK for ADC 
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_ADC12); // Enable ADC hardware clock
    // 2. Setup GPIO and DMA
    ADC_PIN_Init();
    ADC_DMA_Init();

    // 3. Setup ADC Common
    LL_ADC_CommonInitTypeDef ADC_CommonInitStruct = {
        .CommonClock = LL_ADC_CLOCK_SYNC_PCLK_DIV4,               // Divide clock by 4
        .Multimode = LL_ADC_MULTI_INDEPENDENT,                    // Standalone mode
        .MultiDMATransfer = LL_ADC_MULTI_REG_DMA_EACH_ADC,        // Standard DMA mode
        .MultiTwoSamplingDelay = LL_ADC_MULTI_TWOSMP_DELAY_1CYCLE // Small delay Sampling
    };
    LL_ADC_CommonInit(__LL_ADC_COMMON_INSTANCE(ADC2), &ADC_CommonInitStruct);

    LL_ADC_DisableDeepPowerDown(ADC2);
    LL_ADC_EnableInternalRegulator(ADC2);

    // Wait ~20us for power to become stable
    for (volatile uint32_t wait_loop = 0; wait_loop < 50000; wait_loop++)
    {
    }

    // 5. Basic ADC setup
    LL_ADC_InitTypeDef ADC_InitStruct = {
        .Resolution = LL_ADC_RESOLUTION_12B,      // 12-bit resolution
        .DataAlignment = LL_ADC_DATA_ALIGN_RIGHT, // Data on the right side
        .LowPowerMode = LL_ADC_LP_MODE_NONE       // LP_MODE : ADC conversions are performed only when necessary
    };
    LL_ADC_Init(ADC2, &ADC_InitStruct);

    // 6. Setup queue (Sequence)
    LL_ADC_REG_InitTypeDef ADC_REG_InitStruct = {
        .TriggerSource = LL_ADC_REG_TRIG_SOFTWARE,            // Start by software code
        .SequencerLength = LL_ADC_REG_SEQ_SCAN_ENABLE_4RANKS, // Read 4 channels (CRITICAL FIX!)
        .SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_DISABLE,   // Read all at once
        .ContinuousMode = LL_ADC_REG_CONV_CONTINUOUS,         // loop
        .DMATransfer = LL_ADC_REG_DMA_TRANSFER_UNLIMITED,     // Keep sending to DMA
        .Overrun = LL_ADC_REG_OVR_DATA_OVERWRITTEN            // Overwrite old data if full
    };
    LL_ADC_REG_Init(ADC2, &ADC_REG_InitStruct);

    // 7. Assign channels to queue (Rank 1 to 4)
    // Rank 1 -> Channel 1
    LL_ADC_REG_SetSequencerRanks(ADC2, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_1);
    LL_ADC_SetChannelSamplingTime(ADC2, LL_ADC_CHANNEL_1, LL_ADC_SAMPLINGTIME_12CYCLES_5);
    LL_ADC_SetChannelSingleDiff(ADC2, LL_ADC_CHANNEL_1, LL_ADC_DIFFERENTIAL_ENDED);

    // Rank 2 -> Channel 3
    LL_ADC_REG_SetSequencerRanks(ADC2, LL_ADC_REG_RANK_2, LL_ADC_CHANNEL_3);
    LL_ADC_SetChannelSamplingTime(ADC2, LL_ADC_CHANNEL_3, LL_ADC_SAMPLINGTIME_12CYCLES_5);
    LL_ADC_SetChannelSingleDiff(ADC2, LL_ADC_CHANNEL_3, LL_ADC_DIFFERENTIAL_ENDED);

    // Rank 3 -> Channel 5
    LL_ADC_REG_SetSequencerRanks(ADC2, LL_ADC_REG_RANK_3, LL_ADC_CHANNEL_5);
    LL_ADC_SetChannelSamplingTime(ADC2, LL_ADC_CHANNEL_5, LL_ADC_SAMPLINGTIME_12CYCLES_5);
    LL_ADC_SetChannelSingleDiff(ADC2, LL_ADC_CHANNEL_5, LL_ADC_DIFFERENTIAL_ENDED);

    // Rank 4 -> Channel 7
    LL_ADC_REG_SetSequencerRanks(ADC2, LL_ADC_REG_RANK_4, LL_ADC_CHANNEL_7);
    LL_ADC_SetChannelSamplingTime(ADC2, LL_ADC_CHANNEL_7, LL_ADC_SAMPLINGTIME_12CYCLES_5);
    LL_ADC_SetChannelSingleDiff(ADC2, LL_ADC_CHANNEL_7, LL_ADC_DIFFERENTIAL_ENDED);

    // 8. Calibrate the scale (Find zero error)
    LL_ADC_StartCalibration(ADC2, LL_ADC_DIFFERENTIAL_ENDED);
    while (LL_ADC_IsCalibrationOnGoing(ADC2) != 0)
    {
        // Wait until done
    }

    // 9. Turn on ADC
    LL_ADC_ClearFlag_ADRDY(ADC2);
    LL_ADC_Enable(ADC2);
    while (LL_ADC_IsActiveFlag_ADRDY(ADC2) == 0)
    {
        // Wait until ready
    }
}