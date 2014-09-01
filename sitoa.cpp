#include "defs.h"
#include "ayu.h"
#include "util.h"
#include "abpruning.h"

#include "stats.h"

int prefer_outside_to_inside_moves(board_t my_board, board_t * moves, int num_moves) {
    int selected_moves = 0;
    int c;
    int m;
    for (c = 0; c < BOARD_SIZE / 2 + 1; c++) {
        for(m = 0; m < num_moves; ++m) {
            if ((my_board & moves[m] & B_CIRCLES[c]) && (B_CIRCLES[c+1] & moves[m])) {
                moves[selected_moves++] = moves[m];
            }
        }
        if (selected_moves) return selected_moves;

        for(m = 0; m < num_moves; ++m) {
            if ((my_board & moves[m] & B_CIRCLES[c])) {
                moves[selected_moves++] = moves[m];
            }
        }
        if (selected_moves) return selected_moves;
    }

    return num_moves;
}

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


board_t get_move(board_t my_color, board_t other_color, int ply) {
    board_t possible_moves[MAX_MOVES];

    int num_moves;

    num_moves = best_negamax_moves(my_color, other_color, possible_moves, ply);

    num_moves = prefer_outside_to_inside_moves(my_color, possible_moves, num_moves);
//    fprintf(stderr, "Found %d moves after filtering\n", num_moves);

    return random_move(possible_moves, num_moves);
}

void game_loop(FILE * fp) {


    size_t nbytes = 0;
    char *line = NULL;

    board_t mycolor = B_BLACK_START;
    board_t othercolor = B_WHITE_START;

    board_t move;

    char out[256];

    char trace[256][10];
    int num_trace = 0;

    int ply = 0;
    while(getline(&line, &nbytes, fp))
    {
#if USE_STATS
            statistics.resume();
#endif
//        trace[num_trace] = malloc(10);
        strcpy(trace[num_trace++], line);
        if (strcmp(line, "Start\n") == 0) {
            mycolor = B_WHITE_START;
            othercolor = B_BLACK_START;
        } else if (strcmp(line, "Quit\n") == 0) {
            fprintf(stderr, "TRACE:\n");
            int t;
            for (t  = 0; t < num_trace; ++t) {
                fputs(trace[t], stderr);
            }
#if USE_STATS
            statistics.dump_total(stderr);
#endif
            return;
        } else {
            move = read_move(line);
            othercolor ^= move;
            ply++;
        }
        move = get_move(mycolor, othercolor, ply++);
        write_move(out, mycolor & move, ~mycolor & move);

        mycolor ^= move;
#if USE_STATS
        statistics.pause();
        statistics.dump_last(stderr);
#endif
        puts(out);
        fflush(stdout);
    }
    free(line);
}

int main( int argc, const char* argv[] )
{

#if USE_STATS
        init_stats();
#endif

    init_rand(argc > 1 ? argv[1] : "");

    FILE * fp = stdin;
    if(argc > 2) {
        fp =fopen(argv[2], "r");
    }
    game_loop(fp);

    if (argc > 2) {
        fclose(fp);
    }
    return 0;
}