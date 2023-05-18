#include <codes.h>
#include <bit_arr.h>

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

uint8_t*
ENCODE_MSG(char* message, Codes* a, uint64_t* offs, size_t* size_bit_arr)
{
    uint64_t tmp_offs = 0;
    size_t size = bit_arr_size(a, message);
    uint8_t* bit_arr = malloc(size);
    init_bit_arr(bit_arr, size);
    uint16_t value;
    uint8_t value_len;
    for (int i = 0; message[i] != '\0'; i++) {
        value = a[(uint8_t)message[i]].code;
        value_len = a[(uint8_t)message[i]].len;
        tmp_offs = setbits(bit_arr, tmp_offs, value, value_len);
    }
    *offs = tmp_offs;
    *size_bit_arr = size;
    return bit_arr;
}

char* DECODE_MSG(
        uint8_t* bit_arr,
        Codes* a,
        uint8_t n_last_bits,
        size_t size_bit_arr,
        size_t message_size)
{
    char letter[2] = {0};
    char* message = malloc(message_size + 1);
    for(size_t i = 0; i < message_size + 1; i++)
        message[i] = 0;
    size_t n = size_bit_arr - 1;
    Codes tmp[127];
    copy_codes(tmp, a);
    uint16_t value = 0;
    uint8_t value_len = 0;
    for (size_t i = 0; i < (8 * n + n_last_bits); i++) {
        value = (value << 1) | getbit(bit_arr, i);
        value_len++;
        clear(tmp, value, value_len);
        for (uint8_t s = 1; s < 127; s++) {
            if (find_code(tmp, value, value_len, s)) {
                letter[0] = s;
                message = strcat(message, letter);
                copy_codes(tmp, a);
                value = 0;
                value_len = 0;
                break;
            }
        }
        if (empty_codes(tmp)) {
            printf("Ошибка декодирования, такого кода не существует!\n");
            exit(1);
        }
    }
    return message;
}