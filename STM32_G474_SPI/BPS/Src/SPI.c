#include "SPI.h"

// ---------------------------------------------------------
// Function: SPI2_Init
// Purpose:  Configure SPI2 hardware and GPIO pins
// ---------------------------------------------------------
void SPI2_Init(void)
{
    LL_SPI_InitTypeDef SPI_InitStruct = {0};
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    // 1. Turn on the clock for SPI2 and Port B
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI2);
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);

    /** SPI2 GPIO Configuration 
     * PB13 ------> SPI2_SCK 
     * PB14 ------> SPI2_MISO 
     * PB15 ------> SPI2_MOSI 
     * PB1  ------> SPI2_CS (Chip Select)
     * PB2  ------> SPI2_RST (Reset for W5500)
     */

    // 2. Configure SCK, MISO, MOSI pins (PB13, PB14, PB15)
    GPIO_InitStruct.Pin = LL_GPIO_PIN_13 | LL_GPIO_PIN_14 | LL_GPIO_PIN_15;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_5;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // 3. Configure CS and RST pins as simple outputs
    GPIO_InitStruct.Pin = LL_GPIO_PIN_1 | LL_GPIO_PIN_2;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Alternate = 0;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // Set CS pin to HIGH (Standby state)
    LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_1);

    // 4. Configure SPI2 parameters
    SPI_InitStruct.TransferDirection = LL_SPI_FULL_DUPLEX;
    SPI_InitStruct.Mode = LL_SPI_MODE_MASTER;
    SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_8BIT;
    SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_LOW;
    SPI_InitStruct.ClockPhase = LL_SPI_PHASE_1EDGE;
    SPI_InitStruct.NSS = LL_SPI_NSS_SOFT;
    SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV8;
    SPI_InitStruct.BitOrder = LL_SPI_MSB_FIRST;
    SPI_InitStruct.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
    LL_SPI_Init(SPI2, &SPI_InitStruct);

    LL_SPI_SetStandard(SPI2, LL_SPI_PROTOCOL_MOTOROLA);
    LL_SPI_EnableNSSPulseMgt(SPI2);
    
    // Set alert when 1 byte is received
    LL_SPI_SetRxFIFOThreshold(SPI2, LL_SPI_RX_FIFO_TH_QUARTER);
    LL_SPI_Enable(SPI2);
}

// ---------------------------------------------------------
// Function: Write_W5500
// Purpose:  Send data to W5500 registers via SPI
// ---------------------------------------------------------
void Write_W5500(uint16_t address, uint8_t bsb, uint8_t *data, uint16_t len)
{
    uint8_t spi_fsm = 0;
    uint8_t header[3] = { (uint8_t)(address >> 8), (uint8_t)address, bsb };
    uint8_t header_idx = 0;
    uint16_t total_len = 3 + len;

    LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_1); // CS LOW

    do {
        switch(spi_fsm) {
            case 0: // Send
                if (header_idx < 3) {
                    LL_SPI_TransmitData8(SPI2, header[header_idx++]);
                } else {
                    LL_SPI_TransmitData8(SPI2, *data++);
                }
                spi_fsm = 1;
                break;

            case 1: // Wait for RX
                if (LL_SPI_IsActiveFlag_RXNE(SPI2) != 0) spi_fsm = 2;
                break;

            case 2: // Clear RX flag
                (void)LL_SPI_ReceiveData8(SPI2);
                total_len--;
                spi_fsm = 0;
                break;
        }
    } while(total_len > 0);

    LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_1); // CS HIGH
}

// ---------------------------------------------------------
// Function: Read_W5500
// Purpose:  Read data from W5500 registers via SPI
// ---------------------------------------------------------
void Read_W5500(uint16_t address, uint8_t bsb, uint8_t *data, uint16_t len)
{
    uint8_t spi_fsm = 0;
    uint8_t control_byte = (bsb << 3) | 0x00;
    uint8_t header[3] = { (uint8_t)(address >> 8), (uint8_t)address, control_byte };
    uint8_t header_idx = 0;
    uint16_t total_len = 3 + len;

    LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_1); // CS LOW

    do {
        switch(spi_fsm) {
            case 0: // Send
                if (header_idx < 3) {
                    LL_SPI_TransmitData8(SPI2, header[header_idx++]);
                } else {
                    LL_SPI_TransmitData8(SPI2, 0xFF); // Push dummy
                }
                spi_fsm = 1;
                break;

            case 1: // Wait for RX
                if (LL_SPI_IsActiveFlag_RXNE(SPI2) != 0) spi_fsm = 2;
                break;

            case 2: // Read Data
                {
                    uint8_t rx_data = LL_SPI_ReceiveData8(SPI2);
                    if (total_len <= len && data != NULL) {
                        *data++ = rx_data;
                    }
                    total_len--;
                    spi_fsm = 0;
                }
                break;
        }
    } while(total_len > 0);

    LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_1); // CS HIGH
}

// ---------------------------------------------------------
// Function: W5500_Init
// Purpose:  Configure IP, MAC, Subnet, and Gateway for W5500
// ---------------------------------------------------------
void W5500_Init(void)
{
    // 1. Hardware Reset
    LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_2);
    LL_mDelay(10);
    LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_2);
    LL_mDelay(100);

    // 2. Software Reset
    uint8_t reset_cmd = 0x80;
    Write_W5500(0x0000, 0x04, &reset_cmd, 1);
    LL_mDelay(50);

    // 3. Set Gateway, Subnet, MAC, IP
    uint8_t gw[4]  = {192, 168, 3, 16};
    uint8_t sn[4]  = {255, 255, 255, 0};
    uint8_t mac[6] = {0x00, 0x08, 0xDC, 0x01, 0x02, 0x03};
    uint8_t ip[4]  = {192, 168, 3, 2};

    Write_W5500(0x0001, 0x04, gw, 4);
    Write_W5500(0x0005, 0x04, sn, 4);
    Write_W5500(0x0009, 0x04, mac, 6);
    Write_W5500(0x000F, 0x04, ip, 4);
}