#include "main.h"

volatile uint8_t w5500_version = 0;

int main(void) 
{ 
    // 1. Initialize system clock 
    Clock_system();

    SPI2_Init();
//    W5500_Init();
		
		w5500_version = Read_W5500_1Byte(0x0039, 0x01);
    while (1) 
    { 
        
    } 
}