#pragma once

#include "headers.h"

#define CODEBOOK_MAXPLY 4
#define CODEBOOK_SIZE 5
#define CODEBOOK_RANGE 64

struct cb {
    hash_t hash;
    uint16_t moveindices;
};

const cb CODEBOOK[CODEBOOK_SIZE] = {
        {0x0, 0x0},
        {0x0, 0x0},
        {0x0, 0x0}};

board_t get_codebook_move(search_node node) {
    unsigned first = 0;
    unsigned last = CODEBOOK_SIZE - 1;
    unsigned guess = node.hash / (double) HASH_T_MAX;
    if (guess - CODEBOOK_RANGE > 0 && CODEBOOK[guess - CODEBOOK_RANGE].hash < node.hash) {
        first = guess - CODEBOOK_RANGE;
    }

    if (guess + CODEBOOK_RANGE < CODEBOOK_SIZE && CODEBOOK[guess + CODEBOOK_RANGE].hash > node.hash) {
        last = guess + CODEBOOK_RANGE;
    }

    unsigned middle = (first+last)/2;

    while (first <= last) {
        if(CODEBOOK[middle].hash < node.hash) {
            first = middle + 1;
        } else if (CODEBOOK[middle].hash == node.hash) {
            uint16_t idx_low = CODEBOOK[middle].moveindices & 0xFF;
            uint16_t idx_high = CODEBOOK[middle].moveindices >> 8;
            return B_SINGLE_BIT[idx_low] | B_SINGLE_BIT[idx_high];
        } else {
            last = middle - 1;
        }

        middle = (first + last)/2;
    }

    fprintf(stderr, "Codebook lookup failed! %lx", node.hash);

    return B_EMPTY;
}