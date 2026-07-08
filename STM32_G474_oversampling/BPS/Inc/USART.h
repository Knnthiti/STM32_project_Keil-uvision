#include "stm32g4xx_ll_bus.h"
#include "stm32g4xx_ll_gpio.h"
#include "stm32g4xx_ll_rcc.h"
#include "stm32g4xx_ll_utils.h"
#include "stm32g4xx_ll_usart.h"
#include "stm32g4xx_ll_dma.h"
#include "stm32g4xx_ll_dmamux.h"

#define SLAVE1_ADDRESS 0x01
#define SLAVE2_ADDRESS 0x02

void USART1_UART_Init(void);
void USART3_UART_Init(void);

void USART1_DMA_TX_Init(void);
void USART1_DMA_RX_Init(void);
void Send_Data_USART1_DMA(uint32_t *src_addr, uint32_t len);
void Receive_Data_USART1_DMA(uint32_t *dst_addr, uint32_t len);

void USART3_DMA_TX_Init(void);
void USART3_DMA_RX_Init(void);
void Send_Data_USART3_DMA(uint32_t *src_addr, uint32_t len);
void Receive_Data_USART3_DMA(uint32_t *dst_addr, uint32_t len);

void USART1_Master_Init(void);
void USART3_Slave_Init(void);
void UART4_Slave_Init(void);