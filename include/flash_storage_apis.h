

#ifndef __FLASH_APIx__
#define __FLASH_APIx__

#include <stdlib.h>
#include <stdint.h>
#include "flash_lib.h"


typedef enum{
    MEM_SUCCESS,
    MEM_FAILURE,
    MEM_NOT_AVAILABLE
}MEMORY_ERR_CODES;

typedef struct{
    int sensor_value1;
    int sensor_value2;
    int sensor_value3;
    int sensor_value4;
    int sensor_value5;
    int time_stamp;
}app_structure;

uint32_t store_record(app_structure *);

uint32_t pull_records_fast(app_structure * queue_data, void (* data_fetch_callback)(MEMORY_ERR_CODES, app_structure *)){

uint32_t memory_bank_init();

uint32_t memory_bank_commit();

uint32_t store_recordAtTop(app_structure *);

uint32_t get_top_record(app_structure *);

uint32_t get_bottom_record(app_structure *);

#endif
