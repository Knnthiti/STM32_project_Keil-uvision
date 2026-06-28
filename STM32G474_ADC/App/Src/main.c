#include "main.h"

uint16_t ADC_Buffer[4];

volatile float V0, V1, V2, V3;

void ADC_DMA_Start(uint16_t *buffer, uint32_t len)
{
    // 1. Disable DMA channel before configuration
    LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_1);
    while (LL_DMA_IsEnabledChannel(DMA1, LL_DMA_CHANNEL_1))
    {
    }
    // 2. Set source (ADC2) and destination (Buffer) addresses
    LL_DMA_SetPeriphAddress(DMA1, LL_DMA_CHANNEL_1, LL_ADC_DMA_GetRegAddr(ADC2, LL_ADC_DMA_REG_REGULAR_DATA));
    LL_DMA_SetMemoryAddress(DMA1, LL_DMA_CHANNEL_1, (uint32_t)buffer);
    // 3. Set data length to transfer
    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_1, len);
    // 5. Enable DMA Channel
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_1);
    // 6. Ensure ADC is enabled and ready
    if (LL_ADC_IsEnabled(ADC2) == 0)
    {
        LL_ADC_Enable(ADC2);
        while (LL_ADC_IsActiveFlag_ADRDY(ADC2) == 0)
        {
        }
    }
    // 7. Start ADC conversion
    //LL_ADC_REG_StartConversion(ADC2);
}

int main(void)
{
    Clock_system();
    ADC_Init();
#ifdef ADC_DMA
    ADC_DMA_Start(ADC_Buffer, 4);
#endif
    LL_ADC_REG_StartConversion(ADC2);
    while (1)
    {
#if !defined(ADC_DMA)
        for (uint8_t i = 0; i < 4; i++)
        {
            // wait EOC flag
            while (LL_ADC_IsActiveFlag_EOC(ADC2) == 0)
            {
            }
            ADC_Buffer[i] = LL_ADC_REG_ReadConversionData12(ADC2);
        }
#endif
        V0 = (float)(ADC_Buffer[0] * 3.3 / 4095);
        V1 = (float)(ADC_Buffer[1] * 3.3 / 4095);
        V2 = (float)(ADC_Buffer[2] * 3.3 / 4095);
        V3 = (float)(ADC_Buffer[3] * 3.3 / 4095);
    }
}