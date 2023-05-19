#include <codes.h>
#include <minheap.h>
#include <tree.h>
#include <coding.h>
#include <bit_arr.h>

#include <stdint.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

off_t fsize(const char* filename)
{
    struct stat st;

    if (stat(filename, &st) == 0)
        return st.st_size;

    return -1;
}

uint64_t* get_freq(uint64_t* symbols, char* message, off_t message_size)
{
    for (int i = 0; i < message_size + 1; i++)
        symbols[(uint8_t)message[i]]++;

    return symbols;
}

void write_comp_file(
        FILE* file,
        uint8_t* bit_arr,
        Codes* a,
        uint64_t offs,
        size_t size_bit_arr,
        off_t message_size)
{
    uint8_t buf;
    for (int i = 1; i < 127; i++) {
        fwrite(&a[i].len, 1, 1, file);
        if (a[i].len == 0)
            continue;
        if (a[i].len < 9) {
            buf = (uint8_t)a[i].code;
            fwrite(&buf, 1, 1, file);
            continue;
        }
        fwrite(&a[i].code, 2, 1, file);
    }
    uint8_t n_last_bits = offs % 8;
    fwrite(&message_size, sizeof(off_t), 1, file);
    fwrite(&n_last_bits, 1, 1, file);
    fwrite(&size_bit_arr, sizeof(size_t), 1, file);
    fwrite(bit_arr, 1, size_bit_arr, file);
}

Codes* get_codes(Codes* a, char* message, off_t message_size)
{
    uint64_t letters_freq[127] = {0};
    get_freq(letters_freq, message, message_size);
    Heap* h = NULL;
    h = heap_create(h);
    Node* h_tree = HTREE(letters_freq, h);
    traverse_tree(a, h_tree, 0, 0);
    heap_free(h);
    free_tree(h_tree);
    return a;
}

void compress_file(char* input_file_name, char* output_file_name)
{
    FILE* input_file = fopen(input_file_name, "rb");
    if (input_file == NULL) {
        printf("Не удалось открыть файл %s\n", input_file_name);
        exit(-1);
    }
    off_t input_file_size = fsize(input_file_name);
    if (input_file_size == -1) {
        printf("Не удалось определить размер файла %s\n", input_file_name);
        fclose(input_file);
    }
    FILE* output_file = fopen(output_file_name, "wb");
    if (output_file == NULL) {
        printf("Не удалось открыть файл %s\n", output_file_name);
        fclose(input_file);
        exit(-1);
    }
    char* message = malloc(sizeof(char) * input_file_size + 1);
    if (message == NULL) {
        printf("Не удалось выделить память для считывания файла %s\n",
               input_file_name);
        exit(-1);
    }
    for (int i = 0; i < input_file_size + 1; i++)
        message[i] = 0;
    if (fread(message, 1, input_file_size, input_file)
        != (size_t)input_file_size) {
        printf("При чтении файла %s произошла ошибка\n", input_file_name);
        fclose(input_file);
        fclose(output_file);
        free(message);
    }
    fclose(input_file);
    Codes a[127] = {0};
    get_codes(a, message, input_file_size);
    uint64_t offs;
    size_t size_bit_arr;
    uint8_t* bit_arr = ENCODE_MSG(message, a, &offs, &size_bit_arr);
    free(message);
    write_comp_file(
            output_file, bit_arr, a, offs, size_bit_arr, input_file_size);
    free(bit_arr);
    fclose(output_file);
}

void decompress_file(char* input_file_name, char* output_file_name)
{
    FILE* input_file = fopen(input_file_name, "rb");
    if (input_file == NULL) {
        printf("Не удалось открыть файл %s\n", input_file_name);
        exit(-1);
    }
    FILE* output_file = fopen(output_file_name, "w");
    if (output_file == NULL) {
        printf("Не удалось открыть файл %s\n", output_file_name);
        fclose(input_file);
        exit(-1);
    }
    Codes a[127] = {0};
    uint8_t buf;
    for (int i = 1; i < 127; i++) {
        fread(&a[i].len, 1, 1, input_file);
        if (a[i].len == 0)
            continue;
        if (a[i].len < 9) {
            fread(&buf, 1, 1, input_file);
            a[i].code = (uint16_t)buf;
            continue;
        }
        fread(&a[i].code, 2, 1, input_file);
    }
    size_t message_size;
    uint8_t n_last_bits;
    size_t size_bit_arr;
    fread(&message_size, sizeof(size_t), 1, input_file);
    fread(&n_last_bits, 1, 1, input_file);
    fread(&size_bit_arr, sizeof(size_t), 1, input_file);
    uint8_t* bit_arr = malloc(size_bit_arr);
    if (bit_arr == NULL) {
        printf("Не удалось выделить память для битового массива\n");
        fclose(input_file);
        fclose(output_file);
        exit(-1);
    }
    init_bit_arr(bit_arr, size_bit_arr);
    if (fread(bit_arr, 1, size_bit_arr, input_file) != size_bit_arr) {
        printf("Не удалось считать файл %s\n", input_file_name);
        fclose(input_file);
        fclose(output_file);
        free(bit_arr);
        exit(-1);
    }
    fclose(input_file);
    char* message
            = DECODE_MSG(bit_arr, a, n_last_bits, size_bit_arr, message_size);
    free(bit_arr);
    fwrite(message, 1, message_size, output_file);
    free(message);
    fclose(output_file);
}