#include "USART.h"

/**
  * @brief USART1 Initialization Function (Standard UART)
  */
void USART1_UART_Init(void) {
    LL_USART_InitTypeDef USART_InitStruct = {0};
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* 1. Clock Configuration */
    LL_RCC_SetUSARTClockSource(LL_RCC_USART1_CLKSOURCE_PCLK2);
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);

    /* 2. GPIO Configuration (PC4: TX, PC5: RX) */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_4 | LL_GPIO_PIN_5;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_7;
    LL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* 3. Basic USART Settings */
    USART_InitStruct.PrescalerValue = LL_USART_PRESCALER_DIV1;
    USART_InitStruct.BaudRate = 115200;
    USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_9B;   // 8 Data + 1 Parity
    USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
    USART_InitStruct.Parity = LL_USART_PARITY_ODD;        // Enable error checking
    USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
    USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
    USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16; // Better noise filtering
    LL_USART_Init(USART1, &USART_InitStruct);

    /* 4. FIFO & Advanced Settings */
    LL_USART_SetTXFIFOThreshold(USART1, LL_USART_FIFOTHRESHOLD_1_4); // Interrupt if TX FIFO has 2 empty slots
    LL_USART_SetRXFIFOThreshold(USART1, LL_USART_FIFOTHRESHOLD_1_4); // Interrupt if RX FIFO has 2 new bytes
    LL_USART_EnableFIFO(USART1);
    LL_USART_DisableOverrunDetect(USART1);             // Ignore overrun errors
    LL_USART_DisableDMADeactOnRxErr(USART1);           // Keep DMA active on error
    LL_USART_ConfigAsyncMode(USART1);                  // Set to standard UART
    
    /* 5. Enable Peripheral */
    LL_USART_Enable(USART1);
    
    /* 6. Wait for hardware to acknowledge readiness */
    while ((!(LL_USART_IsActiveFlag_TEACK(USART1))) || (!(LL_USART_IsActiveFlag_REACK(USART1)))) { }
}

/**
  * @brief USART3 Initialization Function (Standard UART)
  */
void USART3_UART_Init(void) {
    LL_USART_InitTypeDef USART_InitStruct = {0};
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* 1. Clock Configuration */
    LL_RCC_SetUSARTClockSource(LL_RCC_USART3_CLKSOURCE_PCLK1);
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART3);
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);

    /* 2. GPIO Configuration (PB10: TX, PB11: RX) */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_10 | LL_GPIO_PIN_11;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_7;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* 3. Basic USART Settings */
    USART_InitStruct.PrescalerValue = LL_USART_PRESCALER_DIV1;
    USART_InitStruct.BaudRate = 115200;
    USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_9B;
    USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
    USART_InitStruct.Parity = LL_USART_PARITY_ODD;
    USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
    USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
    USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
    LL_USART_Init(USART3, &USART_InitStruct);

    /* 4. FIFO & Advanced Settings */
    LL_USART_SetTXFIFOThreshold(USART3, LL_USART_FIFOTHRESHOLD_1_4);
    LL_USART_SetRXFIFOThreshold(USART3, LL_USART_FIFOTHRESHOLD_1_4);
    LL_USART_EnableFIFO(USART3);
    LL_USART_DisableOverrunDetect(USART3);
    LL_USART_DisableDMADeactOnRxErr(USART3);
    LL_USART_ConfigAsyncMode(USART3);
    
    /* 5. Enable Peripheral */
    LL_USART_Enable(USART3);
    
    /* 6. Wait for hardware to acknowledge readiness */
    while ((!(LL_USART_IsActiveFlag_TEACK(USART3))) || (!(LL_USART_IsActiveFlag_REACK(USART3)))) { }
}

/**
 * @brief Initialize DMA for USART1 transmit
 */
void USART1_DMA_TX_Init(void) {
    // 1. Enable DMA1 and DMAMUX1 clocks required for DMA operation
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMAMUX1);

    // 2. Configure DMA Channel 1 for USART1_TX
    // Link DMA Channel 1 to the USART1_TX request signal
    LL_DMA_SetPeriphRequest(DMA1, LL_DMA_CHANNEL_1, LL_DMAMUX_REQ_USART1_TX);
    // Set data direction from RAM memory to the USART peripheral
    LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_CHANNEL_1, LL_DMA_DIRECTION_MEMORY_TO_PERIPH);
    // Set DMA priority to low
    LL_DMA_SetChannelPriorityLevel(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PRIORITY_LOW);
    // Set transfer mode to normal (not circular)
    LL_DMA_SetMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MODE_NORMAL);
    // Do not increment peripheral address; increment memory address
    LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PERIPH_NOINCREMENT);
    LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MEMORY_INCREMENT);
    // Set transfer size to byte
    LL_DMA_SetPeriphSize(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PDATAALIGN_BYTE);
    LL_DMA_SetMemorySize(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MDATAALIGN_BYTE);
}

/**
 * @brief Initialize DMA for USART1 receive
 */
void USART1_DMA_RX_Init(void) {
    // 3. Configure DMA Channel 2 for USART1_RX
    // Link DMA Channel 2 to the USART1_RX request signal
    LL_DMA_SetPeriphRequest(DMA1, LL_DMA_CHANNEL_2, LL_DMAMUX_REQ_USART1_RX);
    // Set data direction from USART peripheral to RAM memory
    LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_CHANNEL_2, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
    // Set DMA priority to low
    LL_DMA_SetChannelPriorityLevel(DMA1, LL_DMA_CHANNEL_2, LL_DMA_PRIORITY_LOW);
    // Set transfer mode to normal
    LL_DMA_SetMode(DMA1, LL_DMA_CHANNEL_2, LL_DMA_MODE_NORMAL);
    // Do not increment peripheral address; increment memory address
    LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_CHANNEL_2, LL_DMA_PERIPH_NOINCREMENT);
    LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_CHANNEL_2, LL_DMA_MEMORY_INCREMENT);
    // Set transfer size to byte
    LL_DMA_SetPeriphSize(DMA1, LL_DMA_CHANNEL_2, LL_DMA_PDATAALIGN_BYTE);
    LL_DMA_SetMemorySize(DMA1, LL_DMA_CHANNEL_2, LL_DMA_MDATAALIGN_BYTE);
}

/**
 * @brief Helper function to start DMA Transmit
 * @param src_addr: Pointer to the data to be sent
 * @param len: Number of bytes to send
 */
void Send_Data_USART1_DMA(uint32_t *src_addr, uint32_t len) {
    // Disable DMA channel to configure it
    LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_1);
    // Set memory address (source) and peripheral address (USART_TDR)
    LL_DMA_SetMemoryAddress(DMA1, LL_DMA_CHANNEL_1, (uint32_t)src_addr);
    LL_DMA_SetPeriphAddress(DMA1, LL_DMA_CHANNEL_1, LL_USART_DMA_GetRegAddr(USART1, LL_USART_DMA_REG_DATA_TRANSMIT));
    // Set the number of bytes to send
    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_1, len);
    
    // Enable DMA request in USART peripheral and start DMA channel
    LL_USART_EnableDMAReq_TX(USART1);
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_1);
}

/**
 * @brief Helper function to start DMA Receive
 * @param dst_addr: Pointer to the buffer to store received data
 * @param len: Number of bytes to receive
 */
void Receive_Data_USART1_DMA(uint32_t *dst_addr, uint32_t len) {
    // Disable DMA channel to configure it
    LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_2);
    // Set memory address (destination) and peripheral address (USART_RDR)
    LL_DMA_SetMemoryAddress(DMA1, LL_DMA_CHANNEL_2, (uint32_t)dst_addr);
    LL_DMA_SetPeriphAddress(DMA1, LL_DMA_CHANNEL_2, LL_USART_DMA_GetRegAddr(USART1, LL_USART_DMA_REG_DATA_RECEIVE));
    // Set the number of bytes to receive
    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_2, len);
    
    // Enable DMA request in USART peripheral and start DMA channel
    LL_USART_EnableDMAReq_RX(USART1);
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_2);
}

void USART3_DMA_TX_Init(void) {
    // 1. Enable Clocks for DMA1 and DMAMUX1
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMAMUX1);

    // 2. Configure DMA1 Channel 3 for USART3_TX
    LL_DMA_SetPeriphRequest(DMA1, LL_DMA_CHANNEL_3, LL_DMAMUX_REQ_USART3_TX);
    LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_CHANNEL_3, LL_DMA_DIRECTION_MEMORY_TO_PERIPH);
    LL_DMA_SetChannelPriorityLevel(DMA1, LL_DMA_CHANNEL_3, LL_DMA_PRIORITY_LOW);
    LL_DMA_SetMode(DMA1, LL_DMA_CHANNEL_3, LL_DMA_MODE_NORMAL);
    LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_CHANNEL_3, LL_DMA_PERIPH_NOINCREMENT);
    LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_CHANNEL_3, LL_DMA_MEMORY_INCREMENT);
    LL_DMA_SetPeriphSize(DMA1, LL_DMA_CHANNEL_3, LL_DMA_PDATAALIGN_BYTE);
    LL_DMA_SetMemorySize(DMA1, LL_DMA_CHANNEL_3, LL_DMA_MDATAALIGN_BYTE);
}

/**
 * @brief Initialize DMA for USART3 Receive
 */
void USART3_DMA_RX_Init(void) {
    // 3. Configure DMA1 Channel 4 for USART3_RX
    LL_DMA_SetPeriphRequest(DMA1, LL_DMA_CHANNEL_4, LL_DMAMUX_REQ_USART3_RX);
    LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_CHANNEL_4, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
    LL_DMA_SetChannelPriorityLevel(DMA1, LL_DMA_CHANNEL_4, LL_DMA_PRIORITY_LOW);
    LL_DMA_SetMode(DMA1, LL_DMA_CHANNEL_4, LL_DMA_MODE_NORMAL);
    LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_CHANNEL_4, LL_DMA_PERIPH_NOINCREMENT);
    LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_CHANNEL_4, LL_DMA_MEMORY_INCREMENT);
    LL_DMA_SetPeriphSize(DMA1, LL_DMA_CHANNEL_4, LL_DMA_PDATAALIGN_BYTE);
    LL_DMA_SetMemorySize(DMA1, LL_DMA_CHANNEL_4, LL_DMA_MDATAALIGN_BYTE);
}

/**
 * @brief Helper function to start USART3 DMA Transmit
 */
void Send_Data_USART3_DMA(uint32_t *src_addr, uint32_t len) {
    LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_3);
    LL_DMA_SetMemoryAddress(DMA1, LL_DMA_CHANNEL_3, (uint32_t)src_addr);
    LL_DMA_SetPeriphAddress(DMA1, LL_DMA_CHANNEL_3, LL_USART_DMA_GetRegAddr(USART3, LL_USART_DMA_REG_DATA_TRANSMIT));
    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_3, len);
    
    LL_USART_EnableDMAReq_TX(USART3);
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_3);
}

/**
 * @brief Helper function to start USART3 DMA Receive
 */
void Receive_Data_USART3_DMA(uint32_t *dst_addr, uint32_t len) {
    LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_4);
    LL_DMA_SetMemoryAddress(DMA1, LL_DMA_CHANNEL_4, (uint32_t)dst_addr);
    LL_DMA_SetPeriphAddress(DMA1, LL_DMA_CHANNEL_4, LL_USART_DMA_GetRegAddr(USART3, LL_USART_DMA_REG_DATA_RECEIVE));
    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_4, len);
    
    LL_USART_EnableDMAReq_RX(USART3);
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_4);
}

/**
 * @brief  Initialize USART1 (Master Mode)
 * @param  None
 * @retval None
 */
void USART1_Master_Init(void)
{
    LL_USART_InitTypeDef USART_InitStruct = {0};
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    // 1. Turn on clocks
    LL_RCC_SetUSARTClockSource(LL_RCC_USART1_CLKSOURCE_PCLK2);
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);

    // 2. Configure TX/RX pins (PC4/PC5)
    GPIO_InitStruct.Pin = LL_GPIO_PIN_4 | LL_GPIO_PIN_5;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_7;
    LL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    // 3. Configure USART parameters
    USART_InitStruct.BaudRate = 115200;
    USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_9B;
    USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
    USART_InitStruct.Parity = LL_USART_PARITY_NONE;
    USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
    USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
    USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
    LL_USART_Init(USART1, &USART_InitStruct);

    // 4. Advanced Settings
    LL_USART_SetTXFIFOThreshold(USART1, LL_USART_FIFOTHRESHOLD_1_4);
    LL_USART_SetRXFIFOThreshold(USART1, LL_USART_FIFOTHRESHOLD_1_4);
    LL_USART_EnableFIFO(USART1);
    LL_USART_DisableOverrunDetect(USART1);
    LL_USART_DisableDMADeactOnRxErr(USART1);
    LL_USART_ConfigAsyncMode(USART1);
    LL_USART_Enable(USART1);

    // 5. Wait for hardware readiness
    while (!(LL_USART_IsActiveFlag_TEACK(USART1)) || !(LL_USART_IsActiveFlag_REACK(USART1))) {}
}

/**
 * @brief  Initialize USART3 (Slave Mode)
 * @param  None
 * @retval None
 */
void USART3_Slave_Init(void)
{
    LL_USART_InitTypeDef USART_InitStruct = {0};
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    // 1. Turn on clocks
    LL_RCC_SetUSARTClockSource(LL_RCC_USART3_CLKSOURCE_PCLK1);
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART3);
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);

    // 2. Setup GPIO pins (PB10=TX, PB11=RX)
    GPIO_InitStruct.Pin = LL_GPIO_PIN_10;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_7;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LL_GPIO_PIN_11;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // 3. Configure USART parameters
    USART_InitStruct.BaudRate = 115200;
    USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_9B;
    USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
    USART_InitStruct.Parity = LL_USART_PARITY_NONE;
    USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
    LL_USART_Init(USART3, &USART_InitStruct);

    // 4. Multiprocessor (Address Mark) Setup
    LL_USART_ConfigNodeAddress(USART3, LL_USART_ADDRESS_DETECT_4B, SLAVE1_ADDRESS);
    LL_USART_SetWakeUpMethod(USART3, LL_USART_WAKEUP_ADDRESSMARK);

    // 5. FIFO and Modes
    LL_USART_SetTXFIFOThreshold(USART3, LL_USART_FIFOTHRESHOLD_1_4);
    LL_USART_SetRXFIFOThreshold(USART3, LL_USART_FIFOTHRESHOLD_1_4);
    LL_USART_EnableFIFO(USART3);
    LL_USART_DisableOverrunDetect(USART3);
    LL_USART_DisableDMADeactOnRxErr(USART3);
    LL_USART_ConfigAsyncMode(USART3);

    LL_USART_Enable(USART3);
    while (!(LL_USART_IsActiveFlag_TEACK(USART3)) || !(LL_USART_IsActiveFlag_REACK(USART3))) {}

    // 6. Enter Mute mode
    LL_USART_EnableMuteMode(USART3);
    LL_USART_RequestEnterMuteMode(USART3);
}

/**
 * @brief  Initialize UART4 (Slave Mode)
 * @param  None
 * @retval None
 */
void UART4_Slave_Init(void)
{
    LL_USART_InitTypeDef UART_InitStruct = {0};
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    // 1. Turn on clocks
    LL_RCC_SetUARTClockSource(LL_RCC_UART4_CLKSOURCE_PCLK1);
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_UART4);
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);

    // 2. Setup GPIO pins (PC10=TX, PC11=RX)
    GPIO_InitStruct.Pin = LL_GPIO_PIN_10;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_5;
    LL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LL_GPIO_PIN_11;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    // 3. Configure UART parameters
    UART_InitStruct.BaudRate = 115200;
    UART_InitStruct.DataWidth = LL_USART_DATAWIDTH_9B;
    UART_InitStruct.StopBits = LL_USART_STOPBITS_1;
    UART_InitStruct.Parity = LL_USART_PARITY_NONE;
    UART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
    LL_USART_Init(UART4, &UART_InitStruct);

    // 4. Multiprocessor (Address Mark) Setup
    LL_USART_ConfigNodeAddress(UART4, LL_USART_ADDRESS_DETECT_4B, SLAVE2_ADDRESS);
    LL_USART_SetWakeUpMethod(UART4, LL_USART_WAKEUP_ADDRESSMARK);

    // 5. FIFO and Modes
    LL_USART_SetTXFIFOThreshold(UART4, LL_USART_FIFOTHRESHOLD_1_4);
    LL_USART_SetRXFIFOThreshold(UART4, LL_USART_FIFOTHRESHOLD_1_4);
    LL_USART_EnableFIFO(UART4);
    LL_USART_DisableOverrunDetect(UART4);
    LL_USART_DisableDMADeactOnRxErr(UART4);
    LL_USART_ConfigAsyncMode(UART4);

    LL_USART_Enable(UART4);
    while (!(LL_USART_IsActiveFlag_TEACK(UART4)) || !(LL_USART_IsActiveFlag_REACK(UART4))) {}

    // 6. Enter Mute mode
    LL_USART_EnableMuteMode(UART4);
    LL_USART_RequestEnterMuteMode(UART4);
}