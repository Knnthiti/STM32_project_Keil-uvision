/******************************************************************************
  * @FileName    : bsp_adccfg.c
  * @Author      : by Haribabu.P
  * @Version     : v1.0
  * @Created     : 07-Jun-2024
  * @Target      : STM32C031F6P3 - TSSOP-20
  * @ToolChain   : Keil MDK_ARM
  * @ProjectName : PMZ250Z54V1B_Secondary
  * @Description : --
  * Copyright (c) Delta Electronics (Thailand) Public Co., Ltd.
  * All rights reserved.
  ******************************************************************************/

/* ------------------------ System Header Files ----------------------------- */
#include "stm32c0xx_ll_rcc.h"
#include "stm32c0xx_ll_dma.h"

/* ------------------------- User Header Files ------------------------------ */
#include "bsp_gpiocfg.h"
#include "bsp_adccfg.h"

/* -------------------------- Private typedef -------------------------------- */
/* --------------------------- Private define -------------------------------- */
/* ---------------------------- Private macro -------------------------------- */

/* -------------------------- Private variables ------------------------------ */
__IO uint16_t u16ADCxConvertedData[BSP_MAX_ADC_CHANNEL];
volatile uint8_t u8ShortCircuitCounter = 0;

/* --------------------------- Extern variables ------------------------------ */

/* ----------------------- Private function prototypes ----------------------- */
static fpADC_Receive_CallBack_t pThisFucHandler;

/* ---------------------------- Private functions ---------------------------- */

/******************************************************************************
  * @FunctionName : vBsp_STM32C0_ADCInit()
  * @Description  : This function configures ADC Pin functions.
  * @note         :
  * @Param        : None.
  * @Return       : None.
  ******************************************************************************/
static void vBsp_STM32C0_ADCInit(void) 
{
    LL_ADC_InitTypeDef ADC_InitStruct = {0};
    LL_ADC_REG_InitTypeDef ADC_REG_InitStruct = {0};
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    LL_RCC_SetADCClockSource(LL_RCC_ADC_CLKSOURCE_SYSCLK);
    
    /* Peripheral clock enable */
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC);
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);

    /** ADC1 GPIO Configuration
        PA0 ------> ADC1_IN0
        PA1 ------> ADC1_IN1
        PA2 ------> ADC1_IN2
        PA3 ------> ADC1_IN3
        PA4 ------> ADC1_IN4
    */
    
    /* --------------------------------------------------------------------------
       // ADC1 GPIO Configuration
       // Pin-7 : PA0/ADC_IN0 -->V input Reading - Analog Input
       // Pin-8 : PA1/ADC_IN1 -->I input Reading - Analog Input
       // Pin-9 : PA2/ADC_IN2 -->B+_SENSE Bulk Voltage reading - Analog Input
       // Pin-10: PA3/ADC_IN3 -->NTC Temperature reading - Analog Input
       // Pin-11: PA4/ADC_IN4 -->Vref2v5 Voltage reading - Analog Input
       -------------------------------------------------------------------------- */
    GPIO_InitStruct.Pin = PSU_uCS_VO_SENSE_GPIO_PIN | PSU_uCS_IOUT_SENSE_GPIO_PIN | 
                          PSU_uCS_NTC_SEC_GPIO_PIN  | PSU_uCS_2V5REF_GPIO_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /** Configure the global features of the ADC 
     * (Clock, Resolution, Data Alignment and number of conversion) 
     ****************************************************************/
    ADC_InitStruct.Clock = LL_ADC_CLOCK_SYNC_PCLK_DIV2;
    ADC_InitStruct.Resolution = LL_ADC_RESOLUTION_12B;
    ADC_InitStruct.DataAlignment = LL_ADC_DATA_ALIGN_RIGHT;
    ADC_InitStruct.LowPowerMode = LL_ADC_LP_MODE_NONE;
    LL_ADC_Init(ADC1, &ADC_InitStruct);

    LL_ADC_REG_SetSequencerConfigurable(ADC1, LL_ADC_REG_SEQ_FIXED);

    /* Clear flag ADC channel configuration ready */
    LL_ADC_ClearFlag_CCRDY(ADC1);

    ADC_REG_InitStruct.TriggerSource = LL_ADC_REG_TRIG_EXT_TIM3_TRGO; // LL_ADC_REG_TRIG_SOFTWARE;
    ADC_REG_InitStruct.SequencerLength = LL_ADC_REG_SEQ_SCAN_ENABLE_8RANKS;
    ADC_REG_InitStruct.SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_DISABLE;
    ADC_REG_InitStruct.ContinuousMode = LL_ADC_REG_CONV_SINGLE;
    ADC_REG_InitStruct.DMATransfer = LL_ADC_REG_DMA_TRANSFER_UNLIMITED;
    ADC_REG_InitStruct.Overrun = LL_ADC_REG_OVR_DATA_OVERWRITTEN;
    LL_ADC_REG_Init(ADC1, &ADC_REG_InitStruct);

    LL_ADC_ConfigOverSamplingRatioShift(ADC1, LL_ADC_OVS_RATIO_4, LL_ADC_OVS_SHIFT_RIGHT_2);
    LL_ADC_SetOverSamplingScope(ADC1, LL_ADC_OVS_GRP_REGULAR_CONTINUED);

    LL_ADC_SetTriggerFrequencyMode(ADC1, LL_ADC_CLOCK_FREQ_MODE_HIGH);
    LL_ADC_REG_SetTriggerEdge(ADC1, LL_ADC_REG_TRIG_EXT_RISING); // Rising edge on TRGO from TIM3

    LL_ADC_SetSamplingTimeCommonChannels(ADC1, LL_ADC_SAMPLINGTIME_COMMON_1, LL_ADC_SAMPLINGTIME_3CYCLES_5);

    LL_ADC_DisableIT_EOC(ADC1);
    LL_ADC_DisableIT_EOS(ADC1);

    LL_ADC_REG_SetSequencerChannels(ADC1, LL_ADC_CHANNEL_1 | LL_ADC_CHANNEL_2 | 
                                          LL_ADC_CHANNEL_3 | LL_ADC_CHANNEL_4);

    /* Clear flag ADC channel configuration ready */
    LL_ADC_ClearFlag_CCRDY(ADC1);

    LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_1, LL_ADC_SAMPLINGTIME_COMMON_1);
    LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_2, LL_ADC_SAMPLINGTIME_COMMON_1);
    LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_3, LL_ADC_SAMPLINGTIME_COMMON_1);
    LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_4, LL_ADC_SAMPLINGTIME_COMMON_1);

    // Setup Watchdog 1 on ADC Channel 2 to capture short circuit pulse
    LL_ADC_SetAnalogWDMonitChannels(ADC1, LL_ADC_AWD1, LL_ADC_AWD_CHANNEL_2_REG);
    LL_ADC_ConfigAnalogWDThresholds(ADC1, LL_ADC_AWD1, 
                                    (3500 >> 4), // High threshold is 14 Amps, trigger here (right shift by 4 is due to oversampling mode).
                                    0            // Low threshold is 0
                                   );

    LL_ADC_EnableIT_AWD1(ADC1);

    NVIC_SetPriority(ADC1_IRQn, 1);
    NVIC_EnableIRQ(ADC1_IRQn);
}

/******************************************************************************
  * @FunctionName : vBsp_DMAChannel1_ADCInit()
  * @Description  : This function configures DMA function.
  * @note         :
  * @Param        : None.
  * @Return       : None.
  *****************************************************************************/
static void vBsp_DMAChannel1_ADCInit(void)
{
    /* Init with LL driver // DMA controller clock enable */
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);

    /* ADC1 DMA Init // Configure DMA request ADC1TOMEM_DMA1_Channel1 */
    
    /* Set request number */
    LL_DMA_SetPeriphRequest(DMA1, LL_DMA_CHANNEL_1, LL_DMAMUX_REQ_ADC1);

    /* Set transfer direction */
    LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_CHANNEL_1, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);

    /* Set priority level */
    LL_DMA_SetChannelPriorityLevel(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PRIORITY_HIGH);

    /* Set DMA mode */
    LL_DMA_SetMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MODE_CIRCULAR);

    /* Set peripheral increment mode */
    LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PERIPH_NOINCREMENT);

    /* Set memory increment mode */
    LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MEMORY_INCREMENT);

    /* Set peripheral data width */
    LL_DMA_SetPeriphSize(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PDATAALIGN_HALFWORD);

    /* Set memory data width */
    LL_DMA_SetMemorySize(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MDATAALIGN_HALFWORD);

    /* Set DMA transfer addresses of source and destination */
    LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_1, 
                           LL_ADC_DMA_GetRegAddr(ADC1, LL_ADC_DMA_REG_REGULAR_DATA), 
                           (uint32_t)&u16ADCxConvertedData, 
                           LL_DMA_DIRECTION_PERIPH_TO_MEMORY);

    /* Set DMA transfer size */
    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_1, BSP_MAX_ADC_CHANNEL);

    /* Enable DMA transfer interruption: transfer complete */
    LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_1);

    /* DMA interrupt init // DMA1_Channel1_IRQn interrupt configuration */
    NVIC_SetPriority(DMA1_Channel1_IRQn, 0);
    NVIC_EnableIRQ(DMA1_Channel1_IRQn);

    /* Enable the DMA transfer */
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_1);
}

/*****************************************************************************
  * @FunctionName : vBsp_ADC_Activate()
  * @Description  : This function activates the ADC.
  * @note         :
  * @Param        : None.
  * @Return       : None.
  ******************************************************************************/
void vBsp_ADC_Activate(void) 
{
    __IO uint32_t wait_loop_index = 0U;
    __IO uint32_t backup_setting_adc_dma_transfer = 0U;

    if (LL_ADC_IsEnabled(ADC1) == 0) 
    {
        /* Enable ADC internal voltage regulator */
        LL_ADC_EnableInternalRegulator(ADC1);
        
        // Note: Missing delay here? STM32 usually needs ~20us after EnableInternalRegulator
    }

    LL_ADC_REG_StartConversion(ADC1);
}

/******************************************************************************
  * @FunctionName : vBsp_ADC1TrigTimerDMAInit()
  * @Description  : This function initialises DMA with ADC
  * @note         :
  * @Param        : fpADC_Receive_CallBack_t function pointer variable
  * @Return       : None.
  ******************************************************************************/
void vBsp_ADC1TrigTimerDMAInit(fpADC_Receive_CallBack_t pFucHandler) 
{
    pThisFucHandler = pFucHandler;

    vBsp_DMAChannel1_ADCInit();
    vBsp_STM32C0_ADCInit();
}

/******************************************************************************
  * @FunctionName : DMA1_Channel1_IRQHandler()
  * @Description  : This function calls the DMA ISR after Conversion Complete
  * @note         : This function handles DMA1 channel 1 interrupt.
  * @Param        : None.
  * @Return       : None.
  ******************************************************************************/
void DMA1_Channel1_IRQHandler(void) 
{
    /* USER CODE BEGIN DMA1_Channel1_IRQn 0 */
    if (LL_DMA_IsActiveFlag_TC1(DMA1) == 1)
    {
        /* Clear flag DMA transfer complete */
        LL_DMA_ClearFlag_TC1(DMA1);
        
        /* Call interruption treatment function */
        if (pThisFucHandler != 0) 
        {
            pThisFucHandler();
        }
    }
}

/******************************************************************************
  * @FunctionName : ADC1_IRQHandler()
  * @Description  : ADC1 Interrupt Service Routine
  ******************************************************************************/
void ADC1_IRQHandler(void)
{
    // ADC watchdog 1 & 2 to detect short circuit pulse and low voltage
    if (LL_ADC_IsActiveFlag_AWD1(ADC1) == 1)
    {
        PIN_DEBUG_TOGGLE();
        u8ShortCircuitCounter++;
        LL_ADC_ClearFlag_AWD1(ADC1);
    }
}

/******************************************************************************
  * @FunctionName : vBsp_ADCStartConversion()
  * @Description  : This function start ADC conversion
  * @note         :
  * @Param        : None.
  * @Return       : None.
  ******************************************************************************/
void vBsp_ADCStartConversion(void) 
{
    // if((LL_ADC_IsEnabled(ADC1) == 1) && 
    //    (LL_ADC_IsDisableOngoing(ADC1) == 0) && 
    //    (LL_ADC_REG_IsConversionOngoing(ADC1) == 0))
    // {
    //     LL_ADC_REG_StartConversion(ADC1);
    // }
}