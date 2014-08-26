#pragma once

#include "ayu.h"
#include "defs.h"
#include "util.h"
#include <cmath>

#define WIN_SCORE 10000

#define NODES_PER_MOVE 10000000
#define DEPTH_BREAKER 1
#define MAX_DEPTH 10

unsigned int INTERESTING_MOVE_COUNT[BOARD_SIZE* BOARD_SIZE* 2] = {0};
const int MOVE_RANK[BOARD_SIZE* BOARD_SIZE* 2] = {
        20, 20, 19, 20, 19, 20, 19, 20, 19, 20, 19, 20, 19, 20, 19, 20, 19, 20, 19, 20, 20, -1, 20, 19, 18, 18, 17, 18, 17, 18, 17, 18, 17, 18, 17, 18, 17, 18, 17, 18, 18, 19, 20, -1, 20, 19, 18, 17, 16, 16, 15, 16, 15, 16, 15, 16, 15, 16, 15, 16, 16, 17, 18, 19, 20, -1, 20, 19, 18, 17, 16, 15, 14, 14, 13, 14, 13, 14, 13, 14, 14, 15, 16, 17, 18, 19, 20, -1, 20, 19, 18, 17, 16, 15, 14, 13, 12, 12, 11, 12, 12, 13, 14, 15, 16, 17, 18, 19, 20, -1, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 11, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, -1, 20, 19, 18, 17, 16, 15, 14, 13, 13, 12, 13, 12, 13, 13, 14, 15, 16, 17, 18, 19, 20, -1, 20, 19, 18, 17, 16, 15, 15, 14, 15, 14, 15, 14, 15, 14, 15, 15, 16, 17, 18, 19, 20, -1, 20, 19, 18, 17, 17, 16, 17, 16, 17, 16, 17, 16, 17, 16, 17, 16, 17, 17, 18, 19, 20, -1, 20, 19, 19, 18, 19, 18, 19, 18, 19, 18, 19, 18, 19, 18, 19, 18, 19, 18, 19, 19, 20, -1, -1, 20, -1, 20, -1, 20, -1, 20, -1, 20, -1, 20, -1, 20, -1, 20, -1, 20, -1, 20, -1, -1
};

inline int move_to_index(board_t move) {
    int idx = move.ctz();
    if (B_SINGLE_BIT[idx+1] & move) idx = idx * 2 + 1;
    else idx = idx * 2;

    return idx;
}

int compare_moves(const void *a, const void *b) {
    const int idx1 = move_to_index(*(const board_t*) a);
    const int idx2 = move_to_index(*(const board_t*) b);
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

int negamax_ab(board_t board_cur, board_t board_oth, int depth, int alpha, int beta) {
#if USE_STATS
        ++statistics.negamax_count;
#endif
    if (depth == 0) {
        return find_solution_distance(board_oth, board_cur) - find_solution_distance(board_cur, board_oth);
    }

    board_t moves[MAX_MOVES];
    int num_moves = find_possible_moves(board_cur, board_oth, moves);

    if (num_moves == 0) {
        return WIN_SCORE;
    }

    shuffle(moves, num_moves);
    qsort(moves, num_moves, sizeof(board_t), compare_moves);

    int m;
    int best_value = -WIN_SCORE * 2;
    for (m = 0; m < num_moves; ++m) {
        best_value = max(best_value, -negamax_ab( board_oth, board_cur ^ moves[m], depth-1, -beta, -alpha));
        alpha = max(best_value, alpha);
        if (alpha >= beta) {
#if USE_STATS
        ++statistics.prunes;
#endif
            INTERESTING_MOVE_COUNT[move_to_index(moves[m])]++;
            break;
        }
    }

    return best_value;
}



int best_negamax_moves(board_t board_cur, board_t board_other, board_t * moves, int round) {
    int depth;

    int num_possible_moves = 0;
    int scores[MAX_MOVES];

    num_possible_moves = find_possible_moves(board_cur, board_other, moves);
    shuffle(moves, num_possible_moves);
    qsort(moves, num_possible_moves, sizeof(board_t), compare_moves);

    depth = 1;

    if (num_possible_moves > 1) {
        depth = (int)(log((double) NODES_PER_MOVE) / log((double) num_possible_moves + DEPTH_BREAKER)) -1;
        depth = max(depth, 1);
    }

    fprintf(stderr, "Num moves %i depth %i \n", num_possible_moves, depth);
    fflush(stderr);

    int m;

    int best_score = -WIN_SCORE;
    int beta = WIN_SCORE;
    int alpha = -WIN_SCORE;
    for (m = 0; m < num_possible_moves; ++m) {
        scores[m] = -negamax_ab(board_other, board_cur ^ moves[m], depth, -beta, -alpha);
        best_score = max(best_score, scores[m]);
//        alpha = max(best_score, alpha);
    }

    int sel_moves = 0;
    for (m = 0; m < num_possible_moves; ++m) {
        if (scores[m] == best_score) {
            moves[sel_moves++] = moves[m];
        }
    }

    return sel_moves;
}