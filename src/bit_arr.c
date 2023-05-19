#include <codes.h>

#include <stdint.h>
#include <stdio.h>

uint64_t
setbits(uint8_t* bit_arr, uint64_t offs, uint16_t value, uint8_t value_len)
{
    uint64_t byte_n = offs / 8;
    uint8_t byte_offs = offs % 8;
    value = value << (16 - value_len);
    for (int i = value_len; i > 0; i--) {
        bit_arr[byte_n]
                = (bit_arr[byte_n] | ((value & 0x8000) >> (byte_offs + 8)));
        byte_offs++;
        if (byte_offs % 8 == 0) {
            byte_n++;
            byte_offs = 0;
        }
        value = value << 1;
    }
    return offs + value_len;
}

uint8_t getbit(uint8_t* bit_arr, uint64_t offs)
{
    uint64_t byte_n
            = offs / 8;
    uint8_t byte_offs = offs % 8;
    uint8_t mask = 0x80 >> byte_offs;
    if ((bit_arr[byte_n] & mask) == 0)
        return 0;
    else
        return 1;
}

size_t bit_arr_size(Codes* a, char* message)
{
    size_t count = 0;
    for (int i = 0; message[i] != '\0'; i++)
        count += a[(uint8_t)message[i]].len;
    size_t size = count / 8;
    if (count % 8 > 0)
        size++;
    return size;
}

void init_bit_arr(uint8_t* bit_arr, size_t size)
{
    for (size_t i = 0; i < size; i++)
        bit_arr[i] = 0;
}