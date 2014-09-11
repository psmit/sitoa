#include "headers.h"

int visits = 0;
int collision = 0;
void visit_recursive(board_t black, board_t white, hash_t hash, bool white_turn, int depth) {
    visits++;
    trans_node * node = lookup(hash);
//    if(node->black) {
//        if (node->black & ~black || node->white & ~white) {
//            collision++;
//            char out[256];
//            board_to_hex(out, node->black);
//            out[32] = ' ';
//            board_to_hex(out + 33, black);
//            out[65] = ' ';
//            board_to_hex(out + 66, node->white);
//            out[98] = ' ';
//            board_to_hex(out + 99, white);
////            out[32] = ' ';
//            puts(out);
//        }
//    }
//    else {
//        node->black = black;
//        node->white = white;
//    }
    node->depth = depth;
    if (depth == 0) return;

    board_t moves[MAX_MOVES];
    int num_moves;
    if (white_turn) {
        num_moves = find_possible_moves(white, black, moves);
    } else {
        num_moves = find_possible_moves(black, white, moves);
    }

    int m;

    for (m = 0; m < num_moves; m++) {
        if (white_turn) {
            visit_recursive(black, white ^moves[m], hash ^ rand_table[(white & moves[m]).ctz()][1] ^ rand_table[(~white & moves[m]).ctz()][1], !white_turn, depth - 1);
        }
        else {
            visit_recursive(black ^moves[m], white, hash ^ rand_table[(black & moves[m]).ctz()][0] ^ rand_table[(~black & moves[m]).ctz()][0], !white_turn, depth - 1);
        }
    }
}

int main( int argc, const char* argv[] ) {

    board_t black = B_BLACK_START;
    board_t white = B_WHITE_START;
    hash_t hash = init_hash(black, white);

    visit_recursive(black, white, hash, true, 4);
    printf("%d, %d, %d, %d", sizeof(trans_node), STORAGE_ID, visits, collision);
    return 0;
}

