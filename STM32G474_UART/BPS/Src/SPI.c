#include "SPI.h"

SPI_DATA_Frame SPI_packed;

void SPI2_Init(void) {
    /* USER CODE BEGIN SPI2_Init 0 */
    /* USER CODE END SPI2_Init 0 */
    LL_SPI_InitTypeDef SPI_InitStruct = {0};
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    /* Peripheral clock enable */ 
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI2);
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB); 
    
    /**SPI2 GPIO Configuration 
    PB13 ------> SPI2_SCK 
    PB14 ------> SPI2_MISO 
    PB15 ------> SPI2_MOSI 
    PB1 ------> SPI2_CS 
    PB2 ------> SPI2_RST */ 
		
    GPIO_InitStruct.Pin = LL_GPIO_PIN_13; 
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE; 
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW; 
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL; 
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO; 
    GPIO_InitStruct.Alternate = LL_GPIO_AF_5; 
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin = LL_GPIO_PIN_14; 
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE; 
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW; 
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL; 
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO; 
    GPIO_InitStruct.Alternate = LL_GPIO_AF_5; 
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin = LL_GPIO_PIN_15; 
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE; 
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW; 
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL; 
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO; 
    GPIO_InitStruct.Alternate = LL_GPIO_AF_5; 
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin = LL_GPIO_PIN_1; 
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT; 
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW; 
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL; 
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO; 
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin = LL_GPIO_PIN_2; 
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT; 
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW; 
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL; 
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO; 
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    
    /* USER CODE BEGIN SPI2_Init 1 */
    /* USER CODE END SPI2_Init 1 */
    
    SPI_InitStruct.TransferDirection = LL_SPI_FULL_DUPLEX; 
    SPI_InitStruct.Mode = LL_SPI_MODE_MASTER; 
    SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_8BIT; 
    SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_LOW; 
    SPI_InitStruct.ClockPhase = LL_SPI_PHASE_1EDGE; 
    SPI_InitStruct.NSS = LL_SPI_NSS_SOFT; 
    SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV4; 
    SPI_InitStruct.BitOrder = LL_SPI_MSB_FIRST; 
    SPI_InitStruct.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE; 
    SPI_InitStruct.CRCPoly = 7;
    
    LL_SPI_Init(SPI2, &SPI_InitStruct);
    LL_SPI_SetStandard(SPI2, LL_SPI_PROTOCOL_MOTOROLA); 
    LL_SPI_EnableNSSPulseMgt(SPI2); 
		
		LL_SPI_SetRxFIFOThreshold(SPI2, LL_SPI_RX_FIFO_TH_QUARTER); 
    LL_SPI_Enable(SPI2);
    
    /* USER CODE BEGIN SPI2_Init 2 */
    /* USER CODE END SPI2_Init 2 */
}

void Write_W5500(uint16_t address, uint8_t control_byte, uint8_t *data, uint16_t len) { 
    // 1. ดึง CS ลง LOW เพื่อเริ่มการสื่อสาร
    LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_1);

    // 2. ส่ง Address ไบต์บน (MSB)
    while (!LL_SPI_IsActiveFlag_TXE(SPI2)) {}
    LL_SPI_TransmitData8(SPI2, (uint8_t)(address >> 8));
//    while (!LL_SPI_IsActiveFlag_RXNE(SPI2)) {}
//    LL_SPI_ReceiveData8(SPI2); // อ่านทิ้ง

    // 3. ส่ง Address ไบต์ล่าง (LSB)
    while (!LL_SPI_IsActiveFlag_TXE(SPI2)) {}
    LL_SPI_TransmitData8(SPI2, (uint8_t)address);
//    while (!LL_SPI_IsActiveFlag_RXNE(SPI2)) {}
//    LL_SPI_ReceiveData8(SPI2); // อ่านทิ้ง

    // 4. ส่ง Control Byte
    while (!LL_SPI_IsActiveFlag_TXE(SPI2)) {}
    LL_SPI_TransmitData8(SPI2, control_byte);
//    while (!LL_SPI_IsActiveFlag_RXNE(SPI2)) {}
//    LL_SPI_ReceiveData8(SPI2); // อ่านทิ้ง
    
    // 5. ส่งข้อมูลจริง (Data Phase)
    for (uint16_t i = 0; i < len; i++)
    {
        while (!LL_SPI_IsActiveFlag_TXE(SPI2)) {}
        LL_SPI_TransmitData8(SPI2, data[i]);
        
//        while (!LL_SPI_IsActiveFlag_RXNE(SPI2)) {}
//        LL_SPI_ReceiveData8(SPI2); // อ่านทิ้ง
    }
    
    // 6. ดึง CS ขึ้น HIGH เพื่อจบการสื่อสาร
    LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_1);
}

void W5500_Init(void){ 
    /* 1. Hardware Reset ให้ W5500 (ใช้ขา PB2) */
    LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_2); // ดึง RST ลง LOW
    LL_mDelay(10);                                // รอ 10ms
    LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_2);   // ดึง RST กลับเป็น HIGH
    LL_mDelay(100);                               // รอให้ W5500 ตั้งสติ 100ms

    /* 2. Software Reset (โหมด VDM 0x04) */ 
    uint8_t reset_cmd = 0x90;  // ค่า 0x80 คือคำสั่ง Software Reset 
    Write_W5500(0x0000, 0x04, &reset_cmd, 1);
    LL_mDelay(50);
    
    /* 2. Set Gateway (อ้างอิงจาก Wi-Fi ของคุณ) */
uint8_t gw[4] = {192, 168, 3, 16};
Write_W5500(0x0001, 0x04, gw, 4); 

/* 3. Set Subnet Mask */
uint8_t sn[4] = {255, 255, 255, 0};
Write_W5500(0x0005, 0x04, sn, 4);

/* 4. Set MAC Address */
uint8_t mac[6] = {0x00, 0x08, 0xDC, 0x01, 0x02, 0x03};
Write_W5500(0x0009, 0x04, mac, 6);

/* 5. Set IP Address (ของบอร์ด W5500) */
uint8_t ip[4] = {192, 168, 3, 2};
Write_W5500(0x000F, 0x04, ip, 4);
}

uint8_t Read_W5500_1Byte(uint16_t address, uint8_t control_byte) {
    uint8_t read_data = 0;
    
    LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_1); // CS LOW

    // 1. ส่ง Address MSB
    while (!LL_SPI_IsActiveFlag_TXE(SPI2)) {}
    LL_SPI_TransmitData8(SPI2, (uint8_t)(address >> 8));
//    while (!LL_SPI_IsActiveFlag_RXNE(SPI2)) {}
//    LL_SPI_ReceiveData8(SPI2); 

    // 2. ส่ง Address LSB
    while (!LL_SPI_IsActiveFlag_TXE(SPI2)) {}
    LL_SPI_TransmitData8(SPI2, (uint8_t)address);
//    while (!LL_SPI_IsActiveFlag_RXNE(SPI2)) {}
//    LL_SPI_ReceiveData8(SPI2); 

    // 3. ส่ง Control Byte (สำหรับ Read)
    while (!LL_SPI_IsActiveFlag_TXE(SPI2)) {}
    LL_SPI_TransmitData8(SPI2, control_byte);
//    while (!LL_SPI_IsActiveFlag_RXNE(SPI2)) {}
//    LL_SPI_ReceiveData8(SPI2); 

    // 4. ส่งค่า Dummy (0xFF) เพื่อผลัก Clock ให้อ่านข้อมูลกลับมา
    while (!LL_SPI_IsActiveFlag_TXE(SPI2)) {}
    LL_SPI_TransmitData8(SPI2, 0xFF);
//    while (!LL_SPI_IsActiveFlag_RXNE(SPI2)) {}
//    read_data = LL_SPI_ReceiveData8(SPI2); // ได้ข้อมูลมาแล้ว!

    LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_1); // CS HIGH
    
    return read_data;
}