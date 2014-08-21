#include "defs.h"
#include "ayu.h"

#include <string.h>

int score_and_filter_moves(board_col_t my_color, board_col_t other_color, board_col_t * moves, int num_moves) {
    int scores[MAX_MOVES];
    int m;

    fprintf(stderr, "Current score %d\n", find_solution_distance(my_color, other_color));

    int best_score = BOARD_SIZE * BOARD_SIZE;
    for (m = 0; m < num_moves; ++m) {
        scores[m] = find_solution_distance(my_color ^ moves[m], other_color);
        if (scores[m] < best_score) {
            best_score = scores[m];
        }
    }

    fprintf(stderr, "Best score moves %d\n", best_score);

    int filtered_moves = 0;
    for (m = 0; m < num_moves; ++m) {
        if (scores[m] = best_score) {
            moves[filtered_moves++] = moves[m];
        }
    }

    return filtered_moves;
}


board_col_t get_move(board_col_t my_color, board_col_t other_color) {
    board_col_t possible_moves[MAX_MOVES];
    int num_moves = find_possible_moves(my_color, other_color, possible_moves);
    fprintf(stderr, "Found %d moves\n", num_moves);
    num_moves = score_and_filter_moves(my_color, other_color, possible_moves, num_moves);
    fprintf(stderr, "Found %d moves after filtering\n", num_moves);
    return possible_moves[0];
}

void game_loop() {
    size_t nbytes = 0;
    char *line = NULL;

    board_col_t mycolor = BLACK_START;
    board_col_t othercolor = WHITE_START;

    board_col_t move;

    char out[256];

    while(getline(&line, &nbytes, stdin))
    {
        if (strcmp(line, "Start\n") == 0) {
            mycolor = WHITE_START;
            othercolor = BLACK_START;
        } else if (strcmp(line, "Quit\n") == 0) {
            return;
        } else {
            move = read_move(line);
            othercolor ^= move;
        }

        move = get_move(mycolor, othercolor);
        write_move(out, mycolor & move, ~mycolor & move);

        mycolor ^= move;

        puts(out);
        fflush(stdout);
    }
    free(line);
}

int main( int argc, const char* argv[] )
{
    init();
    game_loop();
    return 0;
}