
#include "../include/memory_spi.h"

#define SPI_INSTANCE  0 /**< SPI instance index. */
static const nrf_drv_spi_t spi = NRF_DRV_SPI_INSTANCE(SPI_INSTANCE);  /**< SPI instance. */
static volatile bool spi_xfer_done;  /**< Flag used to indicate that SPI instance completed the transfer. */

#define WAIT_FOR_TRANSFER while(!spi_xfer_done){sd_app_evt_wait();}

static uint8_t tx_buffer[256];
static uint8_t rx_buffer[256];

static void spi_event_handler(nrf_drv_spi_evt_t const * p_event,
                       void *                    p_context)
{
    spi_xfer_done = true;
    // if(p_event->data.done.rx_length>0){
    // }
}


uint32_t spi_transceive(uint8_t * tx_buf, uint8_t tx_length, \
                        uint8_t * rx_buf, uint8_t rx_length){
    memcpy(tx_buffer, tx_buf, tx_length);
    memset(rx_buffer, 0, rx_length);
    spi_xfer_done = false;
    ERROR_CHECK(nrf_drv_spi_transfer(&spi, (uint8_t const *)tx_buffer, tx_length,  rx_buffer , rx_length));
    WAIT_FOR_TRANSFER;
    memcpy(rx_buf, rx_buffer, rx_length);
    return MEM_SUCCESS;
}

uint8_t read_reg(uint8_t regtr){
    memset(rx_buffer, 0, 2);
    memset(tx_buffer, 0, 1);
    spi_xfer_done = false;
    tx_buffer[0] = regtr;
    ERROR_CHECK(nrf_drv_spi_transfer(&spi, tx_buffer, 1,  rx_buffer , 2));
    WAIT_FOR_TRANSFER;
    return rx_buffer[1];
}

void spi_write(unsigned char value){

    tx_buffer[0] = value;
    spi_xfer_done = false;
    ERROR_CHECK(nrf_drv_spi_transfer(&spi, tx_buffer, 1, NULL, NULL));
    WAIT_FOR_TRANSFER;
}

static void spi_read(uint8_t length){
    memcpy(rx_buffer, NULL, length);
    spi_xfer_done = false;
    ERROR_CHECK(nrf_drv_spi_transfer(&spi, NULL, NULL, rx_buffer, length));
    WAIT_FOR_TRANSFER;
}

uint32_t memory_pins_init(){

    nrf_gpio_cfg_output(FLASH_RST);
    nrf_drv_spi_config_t spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;
    spi_config.ss_pin   = FLASH_CS;
    spi_config.miso_pin = FLASH_MISO;
    spi_config.mosi_pin = FLASH_MOSI;
    spi_config.sck_pin  = FLASH_SCK;
    spi_config.mode  = NRF_DRV_SPI_MODE_0;
    spi_config.frequency = NRF_DRV_SPI_FREQ_8M;
    NRF_GPIO->PIN_CNF[FLASH_SCK] = 0x00000301;
    return nrf_drv_spi_init(&spi, &spi_config, spi_event_handler, NULL);
}

uint32_t memory_readchipid(uint8_t value){
    tx_buffer[0] = value;
    ERROR_CHECK(nrf_drv_spi_transfer(&spi, tx_buffer,1,rx_buffer,4));
    WAIT_FOR_TRANSFER;
    if(rx_buffer[1] == 0xEF){ //chip id read
        return MEM_SUCCESS;
    }
    else{
        return NRF_ERROR_INTERNAL;
    }
}


