#include "SPI.h"

//SPI_DATA_Frame SPI_packed;

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
    
    /* โค้ดบน */
GPIO_InitStruct.Pin = LL_GPIO_PIN_1 | LL_GPIO_PIN_2; // รวบ 2 ขา
GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
GPIO_InitStruct.Alternate = 0; 
LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_1);
    
    /* USER CODE BEGIN SPI2_Init 1 */
    /* USER CODE END SPI2_Init 1 */
    
    SPI_InitStruct.TransferDirection = LL_SPI_FULL_DUPLEX; 
    SPI_InitStruct.Mode = LL_SPI_MODE_MASTER; 
    SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_8BIT; 
    SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_LOW; 
    SPI_InitStruct.ClockPhase = LL_SPI_PHASE_1EDGE; 
    SPI_InitStruct.NSS = LL_SPI_NSS_SOFT; 
    SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV8; 
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

void Write_W5500(uint16_t address, uint8_t bsb, uint8_t *data, uint16_t len)
{
    uint8_t spi_fsm = 0;
    
    // สร้าง Header 3 ไบต์ (Address MSB, Address LSB, Control Byte)
    uint8_t header[3];
    header[0] = (uint8_t)(address >> 8);
    header[1] = (uint8_t)address;
    header[2] = bsb;
    
    uint8_t header_idx = 0;           // ตัวนับตำแหน่ง Header
    uint16_t total_len = 3 + len;     // จำนวนไบต์ทั้งหมดที่ต้องส่ง (Header 3 + ข้อมูล len)

    LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_1); // CS LOW เริ่มการสื่อสาร

    do {
        switch(spi_fsm) {
            case 0: // State 0: ส่งข้อมูล (Transmit Phase)
            {
                if (header_idx < 3) {
                    // 3 ไบต์แรกให้ดึงจากอาร์เรย์ header
                    LL_SPI_TransmitData8(SPI2, header[header_idx++]);
                } else {
                    // ไบต์ที่เหลือให้ดึงจากอาร์เรย์ data
                    LL_SPI_TransmitData8(SPI2, *data++);
                }
                spi_fsm = 1; // ย้ายไป State รอรับข้อมูล
            }
            break;

            case 1: // State 1: รอข้อมูลไหลกลับมา (Wait RX)
            {
                // เมื่อรับข้อมูลได้ครบ 1 ไบต์ (แฟล็ก RXNE เด้ง)
                if (LL_SPI_IsActiveFlag_RXNE(SPI2) != 0) {
                    spi_fsm = 2; // ย้ายไป State อ่านข้อมูล
                }
            }
            break;
            
            case 2: // State 2: อ่านข้อมูลทิ้ง (Read Dummy & Update Length)
            {
                // อ่านข้อมูลขยะออกจาก DR เพื่อล้างแฟล็ก RXNE
                (void)LL_SPI_ReceiveData8(SPI2); 
                
                total_len--; // ลดจำนวนไบต์ที่ต้องทำลง 1
                spi_fsm = 0; // วนกลับไป State ส่งข้อมูลใหม่
            }
            break;
            
            default:
                spi_fsm = 0;
                total_len = 0; // บังคับจบการทำงานหาก State ผิดพลาด
                break;
        }
        
    } while(total_len > 0);
    
    LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_1); // CS HIGH จบการสื่อสาร
}

void Read_W5500(uint16_t address, uint8_t bsb, uint8_t *data, uint16_t len)
{
    uint8_t spi_fsm = 0;
    
    // โหมด Read ต้องให้บิต RWB เป็น 0 (shift bsb ไปซ้าย 3 บิต แล้ว OR ด้วย 0x00)
    uint8_t control_byte = (bsb << 3) | 0x00; 
    
    uint8_t header[3];
    header[0] = (uint8_t)(address >> 8);
    header[1] = (uint8_t)address;
    header[2] = control_byte;
    
    uint8_t header_idx = 0;
    uint16_t total_len = 3 + len; // 3 ไบต์ส่ง Header + len ไบต์รับ Data

    LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_1); // CS LOW
    
    do {
        switch(spi_fsm) {
            case 0: // State 0: ส่งข้อมูล
            {
                if (header_idx < 3) {
                    LL_SPI_TransmitData8(SPI2, header[header_idx++]); // ส่ง Header 3 ไบต์
                } else {
                    LL_SPI_TransmitData8(SPI2, 0xFF); // ส่ง Dummy Data เพื่อผลักข้อมูล W5500 ออกมา
                }
                spi_fsm = 1;
            }
            break;

            case 1: // State 1: รอรับข้อมูลกลับ
            {
                if (LL_SPI_IsActiveFlag_RXNE(SPI2) != 0) {
                    spi_fsm = 2;
                }
            }
            break;
            
            case 2: // State 2: จัดการข้อมูลที่ได้รับ
            {
                uint8_t rx_data = LL_SPI_ReceiveData8(SPI2);
                
                // ถ้าจำนวนที่เหลืออยู่น้อยกว่าหรือเท่ากับ len แสดงว่าพ้นช่วง Header แล้ว
                if (total_len <= len) { 
                    if (data != NULL) {
                        *data++ = rx_data; // เก็บข้อมูลลง Buffer ของผู้ใช้
                    }
                }
                
                total_len--;
                spi_fsm = 0;
            }
            break;
        }
        
    } while(total_len > 0);
    
    LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_1); // CS HIGH
}

void W5500_Init(void){ 
    /* 1. Hardware Reset ให้ W5500 (ใช้ขา PB2) */
    LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_2); // ดึง RST ลง LOW
    LL_mDelay(10);                                // รอ 10ms
    LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_2);   // ดึง RST กลับเป็น HIGH
    LL_mDelay(100);                               // รอให้ W5500 ตั้งสติ 100ms

    /* 2. Software Reset (โหมด VDM 0x04) */ 
    uint8_t reset_cmd = 0x80;  // ค่า 0x80 คือคำสั่ง Software Reset 
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

//void W5500_Init(void){
//    /* 1. Reset ฮาร์ดแวร์ก่อนเสมอ (ถ้าไม่ทำ W5500 จะเอ๋อ) */
//    LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_2); // ดึงขารีเซ็ตลง LOW
//    LL_mDelay(10);
//    LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_2);   // ดึงขารีเซ็ตขึ้น HIGH
//    LL_mDelay(100); // ต้องรออย่างน้อย 100ms ให้มันตั้งสติ

//    /* 2. สั่ง Software Reset อย่างเดียว (0x80) */
//    // สำคัญ: ต้องเป็น 0x80 เท่านั้น! ห้ามเป็น 0x90 เด็ดขาด! (0x90 = Block Ping)
//    uint8_t reset_cmd = 0x80;
//    Write_W5500(0x0000, 0x04, &reset_cmd, 1);
//    LL_mDelay(50); // รอให้มัน Reset ตัวเองเสร็จ

//    /* 3. ตั้งค่า Network */
//    uint8_t gw[4] = {192, 168, 3, 16};
//    Write_W5500(0x0001, 0x04, gw, 4);

//    uint8_t sn[4] = {255, 255, 255, 0};
//    Write_W5500(0x0005, 0x04, sn, 4);

//    uint8_t mac[6] = {0x00, 0x08, 0xDC, 0x01, 0x02, 0x03};
//    Write_W5500(0x0009, 0x04, mac, 6);

//    uint8_t ip[4] = {192, 168, 3, 2};
//    Write_W5500(0x000F, 0x04, ip, 4);
//    
//    // ลองเพิ่มบรรทัดนี้ไปต่อท้าย W5500_Init ครับ!
//    // เป็นการดึงข้อมูลที่เพิ่งเขียนลงไป กลับมาอ่าน เพื่อบังคับให้ฮาร์ดแวร์มัน Apply ค่า
//    uint8_t read_ip[4];
//    Read_W5500(0x000F, 0x00, read_ip, 4); // อ่าน IP กลับมา (แค่หลอกให้อ่าน)
//}
