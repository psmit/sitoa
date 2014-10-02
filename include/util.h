#pragma once

#include "headers.h"

int min(int a, int b) {
    if (a < b) {
        return a;
    }
    return b;
}

int max(int a, int b) {
    if (a > b) {
        return a;
    }
    return b;
}

void init_rand(unsigned int seed) {
    if (seed == 0) {
        struct timeval time;
        gettimeofday(&time, NULL);
        seed = (time.tv_sec * 1000) + (time.tv_usec / 1000);
    }

    fprintf(stderr, "Randseed %x\n", seed);

    srand(seed);
}

int rand_i(int min, int max) {
    return min + rand() % (max - min);
}

void shuffle(board_t *moves, int num_moves) {
#ifndef DETERMINISTIC
    int m;
    int n;
    for (m = 0; m < num_moves; ++m) {
        n = rand_i(m, num_moves);
        if (n != m) {
            moves[m] ^= moves[n];
            moves[n] ^= moves[m];
            moves[m] ^= moves[n];
        }

    }
#endif
}