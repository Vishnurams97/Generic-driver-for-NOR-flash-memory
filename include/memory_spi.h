

#ifndef __FLASH_SPI__
#define __FLASH_SPI__

#include <stdlib.h>
#include <stdint.h>
#include "nrf_drv_spi.h"
#include "nrf_gpio.h"

#define FLASH_MISO NRF_GPIO_PIN_MAP(0, 26)
#define FLASH_MOSI NRF_GPIO_PIN_MAP(0, 18)
#define FLASH_SCK  NRF_GPIO_PIN_MAP(0, 13)
#define FLASH_RST  NRF_GPIO_PIN_MAP(0, 3)
#define FLASH_CS   NRF_GPIO_PIN_MAP(0, 7)

uint32_t memory_pins_init();

void spi_write(unsigned char value);

uint8_t read_reg(uint8_t regtr);

uint32_t spi_transceive(uint8_t * tx_buf, uint8_t tx_length, \
                        uint8_t * rx_buf, uint8_t rx_length);

uint32_t memory_readchipid(uint8_t);

#endif
