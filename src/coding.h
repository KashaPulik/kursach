#pragma once

uint8_t*
ENCODE_MSG(char* message, Codes* a, uint64_t* offs, size_t* size_bit_arr);
char* DECODE_MSG(
        uint8_t* bit_arr,
        Codes* a,
        uint8_t n_last_bits,
        size_t size_bit_arr,
        size_t message_size);