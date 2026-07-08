#include "main.h"

// Define messages to send
volatile uint8_t tx_msg_U1[] = "67";
volatile uint8_t tx_msg_U1_2[] = "L9";

// Define buffers to store received data
volatile uint8_t rx_buffer_U1[20];
volatile uint8_t tx_msg_U3[] = "56";
volatile uint8_t rx_buffer_U3[20];
volatile uint8_t tx_msg_U4[] = "96";
volatile uint8_t rx_buffer_U4[20];

uint16_t address_packet = 0;

#define USART_DMA
//#define USART_mutil

#ifdef USART_DMA
int main(void)
{
    Clock_system(); // Configure system clock

    USART1_UART_Init(); // Initialize USART1
    USART3_UART_Init(); // Initialize USART3

    USART1_DMA_TX_Init(); // Setup DMA for USART1 transmit
    USART1_DMA_RX_Init(); // Setup DMA for USART1 receive

    USART3_DMA_TX_Init(); // Setup DMA for USART3 transmit
    USART3_DMA_RX_Init(); // Setup DMA for USART3 receive

    while (1)
    {
        // Send and receive data using DMA
        Send_Data_USART3_DMA((uint32_t *)tx_msg_U3, sizeof(tx_msg_U3));
        Receive_Data_USART1_DMA((uint32_t *)rx_buffer_U1, 20);
        LL_mDelay(1000); // Wait for 1 second

        Send_Data_USART1_DMA((uint32_t *)tx_msg_U1, sizeof(tx_msg_U1));
        Receive_Data_USART3_DMA((uint32_t *)rx_buffer_U3, 20);
        LL_mDelay(1000); // Wait for 1 second
    }
}
#endif

#ifdef USART_mutil
int main(void)
{
    Clock_system(); // Initialize system clock

    // Initialize all serial communication ports
    USART1_Master_Init();
    USART3_Slave_Init();
    UART4_Slave_Init();

    while (1)
    {
        // --- Communicate with Slave 1 ---
        address_packet = 0x0100 | SLAVE1_ADDRESS; // Prepare address packet (bit 9 set to 1)

        // Wait until transmit buffer is ready, then send address
        if (LL_USART_IsActiveFlag_TXE_TXFNF(USART1))
        {
            LL_USART_TransmitData9(USART1, address_packet);
        }

        // Loop through the data message
        for (uint32_t i = 0; i < sizeof(tx_msg_U1) - 1; i++)
        {
            uint16_t data_packet = 0x0000 | tx_msg_U1[i]; // Prepare data packet

            // Wait for buffer to be ready, then send data
            if (LL_USART_IsActiveFlag_TXE_TXFNF(USART1))
            {
                LL_USART_TransmitData9(USART1, data_packet);
            }
            LL_mDelay(10); // Small delay for transmission timing

            // Check which Slave is awake (RWU=0) and read its data
            if (LL_USART_IsActiveFlag_RWU(USART3) == 0)
            {
                rx_buffer_U3[i] = LL_USART_ReceiveData9(USART3); // Read from USART3
            }
            else if (LL_USART_IsActiveFlag_RWU(UART4) == 0)
            {
                rx_buffer_U4[i] = LL_USART_ReceiveData9(UART4); // Read from UART4
            }
        }

        LL_mDelay(1000); // Delay before response

        // Receive response from Slave 1
        for (uint32_t i = 0; i < sizeof(tx_msg_U3) - 1; i++)
        {
            if (LL_USART_IsActiveFlag_TXE_TXFNF(USART3))
            {
                LL_USART_TransmitData9(USART3, tx_msg_U3[i]); // Send response
            }
            LL_mDelay(10);
            rx_buffer_U1[i] = LL_USART_ReceiveData9(USART1); // Read response
        }
        LL_USART_RequestEnterMuteMode(USART3); // Put Slave 1 to sleep

        LL_mDelay(1000); // Wait before next cycle

        // --- Communicate with Slave 2 ---
        address_packet = 0x0100 | SLAVE2_ADDRESS; // Prepare address for Slave 2
        if (LL_USART_IsActiveFlag_TXE_TXFNF(USART1))
        {
            LL_USART_TransmitData9(USART1, address_packet); // Send address
        }

        // Loop for the second message
        for (uint32_t i = 0; i < sizeof(tx_msg_U1_2) - 1; i++)
        {
            uint16_t data_packet = 0x0000 | tx_msg_U1_2[i];
            if (LL_USART_IsActiveFlag_TXE_TXFNF(USART1))
            {
                LL_USART_TransmitData9(USART1, data_packet); // Send data
            }
            LL_mDelay(10);

            // Check status and read from the appropriate active slave
            if (LL_USART_IsActiveFlag_RWU(USART3) == 0)
            {
                rx_buffer_U3[i] = LL_USART_ReceiveData9(USART3);
            }
            else if (LL_USART_IsActiveFlag_RWU(UART4) == 0)
            {
                rx_buffer_U4[i] = LL_USART_ReceiveData9(UART4);
            }
        }

        LL_mDelay(1000); // Delay before response

        // Receive response from Slave 2
        for (uint32_t i = 0; i < sizeof(tx_msg_U4) - 1; i++)
        {
            if (LL_USART_IsActiveFlag_TXE_TXFNF(UART4))
            {
                LL_USART_TransmitData9(UART4, tx_msg_U4[i]); // Send response
            }
            LL_mDelay(10);
            rx_buffer_U1[i] = LL_USART_ReceiveData9(USART1); // Read response
        }
        LL_USART_RequestEnterMuteMode(UART4); // Put Slave 2 to sleep

        LL_mDelay(1000); // Final delay before loop repeats
    }
}
#endif