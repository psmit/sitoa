#pragma once

#include "headers.h"

struct search_node {
    board_t white;
    board_t black;
    hash_t hash;
    int ply;
};

search_node sn_apply_move(search_node n, board_t move) {
    if(n.ply % 2) {
        n.black ^= move;
        n.hash ^= rand_table[(n.black & move).ctz()][1] ^ rand_table[(~n.black & move).ctz()][1];
    } else {
        n.white ^= move;
        n.hash ^= rand_table[(n.white & move).ctz()][0] ^ rand_table[(~n.white & move).ctz()][0];
    }
    n.ply++;
    return n;
}

int sn_find_moves(search_node * node, board_t * moves) {
    return find_possible_moves(node->ply % 2 ? node->black: node->white , node->ply % 2 ? node->white: node->black, moves);
}

int sn_score(search_node * node) {
    int score_black = find_solution_distance(node->black, node->white);
    int score_white = find_solution_distance(node->white, node->black);

    if (node->ply % 2 == 1) {
        return score_white - score_black;
    } else {
        return score_black - score_white;
    }
}

void sn_dump(FILE * out, search_node * node) {
    fprintf(out, "Dump Search node\nPly %d; hash %016" PRIx64 "\n", node->ply, node->hash);
    char out_s[256];
    visualize_board(out_s, "BW", node->black, node->white);
    fputs(out_s, out);

    fprintf(out, "Score Black: %d, White: %d, Node score: %d Player: %s\n",
            find_solution_distance(node->black, node->white),
            find_solution_distance(node->white, node->black),
            sn_score(node), node->ply % 2 ? "Black": "White");
}

//search_node sn_create(board_t black, board_t white, int ply) {
//    return {.black = black,
// .white=white,
// .ply=ply,
// .hash=init_hash(black, white)
// };
//}