#include "headers.h"

int main(int argc, const char *argv[]) {

//    board_t black = hex_to_board("000008210471c2218b9506805100b000");
//    board_t white = hex_to_board("007c01022a000c18002800040c8503fc");


//    board_t black = hex_to_board("000008210471c2218f15869053000000");
//    board_t white = hex_to_board("003c010239000c180028000c0c0103fe");
    char out[256];

//    visualize_board(out, "BW", black, white);

//    puts(out);

//    board_t moves[MAX_MOVES];
//    int num_moves = find_possible_moves(black, white, moves);
//    hash_t hash = init_hash(black, white);

//    int depth = 6;


    board_t black = B_BLACK_START;
    board_t white = B_WHITE_START;

    board_t moves[MAX_MOVES];

    int num_moves = find_possible_moves(white, black, moves);
    hash_t hash = init_hash(black, white);


    int depth = 3;
//
    int m;
    for (m = 0; m < num_moves; ++m) {
        write_move(out, black & moves[m], ~black & moves[m]);
        puts(out);
        int normal = -negamax(black, white ^ moves[m], depth, 1);
        int ab = -negamax_ab(black, white ^ moves[m], depth, -WIN_SCORE, WIN_SCORE);
        int mem = -negamax_memory(black, white ^ moves[m], hash ^ rand_table[(white & moves[m]).ctz()][0] ^ rand_table[(~white & moves[m]).ctz()][0], depth, -WIN_SCORE, WIN_SCORE, 1);
        printf("Negamax score %d\n", normal);
        printf("Nega ab score %d\n", ab);
        printf("Nega me score %d\n", mem);
//
        if (mem != ab || normal != mem) {
//            normal = -negamax(black, white ^ moves[m], depth, 1);
            ab = -negamax_ab(black, white ^ moves[m], depth, -WIN_SCORE, WIN_SCORE);
//            mem = -negamax_memory(black, white ^ moves[m], hash ^ rand_table[(white & moves[m]).ctz()][1] ^ rand_table[(~white & moves[m]).ctz()][1], depth, -WIN_SCORE, WIN_SCORE, 1, false);
        }
        fflush(stdout);
    }
//
    return 0;

}