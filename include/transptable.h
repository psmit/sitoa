#pragma once

#include "headers.h"

struct trans_node {
    hash_t hash;
    int16_t value;
    uint16_t flag : 3;
    uint16_t depth : 5;
    uint16_t round : 5;
    uint16_t : 3;

    trans_node *next;
};

unsigned int STORAGE_ID;

trans_node *TRANS_TABLE[HASH_TABLE_SIZE] = {0};
trans_node TRANS_STORAGE[STORAGE_SIZE] = {0};

trans_node *lookup(hash_t h) {
    trans_node *node = TRANS_TABLE[h % HASH_TABLE_SIZE];

    while (node) {
        if (node->hash == h) {
            return node;
        }
        node = node->next;
    }
    // if we are here we create a new node
    if (STORAGE_ID < STORAGE_SIZE) {
        node = &(TRANS_STORAGE[STORAGE_ID++]);
        node->hash = h;
        node->next = TRANS_TABLE[h % HASH_TABLE_SIZE];
        TRANS_TABLE[h % HASH_TABLE_SIZE] = node;
    }
    return node;
}