#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct heapnode {
    uint64_t freq;
    uint8_t symbol;
    struct heapnode* left;
    struct heapnode* right;
} Node;

typedef struct heap {
    uint8_t nnodes;
    struct heapnode nodes[127];
} Heap;

Heap* heap_create();
uint8_t heap_nnodes(Heap* h);
void heap_free(Heap* h);
int heap_insert(Heap* h, uint64_t freq, uint8_t symbol, Node* left, Node* right);
struct heapnode heap_extract_min(Heap* h);
Heap* init_heap(uint64_t* symbols, Heap* h);