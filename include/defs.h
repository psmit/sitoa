#pragma once

#define BOARD_SIZE 11
#define MAX_VERTICES 30

typedef __uint128_t board_col_t;

board_col_t NEIGHBORS[BOARD_SIZE * BOARD_SIZE];

board_col_t ZERO_128 = 0;
board_col_t ONE_128 = 1;

board_col_t BLACK_START;
board_col_t WHITE_START;

void init() {
    int x, y;
    board_col_t neighbors;
    for (x = 0; x < BOARD_SIZE; ++x) {
        for (y = 0; y < BOARD_SIZE; ++y) {
            neighbors = 0;

            if(x > 0) {
                neighbors |= ONE_128 << ((x-1) * BOARD_SIZE + y);
            }

            if(y > 0) {
                neighbors |= ONE_128 << (x * BOARD_SIZE + (y-1));
            }

            if(x < BOARD_SIZE - 1) {
                neighbors |= ONE_128 << ((x+1) * BOARD_SIZE + y);
            }

            if(y < BOARD_SIZE - 1) {
                neighbors |= ONE_128 << (x * BOARD_SIZE + (y+1));
            }

            NEIGHBORS[x* BOARD_SIZE + y] = neighbors;
        }
    }

//    BLACK_START = 426;
    WHITE_START = 682;
    WHITE_START = WHITE_START | WHITE_START << 22 | WHITE_START << 44 | WHITE_START << 66 | WHITE_START << 88 | WHITE_START << 110;
    BLACK_START = 1365;
    BLACK_START = BLACK_START << 11 | BLACK_START << 33 | BLACK_START << 55 | BLACK_START << 77 | BLACK_START << 99;
}