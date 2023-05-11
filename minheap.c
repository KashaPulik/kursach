#include "./minheap.h"

Heap* heap_create(int maxsize)
{
    Heap* h;
    h = malloc(sizeof(*h));
    if (h != NULL) {
        h->maxsize = maxsize;
        h->nnodes = 0;
        h->nodes = malloc(sizeof(*h->nodes) * (maxsize + 1));
        if (h->nodes == NULL) {
            free(h);
            return NULL;
        }
    }
    return h;
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

int heap_insert(Heap* h, int key, int value)
{
    if (h->nnodes >= h->maxsize)
        return -1;

    h->nnodes++;
    h->nodes[h->nnodes].key = key;
    h->nodes[h->nnodes].value = value;

    for (int i = h->nnodes; (i > 1) && (h->nodes[i].key < h->nodes[i / 2].key);
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
        if ((left <= h->nnodes) && (h->nodes[left].key < h->nodes[minimal].key))
            minimal = left;
        if ((right <= h->nnodes)
            && (h->nodes[right].key < h->nodes[minimal].key))
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
        return (struct heapnode){0, 0};

    struct heapnode minnode = h->nodes[1];
    h->nodes[1] = h->nodes[h->nnodes--];

    return minnode;
}

int heap_decrease_key(Heap* h, int index, int newkey)
{
    if (h->nodes[index].key <= newkey)
        return -1;

    h->nodes[index].key = newkey;
    while (index > 1 && h->nodes[index].key < h->nodes[index / 2].key) {
        heap_swap(&h->nodes[index], &h->nodes[index / 2]);
        index /= 2;
    }
    return index;
}