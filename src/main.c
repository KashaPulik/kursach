#include <minheap.h>
#include <bit_arr.h>
#include <codes.h>
#include <coding.h>
#include <output.h>
#include <tree.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

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

uint64_t* get_freq(uint64_t* symbols, char* message, off_t message_size)
{
    // for (int i = 1; i < 127; i++)
    //     symbols[i] = 0;
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
        if(a[i].len == 0)
            continue;
        if(a[i].len < 9) {
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

// void uncomp_file(char* filename)
// {
//     Codes a[127] = {0};
//     FILE* file = fopen(filename, "rb");
//     FILE* ufile = fopen("uncomp_file", "w");
//     for (int i = 1; i < 127; i++) {
//         fread(&a[i].code, 2, 1, file);
//         fread(&a[i].len, 1, 1, file);
//     }
//     size_t message_size;
//     fread(&message_size, sizeof(size_t), 1, file);
//     uint8_t n_last_bits;
//     fread(&n_last_bits, 1, 1, file);
//     size_t size_bit_arr;
//     fread(&size_bit_arr, sizeof(size_t), 1, file);
//     uint8_t bit_arr[fsize(filename) - 394];
//     fread(bit_arr, 1, fsize(filename) - 395, file);
//     print_codes(a);
//     char* amessage
//             = DECODE_MSG(bit_arr, a, n_last_bits, size_bit_arr, message_size);
//     fclose(file);
//     fwrite(amessage, 1, message_size, ufile);
//     free(amessage);
//     fclose(ufile);
// }

// int main(int argc, char* argv[])
// {
//     int abc = argc;
//     abc = abc + 2;
//     char* message = read_file(argv[1]);
//     size_t message_size = strlen(message);
//     uint8_t symbols[127];
//     get_freq(symbols, message);
//     Heap* h = heap_create();
//     Node* h_tree = HTREE(symbols, h);
//     Codes a[127] = {0};
//     traverse_tree(a, h_tree, 0, 0);
//     free(h);
//     free_tree(h_tree);
//     // print_codes(a);
//     uint64_t offs;
//     size_t size_bit_arr;
//     uint8_t* C = ENCODE_MSG(message, a, &offs, &size_bit_arr);
//     free(message);
//     write_file(argv[2], C, a, offs, size_bit_arr, message_size);
//     uncomp_file(argv[2]);
// }

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
    if(input_file == NULL) {
        printf("Не удалось открыть файл %s\n", input_file_name);
        exit(-1);
    }
    off_t input_file_size = fsize(input_file_name);
    if(input_file_size == -1) {
        printf("Не удалось определить размер файла %s\n", input_file_name);
        fclose(input_file);
    }
    FILE* output_file = fopen(output_file_name, "wb");
    if(output_file == NULL) {
        printf("Не удалось открыть файл %s\n", output_file_name);
        fclose(input_file);
        exit(-1);
    }
    char* message = malloc(sizeof(char) * input_file_size + 1);
    if(message == NULL) {
        printf("Не удалось выделить память для считывания файла %s\n", input_file_name);
        exit(-1);
    }
    for(int i = 0; i < input_file_size + 1; i++)
        message[i] = 0;
    if(fread(message, 1, input_file_size, input_file) != (size_t)input_file_size) {
        printf("При чтении файла %s произошла ошибка\n", input_file_name);
        fclose(input_file);
        fclose(output_file);
        free(message);
    }
    fclose(input_file);
    Codes a[127] = {0};
    get_codes(a, message, input_file_size);
    // print_codes(a);
    uint64_t offs;
    size_t size_bit_arr;
    uint8_t* bit_arr = ENCODE_MSG(message, a, &offs, &size_bit_arr);
    free(message);
    write_comp_file(output_file, bit_arr, a, offs, size_bit_arr, input_file_size);
    free(bit_arr);
    fclose(output_file);
}

void decompress_file(char* input_file_name, char* output_file_name)
{
    FILE* input_file = fopen(input_file_name, "rb");
    if(input_file == NULL) {
        printf("Не удалось открыть файл %s\n", input_file_name);
        exit(-1);
    }
    FILE* output_file = fopen(output_file_name, "w");
    if(output_file == NULL) {
        printf("Не удалось открыть файл %s\n", output_file_name);
        fclose(input_file);
        exit(-1);
    }
    Codes a[127] = {0};
    uint8_t buf;
    uint16_t n_info_bytes = 0;
    for (int i = 1; i < 127; i++) {
        fread(&a[i].len, 1, 1, input_file);
        n_info_bytes++;
        if(a[i].len == 0)
            continue;
        if(a[i].len < 9) {
            n_info_bytes++;
            fread(&buf, 1, 1, input_file);
            a[i].code = (uint16_t) buf;
            continue;
        }
        n_info_bytes += 2;
        fread(&a[i].code, 2, 1, input_file);
    }
    n_info_bytes += 17;
    size_t message_size;
    uint8_t n_last_bits;
    size_t size_bit_arr;
    fread(&message_size, sizeof(size_t), 1, input_file);
    fread(&n_last_bits, 1, 1, input_file);
    fread(&size_bit_arr, sizeof(size_t), 1, input_file);
    uint8_t* bit_arr = malloc(size_bit_arr);
    if(bit_arr == NULL) {
        printf("Не удалось выделить память для битового массива\n");
        fclose(input_file);
        fclose(output_file);
        exit(-1);
    }
    init_bit_arr(bit_arr, size_bit_arr);
    if(fread(bit_arr, 1, size_bit_arr, input_file) != size_bit_arr) {
        printf("Не удалось считать файл %s\n", input_file_name);
        fclose(input_file);
        fclose(output_file);
        free(bit_arr);
        exit(-1);
    }
    // print_codes(a);
    fclose(input_file);
    char* message
            = DECODE_MSG(bit_arr, a, n_last_bits, size_bit_arr, message_size);
    free(bit_arr);
    fwrite(message, 1, message_size, output_file);
    free(message);
    fclose(output_file);
}

int main(int argc, char* argv[])
{
    if(argc != 5) {
        printf("Неправильное количество аргументов\n");
        return -1;
    }
    if(strcmp(argv[2], "-o") != 0) {
        printf("Ожидался аргумент -o для указания имени выходного файла\n");
        return -1;
    }
    if((strcmp(argv[1], "-c") != 0) && (strcmp(argv[1], "-d") != 0)) {
        printf("Ожидался аргумент -с для сжатия или -d для распаковки\n");
        return -1;
    }
    if(strcmp(argv[1], "-c") == 0) {
        compress_file(argv[4], argv[3]);
    }
    if(strcmp(argv[1], "-d") == 0) {
        decompress_file(argv[4], argv[3]);
    }
    printf("Успех!\n");
    return 0;
}