#include "grains.h"
uint64_t square(uint8_t index){
    if (index < 1 || index > 64) return 0; // not valid option
    return (uint64_t) 1<<(index -1)  ;
}
uint64_t total(void){
    uint64_t sum = 0;
    for (uint8_t i =1; i <= 64 ; i++ ){ /* here the uint8_t not uint64_t becouse we want to ocunt from 1 to 64 only. */
        sum += square(i);
    }
    return sum;
}
