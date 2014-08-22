#include "defs.h"
#include "ayu.h"
#include "util.h"



board_t random_move(board_t * moves, int num_moves) {
    return moves[rand() % num_moves];
}

int score_and_filter_moves(board_t my_color, board_t other_color, board_t * moves, int num_moves) {
    int scores[MAX_MOVES];
    int m;

    fprintf(stderr, "Current score %d\n", find_solution_distance(my_color, other_color));


    int best_score = BOARD_SIZE * BOARD_SIZE;
    for (m = 0; m < num_moves; ++m) {
        scores[m] = find_solution_distance(my_color ^ moves[m], other_color) - find_solution_distance(other_color, my_color ^ moves[m]);
        if (scores[m] < best_score) {
            best_score = scores[m];
        }
    }

    fprintf(stderr, "Best score moves %d\n", best_score);

    int filtered_moves = 0;
    for (m = 0; m < num_moves; ++m) {
        if (scores[m] == best_score) {
            moves[filtered_moves++] = moves[m];
        }
    }

    return filtered_moves;
}


board_t get_move(board_t my_color, board_t other_color) {
    board_t possible_moves[MAX_MOVES];
    int num_moves = find_possible_moves(my_color, other_color, possible_moves);

    fprintf(stderr, "Current board: \n");
    char out[512];
    visualize_board(out, "MO", my_color, other_color);
    fputs(out, stderr);

    board_to_hex(out, my_color);
    board_to_hex(out+33, other_color);
    out[32] = ' ';
    fputs(out, stderr);


    fprintf(stderr, "Found %d moves\n", num_moves);
    num_moves = score_and_filter_moves(my_color, other_color, possible_moves, num_moves);
    fprintf(stderr, "Found %d moves after filtering\n", num_moves);
    return random_move(possible_moves, num_moves);
}

void game_loop() {


    size_t nbytes = 0;
    char *line = NULL;

    board_t mycolor = B_BLACK_START;
    board_t othercolor = B_WHITE_START;

    board_t move;

    char out[256];

    while(getline(&line, &nbytes, stdin))
    {
        if (strcmp(line, "Start\n") == 0) {
            mycolor = B_WHITE_START;
            othercolor = B_BLACK_START;
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
    init_rand();

    game_loop();
    return 0;
}