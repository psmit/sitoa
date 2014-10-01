#pragma once

#include "headers.h"

unsigned int INTERESTING_MOVE_COUNT[BOARD_SIZE * BOARD_SIZE * 2] = {0};

const int MOVE_RANK[BOARD_SIZE * BOARD_SIZE * 2] = {
        20, 20, 19, 20, 19, 20, 19, 20, 19, 20, 19,
        20, 19, 20, 19, 20, 19, 20, 19, 20, 20, -1,
        20, 19, 18, 18, 17, 18, 17, 18, 17, 18, 17,
        18, 17, 18, 17, 18, 17, 18, 18, 19, 20, -1,
        20, 19, 18, 17, 16, 16, 15, 16, 15, 16, 15,
        16, 15, 16, 15, 16, 16, 17, 18, 19, 20, -1,
        20, 19, 18, 17, 16, 15, 14, 14, 13, 14, 13,
        14, 13, 14, 14, 15, 16, 17, 18, 19, 20, -1,
        20, 19, 18, 17, 16, 15, 14, 13, 12, 12, 11,
        12, 12, 13, 14, 15, 16, 17, 18, 19, 20, -1,
        20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 11,
        11, 12, 13, 14, 15, 16, 17, 18, 19, 20, -1,
        20, 19, 18, 17, 16, 15, 14, 13, 13, 12, 13,
        12, 13, 13, 14, 15, 16, 17, 18, 19, 20, -1,
        20, 19, 18, 17, 16, 15, 15, 14, 15, 14, 15,
        14, 15, 14, 15, 15, 16, 17, 18, 19, 20, -1,
        20, 19, 18, 17, 17, 16, 17, 16, 17, 16, 17,
        16, 17, 16, 17, 16, 17, 17, 18, 19, 20, -1,
        20, 19, 19, 18, 19, 18, 19, 18, 19, 18, 19,
        18, 19, 18, 19, 18, 19, 18, 19, 19, 20, -1,
        -1, 20, -1, 20, -1, 20, -1, 20, -1, 20, -1,
        20, -1, 20, -1, 20, -1, 20, -1, 20, -1, -1
};

inline int move_to_index(board_t move) {
    int idx = move.ctz();
    if (B_SINGLE_BIT[idx + 1] & move) idx = idx * 2 + 1;
    else idx = idx * 2;

    return idx;
}

int compare_moves(const void *a, const void *b) {
    const int idx1 = move_to_index(*(const board_t *) a);
    const int idx2 = move_to_index(*(const board_t *) b);
    const int val1 = INTERESTING_MOVE_COUNT[idx1];
    const int val2 = INTERESTING_MOVE_COUNT[idx2];

    if (val1 == val2) {
        const int mr1 = MOVE_RANK[idx1];
        const int mr2 = MOVE_RANK[idx2];

        if (mr1 == mr2) return 0;
        else return (mr1 > mr2) ? -1 : 1;
    };
    return (val1 > val2) ? -1 : 1;
}


int negamax_memory_rec(search_node snode, int depth, int alpha, int beta) {
    ++statistics.negamax_count;
    int alpha_orig = alpha;

    trans_node *node = lookup(snode.hash);
    if (node != nullptr && node->flag) {
        if (node->depth >= depth && node->round >= snode.ply) {
            if (node->flag & TRANS_EXACT) {
                return node->value;
            } else if (node->flag & TRANS_LOWER) {
                alpha = max(alpha, node->value);
            } else if (node->flag & TRANS_HIGH) {
                beta = min(beta, node->value);
            }
        }
    }

    if (alpha >= beta) {
        return alpha;
    }

    if (depth == 0) {
        return sn_score(&snode);
    }

    board_t moves[MAX_MOVES];
    int num_moves = sn_find_moves(&snode, moves);

    if (num_moves == 0) {
        return WIN_SCORE;
    }

    shuffle(moves, num_moves);
    qsort(moves, num_moves, sizeof(board_t), compare_moves);

    int m;
    int best_value = -WIN_SCORE * 2;
    for (m = 0; m < num_moves; ++m) {
        best_value = max(best_value, -negamax_memory_rec(sn_apply_move(snode, moves[m]), depth - 1, -beta, -alpha));
        alpha = max(best_value, alpha);
        if (alpha >= beta) {
            statistics.prunes++;
            INTERESTING_MOVE_COUNT[move_to_index(moves[m])]++;
            break;
        }
    }

    if (node) {
        node->value = best_value;
        node->depth = depth;
        node->round = snode.ply;
        if (best_value <= alpha_orig) {
            node->flag = TRANS_HIGH;
        } else if (best_value >= beta) {
            node->flag = TRANS_LOWER;
        } else {
            node->flag = TRANS_EXACT;
        }
    }

    return best_value;
};

board_t negamax_memory_decision(search_node node, int depth, int * move_score ) {
    board_t moves[MAX_MOVES];

    int num_possible_moves = sn_find_moves(&node, moves);

    shuffle(moves, num_possible_moves);
    qsort(moves, num_possible_moves, sizeof(board_t), compare_moves);

    int m;

    int best_score = -WIN_SCORE;
    int score;
    board_t best_move = B_EMPTY;

    int beta = WIN_SCORE;
    int alpha = -WIN_SCORE;

    for (m = 0; m < num_possible_moves; ++m) {
        score = -negamax_memory_rec(sn_apply_move(node, moves[m]), depth, -beta, -alpha);
        if (score > best_score) {
            best_score = score;
            best_move = moves[m];
            alpha = max(best_score, alpha);
        }
    }

    *move_score = best_score;
    fprintf(stderr, "# %d nodes in table\n", STORAGE_ID);
    return best_move;
}

board_t mdf_decision(search_node node, int depth, int * move_score) {
    return B_EMPTY;
}