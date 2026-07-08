#include "DMA.h"

/**
 * @brief Initialize DMA1 Channel 1 for ADC2 data transfer
 */
void ADC2_DMA_Init(void)
{
    // 1. Enable DMA1 and DMAMUX1 peripheral clocks
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMAMUX1);

    LL_DMAMUX_EnableEventGeneration(DMAMUX1, LL_DMAMUX_CHANNEL_0);

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

/**
 * @brief  Configure and start DMA1 Channel 1 for ADC2
 * @param  buffer: Pointer to the destination array
 * @param  len: Number of data items to transfer
 * @retval None 
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
 * @brief Initialize DMA1 Channel 2 for TIM2 PWM update via Request Generator
 */
void TIM2_DMA_Init(void)
{
    // 1. Enable DMA1 and DMAMUX1 peripheral clocks
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMAMUX1);

    // 2. Route DMA1 Channel 2 request to "Request Generator 0"
    LL_DMA_SetPeriphRequest(DMA1, LL_DMA_CHANNEL_2, LL_DMAMUX_REQ_GENERATOR0);

    // 3. Set data direction from Memory (RAM) to Peripheral (TIM2 CCRx)
    LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_CHANNEL_2, LL_DMA_DIRECTION_MEMORY_TO_PERIPH);

    // 4. Set DMA channel software priority level
    LL_DMA_SetChannelPriorityLevel(DMA1, LL_DMA_CHANNEL_2, LL_DMA_PRIORITY_LOW);

    // 5. Enable Circular mode to loop data transfer automatically
    LL_DMA_SetMode(DMA1, LL_DMA_CHANNEL_2, LL_DMA_MODE_CIRCULAR);

    // 6. Set address increment mode
    LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_CHANNEL_2, LL_DMA_PERIPH_INCREMENT);
    LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_CHANNEL_2, LL_DMA_MEMORY_INCREMENT);

    // 7. Set data transfer size to 32-bit Word (TIM2 registers are 32-bit)
    LL_DMA_SetPeriphSize(DMA1, LL_DMA_CHANNEL_2, LL_DMA_PDATAALIGN_WORD);
    LL_DMA_SetMemorySize(DMA1, LL_DMA_CHANNEL_2, LL_DMA_MDATAALIGN_HALFWORD);

    // 8. Request Generator Setup: Trigger CH2 transfer using CH0 Event
    LL_DMAMUX_SetRequestSignalID(DMAMUX1, LL_DMAMUX_REQ_GEN_0, LL_DMAMUX_REQ_GEN_DMAMUX_CH0);
    LL_DMAMUX_SetRequestGenPolarity(DMAMUX1, LL_DMAMUX_REQ_GEN_0, LL_DMAMUX_REQ_GEN_POL_RISING);
    
    // Set request quota: 1 Event triggers 2 DMA Requests
    LL_DMAMUX_SetGenRequestNb(DMAMUX1, LL_DMAMUX_REQ_GEN_0, 2);
    LL_DMAMUX_EnableRequestGen(DMAMUX1, LL_DMAMUX_REQ_GEN_0);
}

/**
 * @brief  Start DMA for TIM2 PWM update
 * @param  buffer: Pointer to the source data array
 * @param  len: Number of data items (each item is 32-bit)
 */
void TIM2_DMA_Start(uint32_t *buffer, uint32_t len)
{
    // 1. Disable DMA channel before configuration
    LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_2);
    while (LL_DMA_IsEnabledChannel(DMA1, LL_DMA_CHANNEL_2))
    {
        // Wait until DMA channel is fully disabled
    }

    // 2. Set Memory (Source) and Peripheral (Destination) addresses
    LL_DMA_SetMemoryAddress(DMA1, LL_DMA_CHANNEL_2, (uint32_t)buffer);
    LL_DMA_SetPeriphAddress(DMA1, LL_DMA_CHANNEL_2, (uint32_t)&(TIM2->CCR1));

    // 3. Set the total data length
    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_2, len);

    // 4. Enable DMA Channel to arm the hardware transfer
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_2);
}