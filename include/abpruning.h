#pragma once

#include "ayu.h"
#include "defs.h"
#include "util.h"
#include <cmath>

#define WIN_SCORE 10000

#define NODES_PER_MOVE 80000

int negamax(board_t board_cur, board_t board_oth, int depth, int color) {
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

    int m;
    int best_value = -WIN_SCORE * 2;
    for (m = 0; m < num_moves; ++m) {
        best_value = max(best_value, -negamax( board_oth, board_cur ^ moves[m], depth-1, -color));
    }

    return best_value;
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

    int m;
    int best_value = -WIN_SCORE * 2;
    for (m = 0; m < num_moves; ++m) {
        best_value = max(best_value, -negamax_ab( board_oth, board_cur ^ moves[m], depth-1, -beta, -alpha));
        alpha = max(best_value, alpha);
        if (alpha >= beta) {
#if USE_STATS
        ++statistics.prunes;
#endif
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
    depth = 1;

    if (num_possible_moves < 30 && num_possible_moves > 1) {
        depth = (int)(log((double) NODES_PER_MOVE) / log((double) num_possible_moves)) -1;
    }

    fprintf(stderr, "Num moves %i depth %i \n", num_possible_moves, depth);
    fflush(stderr);

    int m;

    int best_score = -WIN_SCORE;
    for (m = 0; m < num_possible_moves; ++m) {
        scores[m] = -negamax_ab(board_other, board_cur ^ moves[m], depth, -WIN_SCORE, WIN_SCORE);
//        scores[m] = -negamax(board_other, board_cur ^ moves[m] , depth, -1);
        if (scores[m] > best_score) {
            best_score = scores[m];
        }
    }

    int sel_moves = 0;
    for (m = 0; m < num_possible_moves; ++m) {
        if (scores[m] == best_score) {
            moves[sel_moves++] = moves[m];
        }
    }

    return sel_moves;
}