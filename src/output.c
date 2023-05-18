#include <codes.h>

#include <stdint.h>
#include <stdio.h>

void print_bin(uint16_t number)
{
    for (int i = 15; i >= 0; i--) {
        printf("%d", (number >> i) & 1);
    }
}

void print_codes(Codes* a)
{
    for (int i = 1; i < 127; i++) {
        if (a[i].len == 0)
            continue;
        printf("a[%c].code = ", i);
        print_bin(a[i].code);
        printf(" len = %d  %d\n", a[i].len, i);
    }
}