
#ifndef __FLASH_LIB__
#define __FLASH_LIB__

#include <stdlib.h>
#include <stdint.h>
#include "coaster_def.h"
#include "app_timer.h"
#include "nrf_drv_spi.h"

#define WRITE_ENABLE               0x06
#define WRITE_DISABLE              0x04
#define CHIP_ERASE                 0xc7
#define READ_STATUS_REG_1          0x05
#define READ_STATUS_REG_2          0x15
#define READ_STATUS_REG_3          0x35
#define WRITE_STATUS_REG_1         0x01
#define WRITE_STATUS_REG_2         0x31
#define WRITE_STATUS_REG_3         0x11
#define READ_DATA                  0x03
#define FAST_READ_DATA             0x0B
#define FAST_READ                  0x03
#define PAGE_PROGRAM               0x02
#define JEDEC_ID                   0x9F
#define SECTOR_ERASE               0x20
#define BLOCK_ERASE                0x52
#define POWER_DOWN                 0xB9
#define RELEASE_POWER_DOWN         0xAB

//cache storage info
#define META_DATA_SECTOR 0x7FD000
#define CACHE_STORE_PAGE_PRI 0x7FD000
#define CWR_STORE_PAGE_PRI 0x7FD100
#define CACHE_STORE_PAGE_SEC 0x7FD200
#define CWR_STORE_PAGE_SEC 0x7FD300

// Can be set based on memory bank capacity and user requirements
#define PAGE_START 0x010000
#define PAGE_END 0x7FC000
#define PAGE_SIZE 256
#define SECTOR_START 0x010000
#define SECTOR_END 0x7FC000
#define SECTOR_SIZE 4096
#define BLOCK_SIZE 65536

typedef struct{    
    bool bank_empty;
    char check[3];
    uint32_t written_page;
    uint32_t fifo_read_page;
    uint32_t no_of_records_stored;
}flash_status_cache;

uint32_t fifo_write(queue_snos *);

uint32_t fifo_read(queue_snos *);

uint32_t exflash_init();

uint32_t exflash_chip_erase();

uint32_t exflash_chip_clean_up();

void exflash_write_enable();

void exflash_write_disable();

uint32_t exflash_getchipid();

uint8_t exflash_getstatus_reg1();

uint8_t exflash_getstatus_reg2();

uint8_t exflash_getstatus_reg3();

void exflash_power_down();

void exflash_power_up();

void exflash_isbusy();

void exflash_hard_reset();

bool is_sector_tobe_erased();

bool is_sector_already_written();

uint32_t exflash_write_sector(uint32_t sector_num, uint8_t * data, uint32_t length);

uint32_t exflash_write_block(uint32_t block_num, uint8_t * data, uint32_t length);

uint32_t exflash_erase_sector(uint32_t sector_num, uint32_t length); 

uint32_t exflash_erase_block(uint32_t block_num, uint32_t length); 

uint32_t exflash_write_page(uint32_t page_num, uint8_t * data, int length);

void exflash_read_page(uint32_t, uint8_t *, int length);

uint32_t exflash_getSectorAddress(int number);

uint32_t exflash_deinit();

void exflash_iswrite_enabled();

uint32_t exflash_cache_commit();

uint32_t exflash_cache_fetch();

#endif
