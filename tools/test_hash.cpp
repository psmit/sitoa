#include "ayu.h"
#include "hash.h"

int main( int argc, const char* argv[] ) {
    printf("%lu", init_hash(B_BLACK_START, B_WHITE_START));

    board_t black = hex_to_board("000008210471c2218f15869053000000");
    board_t white = hex_to_board("003c010239000c180028000c0c0103fe");

    hash_t orig_hash = init_hash(black, white);
    printf("%#010x\n", orig_hash);

    board_t moves[MAX_MOVES];
    int num_moves = find_possible_moves(black, white, moves);

    int m;
    for (m = 0; m < num_moves; ++m) {

        board_t black_new = black ^moves[m];

        hash_t new_hash = orig_hash ^

        printf("%#010x\n", init_hash(black_new, white));
    }
    return 0;
}