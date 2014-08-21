#pragma once

int ctz128(board_col_t x) {
    // Find trailing zeros for 128 bit variable
    union {
        board_col_t y;
        uint64_t yp[2];
    } u;
    u.y = x;

    if (u.yp[0]) {
        return __builtin_ctzll(u.yp[0]);
    } else{
        return 64 + __builtin_ctzll(u.yp[1]);
    }

}

int popcount128(board_col_t x) {
    union {
        board_col_t y;
        uint64_t yp[2];
    } u;
    u.y = x;

    return __builtin_popcountll(u.yp[0]) + __builtin_popcountll(u.yp[1]);
}

int min(int a, int b){
    if (a < b) {
        return a;
    }
    return b;
}