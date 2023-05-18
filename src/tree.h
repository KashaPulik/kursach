#pragma once

#include <stdint.h>

Node* HTREE(uint64_t* symbols, Heap* h);
void free_tree(Node* tree);
Codes* traverse_tree(Codes* a, Node* tree, uint16_t code, uint8_t len);