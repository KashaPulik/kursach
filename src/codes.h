#pragma once

#include <stdint.h>

typedef struct codes {
    uint16_t code;
    uint8_t len;
} Codes;

Codes* copy_codes(Codes* dest, Codes* src);
Codes* clear(Codes* a, uint16_t value, uint8_t value_len);
_Bool find_code(Codes* tmp, uint16_t value, uint8_t value_len, uint8_t s);
_Bool empty_codes(Codes* tmp);