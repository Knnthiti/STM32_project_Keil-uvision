#include "stm32g4xx_ll_bus.h"
#include "stm32g4xx_ll_gpio.h"
#include "stm32g4xx_ll_utils.h"
#include "stm32g4xx_ll_spi.h"

#include <stdint.h>
#include <stdio.h>

void SPI2_Init(void);
void Write_W5500(uint16_t address, uint8_t bsb, uint8_t *data, uint16_t len);
void Read_W5500(uint16_t address, uint8_t bsb, uint8_t *data, uint16_t len);
void W5500_Init(void);