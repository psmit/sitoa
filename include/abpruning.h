#pragma once

#include "ayu.h"
#include "defs.h"
#include "util.h"

#define WIN_SCORE 10000

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


int alpha_beta_pruning(board_t board_cur, board_t board_oth, int alpha, int beta, int depth ) {
    if (depth == 0) {
        return find_solution_distance(board_oth, board_cur) - find_solution_distance(board_cur, board_oth);
    }

    board_t moves[MAX_MOVES];
    int num_moves = find_possible_moves(board_cur, board_oth, moves);

    if (num_moves == 0) {
        return WIN_SCORE;
    }

    int m;
    for (m = 0; m < num_moves; ++m) {
        beta = min(beta, alpha_beta_pruning(board_oth, board_cur ^ moves[m], beta, alpha, depth-1));
        if (beta <= -alpha) break;
    }

    return beta;
}

int best_ab_moves(board_t board_cur, board_t board_other, board_t * moves) {
    int num_possible_moves = 0;
    int scores[MAX_MOVES];

    num_possible_moves = find_possible_moves(board_cur, board_other, moves);
    int m;

    int best_score = -WIN_SCORE;
    for (m = 0; m < num_possible_moves; ++m) {
        scores[m] = -alpha_beta_pruning(board_other, board_cur ^ moves[m], -2 * WIN_SCORE, -2 * WIN_SCORE, 3);
    }

    int sel_moves = 0;
    for (m = 0; m < num_possible_moves; ++m) {
        if (scores[m] == best_score) {
            moves[sel_moves++] = moves[m];
        }
    }

    return sel_moves;
}


int best_negamax_moves(board_t board_cur, board_t board_other, board_t * moves, int round) {
    int depth;
    depth = round / 20 + 1;
    int num_possible_moves = 0;
    int scores[MAX_MOVES];

    num_possible_moves = find_possible_moves(board_cur, board_other, moves);
    int m;

    int best_score = -WIN_SCORE;
    for (m = 0; m < num_possible_moves; ++m) {
        scores[m] = -negamax(board_other, board_cur ^ moves[m] , depth, -1);
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