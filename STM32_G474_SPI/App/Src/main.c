#include "main.h"
#include <stdio.h>

volatile uint8_t w5500_version = 0;

/**
 * @brief  TCP Client Handler (STM32 connects to the PC)
 */
void W5500_TCP_Client(void)
{
    uint8_t sn_sr, cmd;
    uint8_t val[4];

    // Read the current status of Socket 0
    Read_W5500(0x0003, 0x01, &sn_sr, 1);

    switch (sn_sr)
    {
        case 0x00: // Status: SOCK_CLOSED
            // 1. Set Socket 0 to TCP mode (0x01)
            cmd = 0x01;
            Write_W5500(0x0000, 0x0C, &cmd, 1); // Sn_MR

            // 2. Set STM32 Source Port (e.g., 50000 = 0xC350)
            val[0] = 0xC3; val[1] = 0x50;
            Write_W5500(0x0004, 0x0C, val, 2); // Sn_PORT

            // 3. Send OPEN command (0x01)
            cmd = 0x01;
            Write_W5500(0x0001, 0x0C, &cmd, 1); // Sn_CR
            break;

        case 0x13: // Status: SOCK_INIT (Port opened successfully)
            // 4. Set Destination IP (PC IP: 192.168.3.10)
            val[0] = 192; val[1] = 168; val[2] = 3; val[3] = 10;
            Write_W5500(0x000C, 0x0C, val, 4); // Sn_DIPR

            // 5. Set Destination Port (Python Server Port: 5000 = 0x1388)
            val[0] = 0x13; val[1] = 0x88;
            Write_W5500(0x0010, 0x0C, val, 2); // Sn_DPORT

            // 6. Send CONNECT command (0x04)
            cmd = 0x04;
            Write_W5500(0x0001, 0x0C, &cmd, 1); // Sn_CR
            break;

        case 0x17: // Status: SOCK_ESTABLISHED (Connected)
        {
            uint8_t tx_buffer[30] = "Hello STM32";
            uint16_t send_len = 0;
            uint16_t ptr = 0;

            // Calculate message length
            while(tx_buffer[send_len] != '\0') { send_len++; }

            // 8. Get the TX Write Pointer (Sn_TX_WR)
            Read_W5500(0x0024, 0x01, val, 2);
            ptr = (val[0] << 8) | val[1];

            // 9. Write data to TX Buffer
            Write_W5500(ptr, 0x14, (uint8_t*)tx_buffer, send_len);

            // 10. Move Pointer forward
            ptr += send_len;
            val[0] = (uint8_t)(ptr >> 8);
            val[1] = (uint8_t)(ptr & 0xFF);
            Write_W5500(0x0024, 0x0C, val, 2); // Update Sn_TX_WR

            // 11. Send SEND command (0x20)
            cmd = 0x20;
            Write_W5500(0x0001, 0x0C, &cmd, 1); // Sn_CR

            LL_mDelay(1000); // Delay 1s
            break;
        }

        case 0x1C: // Status: SOCK_CLOSE_WAIT
        case 0x14: // Status: SOCK_LISTEN
            // Send DISCON command (0x08) to disconnect and restart
            cmd = 0x08;
            Write_W5500(0x0001, 0x0C, &cmd, 1); // Sn_CR
            break;
    }
}

/**
 * @brief  Main program
 */
int main(void)
{
    // 1. Initialize system clock
    Clock_system();
    LL_Init1msTick(170000000);

    // 2. Initialize SPI
    SPI2_Init();

    // 3. Hardware Reset for W5500
    LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_2); // RST LOW
    LL_mDelay(10);
    LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_2);   // RST HIGH
    LL_mDelay(100);

    // Check SPI connection
    Read_W5500(0x0039, 0x00, (uint8_t*)&w5500_version, 1);

    if (w5500_version == 0x04)
    {
        W5500_Init(); // Configure IP, MAC, Gateway
    }
    else
    {
        while(1) { LL_mDelay(500); } // Error: SPI failed
    }

    // Main Loop
    while (1)
    {
        W5500_TCP_Client();
    }
}