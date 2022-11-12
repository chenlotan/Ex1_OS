#include <stddef.h>
#include <inttypes.h>

#include "os.h"


/***return pointer to the node of this VPN if exists, else return -1***/
void* search_vpn(uint64_t pt, uint64_t vpn);
void add_to_pt(uint64_t pt, uint64_t vpn, uint64_t ppn);


void page_table_update(uint64_t pt, uint64_t vpn, uint64_t ppn){
    uint64_t *p = search_vpn(pt, vpn);
    if (ppn == NO_MAPPING){
        if ((p != NULL) && (*p % 2 == 1)){
            *p = *p - 1;
        }
    } else{
        if (p != NULL){
            *p = (ppn << 12) + 1;
        } else{
            add_to_pt(pt, vpn, ppn);
        }
    }
}

uint64_t page_table_query(uint64_t pt, uint64_t vpn){
    uint64_t* res = search_vpn(pt, vpn);
    if (res != NULL){
        if (*res % 2 == 1){ // if valid = 1
            return (*res - 1) >> 12;
        }
        return NO_MAPPING;
    }
    return NO_MAPPING;
}

void* search_vpn(uint64_t pt, uint64_t vpn){
    uint64_t* curr_ad = (uint64_t*)phys_to_virt(pt << 12);
    uint64_t index;
    uint64_t mask = (1 << 9) - 1;

    for (int i = 0; i < 4; i++){
        index = vpn >> 9*(4-i); //VERIFY!!!
        index = index & mask;
        if (curr_ad[index] == 0) {
            return NULL;
        }
        curr_ad = (uint64_t*)phys_to_virt(curr_ad[index]);
    }
    index = vpn & mask;
    return curr_ad + index;
}

void add_to_pt(uint64_t pt, uint64_t vpn, uint64_t ppn){
    uint64_t* curr_ad =  (uint64_t*)phys_to_virt(pt << 12);
    uint64_t index;
    uint64_t mask = (1 << 9) - 1;
    int i = 0;

    while (i < 4){
        index = vpn >> 9*(4-i); //VERIFY!!!
        index = index & mask;
        if (curr_ad[index] == 0) {
            curr_ad[index] = alloc_page_frame() << 12;
        }
        curr_ad = (uint64_t*)phys_to_virt(curr_ad[index]);
        i++;
    }
    index = vpn & mask;
    curr_ad[index] = (ppn << 12) + 1;

}





