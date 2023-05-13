#pragma once

#include <stdio.h>
#include <stdlib.h>

typedef struct heapnode {
    uint64_t freq;
    uint8_t symbol;
    struct heapnode* left;
    struct heapnode* right;
} Node;

typedef struct heap {
    int maxsize;
    int nnodes;
    struct heapnode* nodes;
} Heap;

Heap* heap_create(int maxsize);
void heap_free(Heap* h);
void heap_swap(struct heapnode* a, struct heapnode* b);
struct heapnode* heap_min(Heap* h);
int heap_insert(Heap* h, uint64_t freq, uint8_t symbol, Node* left, Node* right);
void heap_heapify(Heap* h, int index);
struct heapnode heap_extract_min(Heap* h);
int heap_decrease_key(Heap* h, int index, int newkey);