#include "stm32g4xx_ll_bus.h"
#include "stm32g4xx_ll_rcc.h"
#include "stm32g4xx_ll_utils.h"
#include "stm32g4xx_ll_spi.h"
#include "stm32g4xx_ll_gpio.h"

#include <stdint.h>
#include <stdio.h>

//typedef union { 
//    uint8_t byte_value;
//    struct {
//        uint8_t BSB  : 5;  
//        uint8_t RWB  : 1;    
//        uint8_t OM   : 2;    
//    } bits; 	
//} Control_Byte;

//typedef struct __attribute__((packed)) {
//    uint8_t Addr_MSB; 
//    uint8_t Addr_LSB;
//    Control_Byte Control;  
//    uint8_t Data;                 
//} SPI_DATA_Frame;



void SPI2_Init(void);
void Write_W5500(uint16_t address, uint8_t bsb, uint8_t*data, uint16_t len); 
void Read_W5500(uint16_t address, uint8_t bsb, uint8_t *data, uint16_t len);
void W5500_Init(void);
