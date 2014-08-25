#include "ayu.h"

#pragma once



int move_inward_moves(board_t board_cur, board_t board_other, board_t * moves, int round) {
    int num_moves;

    num_moves = find_possible_moves(board_cur, board_other, moves);

    int selected_moves = 0;
    int circle = 0;
    int m;
    while (selected_moves == 0) {
        for (m = 0; m < num_moves; ++m) {
            if(board_cur & moves[m] & B_CIRCLES[circle]) {
                if (find_solution_distance(board_cur, board_other) > find_solution_distance(board_cur ^ moves[m], board_other)) {
                    moves[selected_moves++] = moves[m];
                }
            }
        }
        circle++;
    }


    return selected_moves;
}