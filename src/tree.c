#include <codes.h>
#include <minheap.h>

Node* HTREE(uint64_t* symbols, Heap* h)
{
    init_heap(symbols, h);
    Node* w1 = NULL;
    Node* w2 = NULL;
    while (heap_nnodes(h) > 1) {
        w1 = malloc(sizeof(Node));
        *w1 = heap_extract_min(h);
        if (w1->freq == 0) {
            free(w1);
            continue;
        }
        w2 = malloc(sizeof(Node));
        *w2 = heap_extract_min(h);
        heap_insert(h, w1->freq + w2->freq, 0, w1, w2);
    }
    Node* w = malloc(sizeof(Node));
    *w = heap_extract_min(h);
    return w;
}

void free_tree(Node* tree)
{
    if (tree->left)
        free_tree(tree->left);
    if (tree->right)
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