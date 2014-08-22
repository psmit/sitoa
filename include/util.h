#pragma once

#include <cstdlib>
#include <sys/time.h>


int min(int a, int b){
    if (a < b) {
        return a;
    }
    return b;
}

void init_rand() {
    struct timeval time;
    gettimeofday(&time,NULL);
    unsigned int seed = (time.tv_sec * 1000) + (time.tv_usec / 1000);

    fprintf(stderr,"Rand init with seed %x\n", seed);

    srand(seed);
}