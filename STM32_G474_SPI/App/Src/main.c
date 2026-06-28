#include "main.h"
#include <stdio.h>

volatile uint8_t w5500_version = 0;

// ---------------------------------------------------------
// ฟังก์ชันจัดการ TCP Client (STM32 เป็นฝ่ายวิ่งไปหาคอมพิวเตอร์)
// ---------------------------------------------------------
void W5500_TCP_Client(void) {
    uint8_t sn_sr, cmd;
    uint8_t val[4];
  //  static uint32_t message_count = 0; // ประกาศให้เป็น uint32_t (รองรับได้ถึง 4 พันล้าน)

    // อ่าน "สถานะปัจจุบัน" ของ Socket 0
    Read_W5500(0x0003, 0x01, &sn_sr, 1);

    switch (sn_sr) {
        
        case 0x00: // สถานะ SOCK_CLOSED (ปิดอยู่)
            // 1. เปิด Socket เป็นโหมด TCP (0x01)
            cmd = 0x01;
            Write_W5500(0x0000, 0x0C, &cmd, 1); // Sn_MR

            // 2. ตั้งค่า Source Port (พอร์ตของฝั่ง STM32 เอง เช่น 50000 = 0xC350)
            val[0] = 0xC3; val[1] = 0x50;
            Write_W5500(0x0004, 0x0C, val, 2); // Sn_PORT

            // 3. สั่ง OPEN Socket (0x01)
            cmd = 0x01;
            Write_W5500(0x0001, 0x0C, &cmd, 1); // Sn_CR
            break;

        case 0x13: // สถานะ SOCK_INIT (เปิดพอร์ตตัวเองสำเร็จแล้ว)
            // 4. ตั้งค่า IP ปลายทาง (IP ของคอมพิวเตอร์คุณ คือ 192.168.3.10)
            val[0] = 192; val[1] = 168; val[2] = 3; val[3] = 10;
            Write_W5500(0x000C, 0x0C, val, 4); // Sn_DIPR

            // 5. ตั้งค่า Port ปลายทาง (Port ของ Python Server คือ 5000 = 0x1388)
            val[0] = 0x13; val[1] = 0x88;
            Write_W5500(0x0010, 0x0C, val, 2); // Sn_DPORT

            // 6. สั่ง CONNECT (0x04) วิ่งไปเคาะประตู!
            cmd = 0x04;
            Write_W5500(0x0001, 0x0C, &cmd, 1); // Sn_CR
            break;

        case 0x17: // สถานะ SOCK_ESTABLISHED (🎉 เชื่อมต่อกับคอมสำเร็จแล้ว!)
        {
            // === สร้าง Scope ย่อยด้วยการใส่ปีกกา { ... } เพื่อแก้ Warning ===
            uint8_t tx_buffer[30] = "Hello STM32"; 
    uint16_t send_len = 0;
    uint16_t ptr = 0;
    
    // หาความยาวข้อความ
    while(tx_buffer[send_len] != '\0') {
        send_len++; 
    }

    // 8. หาตำแหน่ง Pointer สำหรับเขียนข้อมูล (Sn_TX_WR)
            Read_W5500(0x0024, 0x01, val, 2);
            ptr = (val[0] << 8) | val[1];

            // 9. เขียนข้อมูลลง TX Buffer
            // ⚠️ แก้พารามิเตอร์จาก 0x02 ให้เป็น 0x14 (Control Byte ที่ถูกต้องสำหรับเขียนลง TX)
            Write_W5500(ptr, 0x14, (uint8_t*)tx_buffer, send_len); 

            // 10. เลื่อน Pointer ไปข้างหน้า
            ptr += send_len;
            val[0] = (uint8_t)(ptr >> 8);
            val[1] = (uint8_t)(ptr & 0xFF);
            Write_W5500(0x0024, 0x0C, val, 2); // อัปเดต Sn_TX_WR

            // 11. สั่ง SEND (0x20) ยิงข้อมูลออกไป!
            cmd = 0x20; 
            Write_W5500(0x0001, 0x0C, &cmd, 1); // Sn_CR
            
            // หน่วงเวลา 1 วินาที เพื่อไม่ให้ยิงรัวเกินไป
            LL_mDelay(1000); 
            break;
        } // จบ Scope ย่อย

        case 0x1C: // สถานะ SOCK_CLOSE_WAIT (คอมพิวเตอร์ตัดสาย)
        case 0x14: // สถานะ SOCK_LISTEN (ถ้าเผลอเข้าโหมดผิด)
            // สั่ง DISCON (0x08) เตะออกเพื่อวนไปเริ่มใหม่
            cmd = 0x08;
            Write_W5500(0x0001, 0x0C, &cmd, 1); // Sn_CR
            break;
    }
}

int main(void) 
{ 
    // 1. Initialize system clock 
    Clock_system();

    SPI2_Init();
LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_2); // ดึง RST ลง LOW
    LL_mDelay(10);                                // รอ 10 มิลลิวินาที
    LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_2);   // ดึง RST กลับเป็น HIGH
    LL_mDelay(100);                               // รอให้ W5500 บูตระบบ 100 มิลลิวินาที

    // === วิธีใช้ Read_W5500 เพื่ออ่านค่า 1 ไบต์ ===
    uint8_t w5500_version = 0; // 1. สร้างตัวแปรมารอรับค่า
    
    // 2. เรียกใช้ฟังก์ชัน
    // Address = 0x0039, BSB = 0x00, Pointer = &w5500_version, Length = 1
    Read_W5500(0x0039, 0x00, &w5500_version, 1); 
    
    // 3. นำค่ามาเช็คเหมือนเดิม
    if (w5500_version == 0x04) 
    {
        // 🎉 SPI ทำงานถูกต้อง 100%
        W5500_Init(); 
    }

    while (1)
    {
        W5500_TCP_Client();
    }
}