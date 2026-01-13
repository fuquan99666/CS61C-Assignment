#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "lfsr.h"
#include "bit_ops.h"
#include "bit_ops.c"

void lfsr_calculate(uint16_t *reg) {
    /* YOUR CODE HERE */
    // 每次右移一位
    // 左边填充的一位是0，2，3，5位的异或结果
    int num_0 = (*reg >> 0) & 1;
    int num_2 = (*reg >> 2) & 1;
    int num_3 = (*reg >> 3) & 1;
    int num_5 = (*reg >> 5) & 1;
    int left = num_0 ^ num_2 ^num_3 ^num_5;
    *reg = (*reg) >> 1;
    set_bit(reg,15,left);
    
}

