
#include "flash_lib.h"
#include "../include/memory_spi.h"
#include "nrf_delay.h"

static flash_status_cache flash_sts_cache;

uint32_t fifo_write(queue_snos * sno){
    ret_code_t err;
    if(flash_sts_cache.written_page < SECTOR_END){
        flash_sts_cache.bank_empty = false;
        err = exflash_write_page(flash_sts_cache.written_page, (uint8_t *)sno, sizeof(queue_snos));
        flash_sts_cache.written_page += PAGE_SIZE;
        flash_sts_cache.fifo_read_page = flash_sts_cache.written_page - PAGE_SIZE;
        flash_sts_cache.no_of_records_stored += 1;
        exflash_isbusy();
        return err;
    }
    else{
        return MEM_NOT_AVAILABLE;
    }
}

uint32_t fifo_read(queue_snos * sno){
    if(flash_sts_cache.fifo_read_page >= SECTOR_START && flash_sts_cache.no_of_records_stored > 0){
        exflash_read_page(flash_sts_cache.fifo_read_page, (uint8_t *)sno, sizeof(queue_snos));
        flash_sts_cache.fifo_read_page -= PAGE_SIZE;
        flash_sts_cache.no_of_records_stored -= 1;
        return MEM_SUCCESS;
    }
    else{
        flash_sts_cache.bank_empty = true;
        return MEM_NOT_AVAILABLE;
    }
}

bool is_sector_start_address(){
    return ((flash_sts_cache.written_page & 0xfff000) == (flash_sts_cache.written_page));
}

void cache_operation(){
    queue_snos * cache_que = (queue_snos * )malloc(sizeof(queue_snos)*16);
    queue_snos * cache_que_dummy = cache_que;
    int records = 0;
    flash_sts_cache.written_page-=PAGE_SIZE;
    while(1){
        records++;
        exflash_read_page(flash_sts_cache.written_page, (uint8_t *)cache_que, sizeof(queue_snos));
        cache_que++;
        flash_sts_cache.written_page -= PAGE_SIZE;
        if(is_sector_start_address()){
            records++;
            exflash_read_page(flash_sts_cache.written_page, (uint8_t *)cache_que, sizeof(queue_snos));
            break;
        }
    }
    exflash_erase_sector(flash_sts_cache.written_page, 1);
    while(records-- && !(flash_sts_cache.written_page-PAGE_SIZE == flash_sts_cache.fifo_read_page)){
        exflash_write_page(flash_sts_cache.written_page, (uint8_t *)cache_que, sizeof(queue_snos));
        flash_sts_cache.written_page += PAGE_SIZE;
        cache_que-=1;
    }
    free(cache_que_dummy);
}

uint32_t exflash_chip_clean_up(){
    if((flash_sts_cache.written_page - PAGE_SIZE) == flash_sts_cache.fifo_read_page){
        LOG("\nNothing to erase");
    }
    else if(((flash_sts_cache.written_page - PAGE_SIZE) & 0xfff000) == (flash_sts_cache.fifo_read_page & 0xfff000)){
        cache_operation();
    }
    else{
        while(1){
            flash_sts_cache.written_page = flash_sts_cache.written_page & 0xfff000;
            if((flash_sts_cache.written_page & 0xfff000) == (flash_sts_cache.fifo_read_page & 0xfff000)){
                flash_sts_cache.written_page += SECTOR_SIZE;
                break;
            }
            exflash_erase_sector(flash_sts_cache.written_page, 1);
            if(flash_sts_cache.written_page - PAGE_SIZE == flash_sts_cache.fifo_read_page){
                return 0;
            }
            flash_sts_cache.written_page -= SECTOR_SIZE;
        }
        cache_operation();
    }
}

uint32_t exflash_init(){
    flash_sts_cache.written_page = PAGE_START;
    flash_sts_cache.fifo_read_page = flash_sts_cache.written_page - PAGE_SIZE;
    flash_sts_cache.no_of_records_stored = NULL;
    flash_sts_cache.bank_empty = true;
    flash_sts_cache.check[0] = '+';
    flash_sts_cache.check[1] = '-';
    flash_sts_cache.check[2] = '+';
    return memory_pins_init();
}

uint32_t exflash_chip_erase(){
    spi_write(WRITE_ENABLE);
    exflash_iswrite_enabled();
    spi_write(CHIP_ERASE);
    exflash_isbusy();
    return MEM_SUCCESS;
}

uint32_t exflash_getchipid(){
    return memory_readchipid(JEDEC_ID);
}

uint8_t exflash_getstatus_reg1(){
    return read_reg(READ_STATUS_REG_1);
}

uint8_t exflash_getstatus_reg2(){
    return read_reg(READ_STATUS_REG_2);
}

uint8_t exflash_getstatus_reg3(){
    return read_reg(READ_STATUS_REG_3);
}

void exflash_hard_reset(){
    nrf_gpio_pin_clear(FLASH_RST);
    nrf_delay_us(30);  
    nrf_gpio_pin_set(FLASH_RST);
    nrf_delay_us(5);  //as per AC characteristics
}

void exflash_power_down(){
    spi_write(POWER_DOWN);
    nrf_delay_us(100);
}

void exflash_power_up(){
    spi_write(RELEASE_POWER_DOWN);
    nrf_delay_us(100);
}

void exflash_isbusy(){
    while (exflash_getstatus_reg1() & 0x01) {nrf_delay_us(50);};
}

void exflash_iswrite_enabled(){uint8_t value;
    while (1) {value = exflash_getstatus_reg1();
    if(value == 0x02){break;}nrf_delay_us(50);};
}

uint32_t exflash_write_sector(uint32_t sector_num, uint8_t * data, uint32_t length){
    // implement multiple page write or entire sector write by sending 4096 bytes
}

uint32_t exflash_write_block(uint32_t block_num, uint8_t * data, uint32_t length){
    // implement multiple sector write or entire block write by sending 65536 bytes
}

uint32_t exflash_erase_sector(uint32_t sector_num, uint32_t no_of_secs){
    exflash_isbusy();
    spi_write(WRITE_ENABLE);
    exflash_iswrite_enabled();
    uint8_t * tx_ptr = (uint8_t *)malloc(4);
    tx_ptr[0] = SECTOR_ERASE;
    tx_ptr[1] = sector_num >> 16;
    tx_ptr[2] = sector_num >> 8;
    tx_ptr[3] = sector_num;
    LOG("\nErasing sector at 0x%02x %02x %02x", tx_ptr[1],tx_ptr[2],tx_ptr[3]);
    spi_transceive(tx_ptr, 4, NULL, NULL);
    exflash_isbusy();
    free(tx_ptr);
    return MEM_SUCCESS;
}

uint32_t exflash_erase_block(uint32_t block_num, uint32_t length){
    exflash_isbusy();
    spi_write(WRITE_ENABLE);
    exflash_iswrite_enabled();
    uint8_t * tx_ptr = (uint8_t *)malloc(4);
    tx_ptr[0] = BLOCK_ERASE;
    tx_ptr[1] = sector_num >> 16;
    tx_ptr[2] = sector_num >> 8;
    tx_ptr[3] = sector_num;
    LOG("\nErasing block at 0x%02x %02x %02x", tx_ptr[1],tx_ptr[2],tx_ptr[3]);
    spi_transceive(tx_ptr, 4, NULL, NULL);
    exflash_isbusy();
    free(tx_ptr);
    return MEM_SUCCESS;
}

uint32_t exflash_write_page(uint32_t page_num, uint8_t * data, int length){
    exflash_isbusy();
    spi_write(WRITE_ENABLE);
    exflash_iswrite_enabled();
    exflash_isbusy();
    uint8_t * send_data = (uint8_t *)malloc(length + 4);
    send_data[0] = PAGE_PROGRAM;
    send_data[1] = page_num >> 16;
    send_data[2] = page_num >> 8;
    send_data[3] = page_num;
    LOG("\nWriting at 0x%02x %02x %02x", send_data[1],send_data[2],send_data[3]);
    for(int index = 4;index<length+4;index++){
        send_data[index] = data[index - 4];
    }
    ret_code_t err = spi_transceive(send_data, length + 4, NULL, NULL);
    exflash_isbusy();
    spi_write(WRITE_DISABLE);
    free(send_data);
    return err;
}

void exflash_read_page(uint32_t page_num, uint8_t * rx_buffer, int length){
    exflash_isbusy();
    uint8_t * rx_ptr = (uint8_t *)malloc(length + 4);
    rx_ptr[0] = READ_DATA;
    rx_ptr[1] = page_num>>16;
    rx_ptr[2] = page_num>>8;
    rx_ptr[3] = page_num;
    LOG("\nReading at 0x%02x %02x %02x", rx_ptr[1],rx_ptr[2],rx_ptr[3]);
    spi_transceive(rx_ptr, 4 , rx_ptr, length + 4);
    memcpy(rx_buffer, rx_ptr + 4, length);
    free(rx_ptr);
}

uint32_t exflash_getSectorAddress(int number){
    return (number & 0xfff000);
}

uint32_t exflash_deinit(){
    //spi deinit
}

uint32_t exflash_cache_commit(){
    exflash_erase_sector(META_DATA_SECTOR, 1);
    return exflash_write_page(CACHE_STORE_PAGE_PRI, (uint8_t * )&flash_sts_cache, \
                        sizeof(flash_status_cache));
}

uint32_t exflash_cache_fetch(){
    flash_status_cache  * fsc = (flash_status_cache *)malloc(sizeof(flash_status_cache));
    if(fsc == NULL){return MEM_FAILURE;}
    exflash_read_page(CACHE_STORE_PAGE_PRI, (uint8_t *)fsc, sizeof(flash_status_cache));
    if(strcmp(fsc->check, "+-+") == 0){
        memcpy(&flash_sts_cache, fsc, sizeof(flash_status_cache)); // simple check whether the previous 
                                                                   // cache is stored
    }
    free(fsc);
    return MEM_SUCCESS;
}

