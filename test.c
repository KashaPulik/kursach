#include "./minheap.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

typedef struct codes {
    uint8_t code;
    uint8_t len;
} Codes;

typedef struct tree {
    uint64_t weight;
    uint8_t symbol;
    struct tree* left;
    struct tree* right;
} Tree;

typedef struct queue {
    uint64_t weight;
    uint8_t symbol;
} Queue;

enum letters {
    letter_a,
    letter_b,
    letter_c,
    letter_d,
    letter_e,
    letter_f,
    letter_g,
    letter_h,
    letter_i,
    letter_j,
    letter_k,
    letter_l,
    letter_m,
    letter_n,
    letter_o,
    letter_p,
    letter_q,
    letter_r,
    letter_s,
    letter_t,
    letter_u,
    letter_v,
    letter_w,
    letter_x,
    letter_y,
    letter_z,
};

uint8_t get_letter_index(uint8_t T)
{
    return T - 97;
}

uint8_t get_letter(uint8_t s)
{
    return s + 97;
}

uint64_t setbits(uint8_t* C, uint64_t offs, uint8_t value, uint8_t value_len)
{
    uint32_t byte_n
            = offs / 8; // Номер байта в который будет происходить запись
    uint8_t byte_offs = offs % 8; // Смещение внутри этого байта
    uint8_t n_free_bytes
            = 8 - byte_offs; // Количество свободных бит в крайнем байте
    uint8_t tmp_value_1 = value
            << (8 - value_len); // Биты, которые будут записаны
    tmp_value_1 = tmp_value_1
            >> byte_offs; // Подгонка позиции битов под место в массиве
    C[byte_n] = C[byte_n] >> (n_free_bytes); // Зануление свободных битов
    C[byte_n] = C[byte_n] << (n_free_bytes);
    C[byte_n] = C[byte_n] | tmp_value_1; // Запись битов в массив
    if (value_len
        > n_free_bytes) { // Если длина значения больше, чем свободно бит
        uint8_t tmp_value_2 = value
                << (8 - value_len
                    + n_free_bytes); // Только те биты, которые не были записаны
        byte_n++; // Переход к следующему байту массива
        C[byte_n] = 0;           // Зануление байта
        C[byte_n] = tmp_value_2; // Запись значения
    }
    byte_n++;
    C[byte_n] = '\0';
    return offs + value_len;
}

size_t new_file_size(Codes* a, uint8_t* T)
{
    size_t count = 0;
    while (*T != '\0') {
        count += a[get_letter_index(*T)].len;
        T++;
    }
    return count/8 + 61;
}

uint8_t* ENCODE_MSG(uint8_t* T, Codes* a)
{
    uint64_t offs = 0; // Смещение в битовом массиве
    uint8_t* C = malloc(new_file_size(a, T)); // Битовый массив
    uint8_t value; // Значение, которое записывается в битовый массив
    uint8_t value_len; // Длина значения в битах
    for (int i = 0; T[i] != '\0'; i++) { // Цикл для каждой буквы из сообщения
        value = a[get_letter_index(T[i])].code;
        value_len = a[get_letter_index(T[i])].len;
        offs = setbits(
                C, offs, value, value_len); // Запись значения в битовый массив
    }
    return C;
}

Codes* copy_codes(Codes* tmp, Codes* src)
{
    for (int i = 0; i < 26; i++) {
        tmp[i].code = src[i].code;
        tmp[i].len = src[i].len;
    }
    return tmp;
}

uint8_t getbits(uint8_t* array, uint64_t offs, uint8_t len)
{
    uint32_t byte_n
            = offs / 8; // Номер байта в который будет происходить запись
    uint8_t byte_offs = offs % 8; // Смещение внутри этого байта
    uint8_t mask = 0x80 >> byte_offs;
    if (array[byte_n] & (mask == 0))
        return 0;
    else
        return 1;
}

uint8_t codelen(uint8_t value)
{
    uint8_t count = 0;
    while (value > 0) {
        value = value >> 1;
        count++;
    }
    return count;
}

Codes* clear(Codes* tmp, uint8_t value)
{
    uint8_t tmp_value;
    uint8_t value_len = codelen(value);
    for (int i = 0; i < 26; i++) {
        tmp_value = value << (tmp[i].len - value_len);
        if (tmp[i].code < tmp_value)
            tmp[i].len = 0;
    }
    return tmp;
}

_Bool find_code(Codes* tmp, uint8_t value, uint8_t s)
{
    if (tmp[s].code == value)
        return 1;
    return 0;
}

_Bool empty_codes(Codes* tmp)
{
    for (int i = 0; i < 26; i++)
        if (tmp[i].len != 0)
            return 0;
    return 1;
}

uint8_t* DECODE_MSG(uint8_t* C, Codes* a)
{
    char letter[2] = {0};
    uint8_t* T = malloc(2048);
    size_t n = strlen((char*)C);
    Codes tmp[26];
    copy_codes(tmp, a);
    uint8_t value = 0;
    for (int i = 0; i < 8 * n; i++) {
        value = (value << 1) | getbits(C, i, 1);
        clear(tmp, value);
        for (uint8_t s = 0; s < 26; s++) {
            if (find_code(tmp, value, s)) {
                letter[0] = s;
                T = (uint8_t*)strcat((char*)T, letter);
                copy_codes(tmp, a);
                value = 0;
            }
        }
        if (empty_codes(tmp)) {
            printf("Ошибка декодирования, такого кода не существует!\n");
            exit(1);
        }
    }
    return T;
}

// uint64_t queue_len(Queue* symbols)
// {
//     uint64_t count = 1;
//     while(symbols->next != NULL) {
//         T = T->next;
//         count++;
//     }
//     return count;
// }

Heap* init_queue(Queue* symbols)
{
    Heap* h = heap_create(26);
    for(int i = 0; i < 26; i++) {
        heap_insert(h, symbols[i].weight, symbols[i].symbol, NULL, NULL);
    }
    return h;
}

Node HTREE(Queue* symbols)
{
    Heap* h = init_queue(symbols);
    Node w1 = {0, 0, NULL, NULL};
    Node w2 = {0, 0, NULL, NULL};
    while(heap_nnodes(h) > 1) {
        w1 = heap_extract_min(h);
        if(w1.freq == 0)
            continue;
        w2 = heap_extract_min(h);
        heap_insert(h, w1.freq + w2.freq, 0, &w1, &w2);
    }
    return heap_extract_min(h);
}

Codes* traverse_tree(Codes* a, Node* tree, uint8_t code, _Bool key)
{
    if(tree->left && tree->right) {
        code = code << 1;
        if(key == 0)
            code = code | 0;
        else
            code = code | 1;
        Node* buf = tree->left;
        traverse_tree(a, buf, code, 0);
        buf = tree->right;
        traverse_tree(a, buf, code, 1);
    }
    else {
        a[get_letter_index(tree->symbol)].code = code;
    }
    return a;
}

off_t fsize(const char *filename) {
    struct stat st; 

    if (stat(filename, &st) == 0)
        return st.st_size;

    return -1; 
}

uint8_t* read_file(char* filename)
{
    FILE* file = fopen(filename, "rb");
    if(file == NULL) {
        printf("Не удалось открыть файл\n");
        exit(1);
    }
    off_t filesize = fsize(filename);
    uint8_t* buf = malloc(filesize + 1);
    fread(buf, 1, filesize, file);
    fclose(file);
    return buf;
}

Queue* get_freq(Queue* symbols, uint8_t* uncompressed)
{
    for(int i = 0; i < 26; i++) {
        symbols[i].symbol = get_letter(i);
        symbols[i].weight = 0;
    }

    for(int i = 0; uncompressed[i] != '\0'; i++)
        symbols[get_letter_index(uncompressed[i])].weight++;
    
    return symbols;
}

void write_file(char* filename, uint8_t* C, Codes* a)
{
    FILE* file = fopen(filename, "wb");
    if(file == NULL) {
        printf("Не удалось открыть файл\n");
        exit(1);
    }
    for(int i = 0; i < 26; i++) {
        fwrite(&a[i].code, 1, 1, file);
        fwrite(&a[i].len, 1, 1, file);
    }
    for(int i = 0; C[i] != '\0'; i++)
        fwrite(&C[i], 1, 1, file);
}

int main(int argc, char *argv[])
{
    uint8_t* uncompressed = read_file(argv[1]);
    Queue symbols[26];
    get_freq(symbols, uncompressed);
    Node h_tree = HTREE(symbols);
    Codes a[26];
    traverse_tree(a, &h_tree, 0, 0);
    uint8_t* C = ENCODE_MSG(uncompressed, a);
    write_file(argv[2], C, a);
}