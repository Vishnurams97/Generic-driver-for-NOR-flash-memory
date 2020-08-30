
#include "../include/flash_storage_apis.h"
#include "nrf_delay.h"

uint32_t store_record(app_structure * app_data){
    if(exflash_getchipid() == MEM_SUCCESS){
        exflash_cache_fetch();  // this function must be called during startup or after reset and 
                                // is optional to use everytime
        store_recordAtTop(app_data);
        memory_bank_commit();   // updates all caches(RAM data) associated with memory bank
                                // must be called for every interaction with memory
    }
    else{
        LOG("\nUnable to communicate with memory bank");
    }
}
#define MAX_APP_PAGES 30000 // can be modified, depends on developer requirements

uint32_t pull_records_fast(app_structure * queue_data, void (* data_fetch_callback)(MEMORY_ERR_CODES, app_structure *)){
//    exflash_hard_reset();
//    exflash_power_up();
//    nrf_delay_ms(1);
    if(exflash_getchipid() == MEM_SUCCESS){
        exflash_cache_fetch();
        for(int index = 0;index<MAX_APP_PAGES;++index){
            get_top_record(queue_data);
            (*data_fetch_callback)(MEM_SUCCESS, queue_data);
        }
        memory_bank_commit();
    }
    else{
        LOG("\nUnable to communicate with memory bank");
    }
//  exflash_power_down();
}

uint32_t memory_bank_init(){
    return exflash_init();
}

uint32_t memory_bank_commit(){
    exflash_chip_clean_up();
    return exflash_cache_commit();;
}

uint32_t store_recordAtTop(app_structure * record){
    return fifo_write(record);
}

uint32_t get_top_record(app_structure * record){
    return fifo_read(record);
}







