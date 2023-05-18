#include <codes.h>

Codes* copy_codes(Codes* dest, Codes* src)
{
    for (int i = 1; i < 127; i++) {
        dest[i].code = src[i].code;
        dest[i].len = src[i].len;
    }
    return dest;
}

Codes* clear(Codes* a, uint16_t value, uint8_t value_len)
{
    uint16_t tmp_code;

    for (int i = 1; i < 127; i++) {
        if (a[i].len == 0)
            continue;
        tmp_code = a[i].code >> (a[i].len - value_len);
        if (tmp_code != value)
            a[i].len = 0;
    }
    return a;
}

_Bool find_code(Codes* tmp, uint16_t value, uint8_t value_len, uint8_t s)
{
    if (tmp[s].code == value && tmp[s].len == value_len)
        return 1;
    return 0;
}

_Bool empty_codes(Codes* tmp)
{
    for (int i = 1; i < 127; i++)
        if (tmp[i].len != 0)
            return 0;
    return 1;
}