
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "include/flash_storage_apis.h"

void mem_fetch_callback(MEMORY_ERR_CODES code, app_structure * queue_data){
    switch(code){
        case MEM_SUCCESS:{
            LOG_HEX(queue_data);  // application structure data retrieved from memory bank
        }
        break;
        case MEM_FAILURE:{
            // reset memory bank
            //error handling
        }
        break;
        default:
        break;
    }
}

int main(){
    ERROR_CHECK(memory_bank_init());
    app_structure * app_data = (app_structure *)malloc(sizeof(app_structure));;
    store_record(app_data);  // Stores the user application structure at empty locations.
    app_structure * data_from_bank = (app_structure *)malloc(sizeof(app_structure));
    pull_records_fast(data_from_bank, &mem_fetch_callback); // used to fetch all app data from memory
    return 0;
}