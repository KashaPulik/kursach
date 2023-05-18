#include "./minheap.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// Heap* heap_create(int maxsize)
Heap* heap_create()
{
    Heap* h;
    h = malloc(sizeof(*h));
    if (h != NULL) {
        // h->maxsize = maxsize;
        h->maxsize = 126;
        h->nnodes = 0;
        // h->nodes = malloc(sizeof(*h->nodes) * (maxsize + 1));
        if (h->nodes == NULL) {
            free(h);
            return NULL;
        }
    }
    return h;
}

int heap_nnodes(Heap* h)
{
    return h->nnodes;
}

void heap_free(Heap* h)
{
    free(h->nodes);
    free(h);
}

void heap_swap(struct heapnode* a, struct heapnode* b)
{
    struct heapnode temp = *a;
    *a = *b;
    *b = temp;
}

struct heapnode* heap_min(Heap* h)
{
    if (h->nnodes == 0)
        return NULL;
    return &h->nodes[1];
}

int heap_insert(Heap* h, uint64_t freq, uint8_t symbol, Node* left, Node* right)
{
    if (h->nnodes >= h->maxsize)
        return -1;

    h->nnodes++;
    h->nodes[h->nnodes].freq = freq;
    h->nodes[h->nnodes].symbol = symbol;
    h->nodes[h->nnodes].left = left;
    h->nodes[h->nnodes].right = right;

    for (int i = h->nnodes;
         (i > 1) && (h->nodes[i].freq < h->nodes[i / 2].freq);
         i = i / 2)
        heap_swap(&h->nodes[i], &h->nodes[i / 2]);
    return 0;
}

void heap_heapify(Heap* h, int index)
{
    while (1) {
        int left = 2 * index;
        int right = 2 * index + 1;
        int minimal = index;
        if ((left <= h->nnodes)
            && (h->nodes[left].freq < h->nodes[minimal].freq))
            minimal = left;
        if ((right <= h->nnodes)
            && (h->nodes[right].freq < h->nodes[minimal].freq))
            minimal = right;
        if (minimal == index)
            break;
        heap_swap(&h->nodes[index], &h->nodes[minimal]);
        index = minimal;
    }
}

struct heapnode heap_extract_min(Heap* h)
{
    if (h->nnodes == 0)
        return (struct heapnode){0, 0, NULL, NULL};

    struct heapnode minnode = h->nodes[1];
    h->nodes[1] = h->nodes[h->nnodes--];
    heap_heapify(h, 1);

    return minnode;
}

int heap_decrease_freq(Heap* h, int index, int newfreq)
{
    if (h->nodes[index].freq <= newfreq)
        return -1;

    h->nodes[index].freq = newfreq;
    while (index > 1 && h->nodes[index].freq < h->nodes[index / 2].freq) {
        heap_swap(&h->nodes[index], &h->nodes[index / 2]);
        index /= 2;
    }
    return index;
}