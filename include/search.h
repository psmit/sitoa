#pragma once

#include "headers.h"

unsigned int INTERESTING_MOVE_COUNT[BOARD_SIZE * BOARD_SIZE * 2] = {0};
const int MOVE_RANK[BOARD_SIZE * BOARD_SIZE * 2] = {
        20, 20, 19, 20, 19, 20, 19, 20, 19, 20, 19, 20, 19, 20, 19, 20, 19, 20, 19, 20, 20, -1, 20, 19, 18, 18, 17, 18, 17, 18, 17, 18, 17, 18, 17, 18, 17, 18, 17, 18, 18, 19, 20, -1, 20, 19, 18, 17, 16, 16, 15, 16, 15, 16, 15, 16, 15, 16, 15, 16, 16, 17, 18, 19, 20, -1, 20, 19, 18, 17, 16, 15, 14, 14, 13, 14, 13, 14, 13, 14, 14, 15, 16, 17, 18, 19, 20, -1, 20, 19, 18, 17, 16, 15, 14, 13, 12, 12, 11, 12, 12, 13, 14, 15, 16, 17, 18, 19, 20, -1, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 11, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, -1, 20, 19, 18, 17, 16, 15, 14, 13, 13, 12, 13, 12, 13, 13, 14, 15, 16, 17, 18, 19, 20, -1, 20, 19, 18, 17, 16, 15, 15, 14, 15, 14, 15, 14, 15, 14, 15, 15, 16, 17, 18, 19, 20, -1, 20, 19, 18, 17, 17, 16, 17, 16, 17, 16, 17, 16, 17, 16, 17, 16, 17, 17, 18, 19, 20, -1, 20, 19, 19, 18, 19, 18, 19, 18, 19, 18, 19, 18, 19, 18, 19, 18, 19, 18, 19, 19, 20, -1, -1, 20, -1, 20, -1, 20, -1, 20, -1, 20, -1, 20, -1, 20, -1, 20, -1, 20, -1, 20, -1, -1
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


void print_stuff_tostderr(board_t black, board_t white) {
    char out[1024];
    visualize_board(out, "BW", black, white);
    fputs(out, stderr);
    fprintf(stderr, "\nBlack solution distance: %d\n", find_solution_distance(black, white));
    fprintf(stderr, "White solution distance: %d\n", find_solution_distance(white, black));

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
        best_value = max(best_value, -negamax_ab(board_oth, board_cur ^ moves[m], depth - 1, -beta, -alpha));
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

int negamax_memory(board_t board_cur, board_t board_oth, hash_t hash, int depth, int alpha, int beta, int ply) {
#if USE_STATS
    ++statistics.negamax_count;
#endif

//    hash_t test_hash = ply % 2 ? init_hash(board_cur, board_oth) : init_hash(board_oth, board_cur);
//
//    printf("%#010x %#010x\n", hash, test_hash);
//    fflush(stdout);

    int alpha_orig = alpha;

    trans_node *node = lookup(hash);
    if (node != nullptr && node->flag) {
        if (node->depth >= depth && node->round >= ply) {
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
    hash_t hashp;


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

        hashp = hash ^ rand_table[(board_cur & moves[m]).ctz()][ply % 2] ^ rand_table[(~board_cur & moves[m]).ctz()][ply % 2];

        best_value = max(best_value, -negamax_memory(board_oth, board_cur ^ moves[m], hashp, depth - 1, -beta, -alpha, ply + 1));
        alpha = max(best_value, alpha);
        if (alpha >= beta) {
#if USE_STATS
            ++statistics.prunes;
#endif
            INTERESTING_MOVE_COUNT[move_to_index(moves[m])]++;
            break;
        }
    }

    if (node) {
        node->value = best_value;
        node->depth = depth;
        node->round = ply;
        if (best_value <= alpha_orig) {
            node->flag = TRANS_HIGH;
        } else if (best_value >= beta) {
            node->flag = TRANS_LOWER;
        } else {
            node->flag = TRANS_EXACT;
        }
    }
    return best_value;
}


int best_negamax_moves(board_t board_cur, board_t board_other, board_t *moves, int ply, int * d, int * s) {
    int depth;

    int num_possible_moves = 0;
    int scores[MAX_MOVES];

    hash_t hash = ply % 2 ? init_hash(board_cur, board_other) : init_hash(board_other, board_cur);
//    hash_t hash = ply % 2 ? init_hash(board_other, board_cur) : init_hash(board_cur, board_other);

    num_possible_moves = find_possible_moves(board_cur, board_other, moves);
    shuffle(moves, num_possible_moves);
    qsort(moves, num_possible_moves, sizeof(board_t), compare_moves);

    depth = 1;

    if (num_possible_moves > 1) {
        depth = (int) (log((double) NODES_PER_MOVE) / log((double) num_possible_moves + DEPTH_BREAKER)) - 1;
        depth = max(depth, 1);
    }
    *d = depth;

//    fprintf(stderr, "Num moves %i depth %i \n", num_possible_moves, depth);
//    fflush(stderr);

    int m;

    int best_score = -WIN_SCORE;
    int beta = WIN_SCORE;
    int alpha = -WIN_SCORE;
    for (m = 0; m < num_possible_moves; ++m) {
        scores[m] = -negamax_memory(board_other, board_cur ^ moves[m], hash ^ rand_table[(board_cur & moves[m]).ctz()][ply % 2] ^ rand_table[(~board_cur & moves[m]).ctz()][ply % 2], depth, -beta, -alpha, ply + 1);
        best_score = max(best_score, scores[m]);
        alpha = max(best_score, alpha);
    }

    int sel_moves = 0;
    for (m = 0; m < num_possible_moves; ++m) {
        if (scores[m] == best_score) {
            moves[sel_moves++] = moves[m];
        }
    }

    *s = best_score;
    fprintf(stderr, "# %d nodes in table\n", STORAGE_ID);
    fprintf(stderr, "# best score %d\n", best_score);

    return min(sel_moves, 1);
}

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
        best_value = max(best_value, -negamax(board_oth, board_cur ^ moves[m], depth - 1, -color));
    }

    return best_value;
}