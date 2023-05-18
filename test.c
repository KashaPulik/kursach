#include "./minheap.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

typedef struct codes {
    uint16_t code;
    uint8_t len;
} Codes;

void print_bin(uint16_t number)
{
    for(int i = 15; i >=0; i--) {
        printf("%d", (number >> i) & 1);
    }
}

void print_codes(Codes* a)
{
    // for (int i = 1; i < 127; i++) {
    //     // printf("a[%d].code = %x | len = %d\n", i, a[i].code, a[i].len);
    //     printf("a[%d].code = ", i);
    //     print_bin(a[i].code);
    //     printf(" len = %d\n", a[i].len);
    // }
    for (int i = 32; i < 124; i++) {
        // printf("a[%d].code = %x | len = %d\n", i, a[i].code, a[i].len);
        if(a[i].len == 0)
            continue;
        printf("a[%c].code = ", i);
        print_bin(a[i].code);
        printf(" len = %d  %d\n", a[i].len, i);
    }
}

uint8_t get_letter_index(uint8_t letter)
{
    return letter - 97;
}

uint8_t get_letter(uint8_t index)
{
    return index + 97;
}

// uint64_t setbits(uint8_t* C, uint64_t offs, uint16_t value, uint8_t value_len)
// {
//     uint32_t byte_n
//             = offs / 8; // Номер байта в который будет происходить запись
//     uint8_t byte_offs = offs % 8; // Смещение внутри этого байта
//     uint8_t n_free_bytes
//             = 8 - byte_offs; // Количество свободных бит в крайнем байте
//     uint8_t tmp_value_1 = value
//             << (8 - value_len); // Биты, которые будут записаны
//     tmp_value_1 = tmp_value_1
//             >> byte_offs; // Подгонка позиции битов под место в массиве
//     C[byte_n] = C[byte_n] >> (n_free_bytes); // Зануление свободных битов
//     C[byte_n] = C[byte_n] << (n_free_bytes);
//     C[byte_n] = C[byte_n] | tmp_value_1; // Запись битов в массив
//     if (value_len
//         > n_free_bytes) { // Если длина значения больше, чем свободно бит
//         uint8_t tmp_value_2 = value
//                 << (8 - value_len
//                     + n_free_bytes); // Только те биты, которые не были записаны
//         byte_n++; // Переход к следующему байту массива
//         C[byte_n] = 0;           // Зануление байта
//         C[byte_n] = tmp_value_2; // Запись значения
//     }
//     byte_n++;
//     C[byte_n] = '\0';
//     return offs + value_len;
// }

uint64_t setbits(uint8_t* C, uint64_t offs, uint16_t value, uint8_t value_len)
{
    uint32_t byte_n = offs / 8; // Номер байта с которого начнётся запись
    uint8_t byte_offs = offs % 8; // Смещение внутри этого байта
    value = value << (16 - value_len);
    for (int i = value_len; i > 0; i--) {
        C[byte_n] = (C[byte_n] | ((value & 0x8000) >> (byte_offs + 8)));
        byte_offs++;
        if (byte_offs % 8 == 0) {
            byte_n++;
            byte_offs = 0;
        }
        value = value << 1;
    }
    return offs + value_len;
}

size_t bit_arr_size(Codes* a, char* message)
{
    size_t count = 0;
    size_t size;
    for (int i = 0; message[i] != '\0'; i++)
        count += a[message[i]].len;
    size = count / 8;
    if (count % 8 > 0)
        size++;
    // size++;
    return size;
}

uint8_t*
ENCODE_MSG(char* message, Codes* a, uint64_t* offs, size_t* size_bit_arr)
{
    uint64_t tmp_offs = 0; // Смещение в битовом массиве
    size_t size = bit_arr_size(a, message);
    uint8_t* bit_arr = malloc(size); // Битовый массив
    for (int i = 0; i < size; i++)
        bit_arr[i] = 0;
    uint16_t value; // Значение, которое записывается в битовый массив
    uint8_t value_len; // Длина значения в битах
    for (int i = 0; message[i] != '\0';
         i++) { // Цикл для каждой буквы из сообщения
        value = a[message[i]].code;
        value_len = a[message[i]].len;
        tmp_offs = setbits(
                bit_arr,
                tmp_offs,
                value,
                value_len); // Запись значения в битовый массив
    }
    *offs = tmp_offs;
    *size_bit_arr = size;
    return bit_arr;
}

Codes* copy_codes(Codes* dest, Codes* src)
{
    for (int i = 1; i < 127; i++) {
        dest[i].code = src[i].code;
        dest[i].len = src[i].len;
    }
    return dest;
}

uint8_t getbits(uint8_t* bit_arr, uint64_t offs, uint8_t len)
{
    uint32_t byte_n
            = offs / 8; // Номер байта в который будет происходить запись
    uint8_t byte_offs = offs % 8; // Смещение внутри этого байта
    uint8_t mask = 0x80 >> byte_offs;
    if ((bit_arr[byte_n] & mask) == 0)
        return 0;
    else
        return 1;
}

// uint8_t codelen(uint8_t value)
// {
//     uint8_t count = 0;
//     while (value > 0) {
//         value = value >> 1;
//         count++;
//     }
//     return count;
// }

Codes* clear(Codes* a, uint16_t value, uint8_t value_len)
{
    uint16_t tmp_code;

    for (int i = 1; i < 127; i++) {
        if(a[i].len == 0)
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

char* DECODE_MSG(
        uint8_t* bit_arr,
        Codes* a,
        uint8_t n_last_bits,
        size_t size_bit_arr,
        size_t message_size)
{
    char letter[2] = {0};
    char* message = malloc(message_size + 1);
    size_t n = size_bit_arr - 1;
    Codes tmp[127];
    copy_codes(tmp, a);
    uint16_t value = 0;
    uint8_t value_len = 0;
    for (int i = 0; i < (8 * n + n_last_bits); i++) {
        value = (value << 1) | getbits(bit_arr, i, 1);
        value_len++;
        clear(tmp, value, value_len);
        // print_bin(value);
        // printf("   %d\n", value_len);
        // print_codes(tmp);
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

Heap* init_queue(uint8_t* symbols)
{
    Heap* h = heap_create(127);
    for (int i = 1; i < 127; i++)
        heap_insert(h, symbols[i], i, NULL, NULL);
    return h;
}

Node HTREE(uint8_t* symbols)
{
    Heap* h = init_queue(symbols);
    Node* w1 = NULL;
    Node* w2 = NULL;
    while (heap_nnodes(h) > 1) {
        w1 = malloc(sizeof(Node));
        *w1 = heap_extract_min(h);
        if (w1->freq == 0)
            continue;
        w2 = malloc(sizeof(Node));
        *w2 = heap_extract_min(h);
        heap_insert(h, w1->freq + w2->freq, 0, w1, w2);
    }
    Node* w = malloc(sizeof(Node));
    *w = heap_extract_min(h);
    // heap_free(h);
    return *w;
}

void free_tree(Node* tree)
{
    if(tree->left)
        free_tree(tree->left);
    if(tree->right)
        free_tree(tree->right);
    free(tree);
}

Codes* traverse_tree(Codes* a, Node* tree, uint16_t code, uint8_t len)
{
    uint16_t tmp_code;
    uint8_t tmp_len;
    if (tree->left) {
        tmp_len = len + 1;
        tmp_code = code;
        tmp_code = tmp_code << 1;
        traverse_tree(a, tree->left, tmp_code, tmp_len);
    }
    if (tree->right) {
        tmp_len = len + 1;
        tmp_code = code;
        tmp_code = tmp_code << 1;
        tmp_code = tmp_code | 1;
        traverse_tree(a, tree->right, tmp_code, tmp_len);
    }
    if (tree->left == NULL && tree->right == NULL) {
        a[tree->symbol].code = code;
        a[tree->symbol].len = len;
    }
    return a;
}

off_t fsize(const char* filename)
{
    struct stat st;

    if (stat(filename, &st) == 0)
        return st.st_size;

    return -1;
}

char* read_file(char* filename)
{
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Не удалось открыть файл\n");
        exit(1);
    }
    off_t filesize = fsize(filename);
    char* message = malloc(filesize + 1);
    fread(message, 1, filesize, file);
    fclose(file);
    return message;
}

uint8_t* get_freq(uint8_t* symbols, char* message)
{
    for (int i = 1; i < 127; i++)
        symbols[i] = 0;
    for (int i = 0; message[i] != '\0'; i++)
        symbols[message[i]]++;

    return symbols;
}

void write_file(
        char* filename,
        uint8_t* C,
        Codes* a,
        uint64_t offs,
        size_t size_bit_arr,
        size_t message_size)
{
    FILE* file = fopen(filename, "wb");
    if (file == NULL) {
        printf("Не удалось открыть файл\n");
        exit(1);
    }
    for (int i = 1; i < 127; i++) {
        fwrite(&a[i].code, 2, 1, file);
        fwrite(&a[i].len, 1, 1, file);
    }
    uint8_t n_last_bits = offs % 8;
    fwrite(&message_size, sizeof(size_t), 1, file);
    fwrite(&n_last_bits, 1, 1, file);
    fwrite(&size_bit_arr, sizeof(size_t), 1, file);
    // for (int i = 0; i < size_bit_arr; i++)
        // fwrite(&C[i], 1, 1, file);
    fwrite(C, 1, size_bit_arr, file);
    free(C);
    fclose(file);
}

void uncomp_file(char* filename)
{
    Codes a[127] = {0};
    FILE* file = fopen(filename, "rb");
    FILE* ufile = fopen("uncomp_file", "w");
    for (int i = 1; i < 127; i++) {
        fread(&a[i].code, 2, 1, file);
        fread(&a[i].len, 1, 1, file);
    }
    size_t message_size;
    fread(&message_size, sizeof(size_t), 1, file);
    uint8_t n_last_bits;
    fread(&n_last_bits, 1, 1, file);
    size_t size_bit_arr;
    fread(&size_bit_arr, sizeof(size_t), 1, file);
    uint8_t bit_arr[fsize(filename) - 394];
    // char C[2048];
    fread(bit_arr, 1, fsize(filename) - 395, file);
    // fread(C, 1, 2048, file);
    // C[fsize(filename) - 395] = '\0';
    print_codes(a);
    char* message = DECODE_MSG(bit_arr, a, n_last_bits, size_bit_arr, message_size);
    // for (int i = 0; T[i] != '\0'; i++) {
        // fwrite(&T[i], 1, 1, ufile);
    // }
    fclose(file);
    fwrite(message, 1, message_size, ufile);
    free(message);
    fclose(ufile);
}

int main(int argc, char* argv[])
{
    char* message = read_file(argv[1]);
    size_t message_size = strlen(message);
    uint8_t symbols[127];
    get_freq(symbols, message);
    Node h_tree = HTREE(symbols);
    Codes a[127] = {0};
    traverse_tree(a, &h_tree, 0, 0);
    free_tree(&h_tree);
    print_codes(a);
    uint64_t offs;
    size_t size_bit_arr;
    uint8_t* C = ENCODE_MSG(message, a, &offs, &size_bit_arr);
    write_file(argv[2], C, a, offs, size_bit_arr, message_size);
    uncomp_file(argv[2]);
}