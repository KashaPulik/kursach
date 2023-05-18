#pragma once

#include <codes.h>

#include <stdint.h>
#include <stdio.h>

uint64_t
setbits(uint8_t* bit_arr, uint64_t offs, uint16_t value, uint8_t value_len);
uint8_t getbit(uint8_t* bit_arr, uint64_t offs);
size_t bit_arr_size(Codes* a, char* message);
void init_bit_arr(uint8_t* bit_arr, size_t size);