#include "headers.h"

struct move_stats {
    int ply;
    int depth;
    double time;

};

int get_depth(int iter, int fixed_depth, search_node *sn, move_stats *stats, double time_left) {
    // if fixed depth, do exactly one iteration


    board_t moves[MAX_MOVES];

    int depth = stats->depth;

    int expected_moves = 2 * min(find_solution_distance(sn->white, sn->black),find_solution_distance(sn->black, sn->white));
    int branch_factor = sn_find_moves(sn, moves);

    double time_per_move = time_left / (double) expected_moves;
    fprintf(stderr, "%d %d %.3f\n", stats->depth, stats->ply, stats->time);
    fprintf(stderr, "%d %d %d %.3f %.3f\n", depth, expected_moves, branch_factor, time_per_move, time_left);
    fflush(stderr);





    if (fixed_depth != 0) {
        if (iter != 0) {
            return 0;
        }
        else {
            return fixed_depth;
        }
    }

    if (stats->depth == 0) {
        return 2;
    }

    if (stats->time * branch_factor * 0.6 < time_per_move) {
        return depth + 1;
    }
    if (iter != 0) {
        return 0;
    }

    return depth;
}

void game_loop(FILE *fp) {
    board_t move;

    struct timeval search_start, search_end;

    int debug = 0;

    char out[256];

    int depth;
    int move_iterations;
    int fixed_depth = 0;
    int score = 0;

    unsigned int seed;

    search_node sn;
    sn_init(&sn, B_WHITE_START, B_BLACK_START, 0);

    move_stats ms = {0, 0, 0.0};

    // getline variables
    size_t nbytes = 0;
    char *line = NULL;

    while (getline(&line, &nbytes, fp) > 0) {
        fixed_depth = 0;
        statistics.resume();
        if (read_move(line, &move)) {
            sn = sn_apply_move(sn, move);
        } else if (read_quit(line)) {
            statistics.dump_total(stderr);
            break;
        } else if (read_start(line)) {

        } else if (read_comment(line) || read_debug(line, &debug)) {
            continue;
        } else if (read_logstring(line, &sn, &fixed_depth, &score)) {

        } else if (read_debug_moves(line, &sn)) {
            sn_dump(stderr, &sn);
            board_t moves[MAX_MOVES];
            int num_moves = sn_find_moves(&sn, moves);
            for (int m = 0; m < num_moves; ++m) {
                write_move(out, (sn.white | sn.black) & moves[m], ~(sn.white | sn.black) & moves[m]);
                puts(out);

            }
            puts("--------");
            fflush(stdout);
            continue;
        } else if (read_randseed(line, &seed)) {
            srand(seed);
            fprintf(stderr, "Randseed %x\n", seed);
            continue;
        } else if (read_log(line)) {
            fprintf(stderr, LOG_FORMAT_STRING, sn.ply,
                    sn.white.hi,
                    sn.white.low,
                    sn.black.hi,
                    sn.black.low,
                    fixed_depth,
                    score);
            continue;
        } else if (read_dump(line)) {
            sn_dump(stderr, &sn);
            continue;
        } else {
            fprintf(stderr, "Unknown line: %s", line);
            continue;
        }

        if (debug) {
            sn_dump(stderr, &sn);
        }

        move_iterations = 0;
        move = B_EMPTY;

        while ((depth = get_depth(move_iterations, fixed_depth, &sn, &ms, TOTAL_TIME - statistics.time_spent))) {
            fprintf(stderr, "Run with depth %d\n", depth);
            gettimeofday(&search_start, NULL);
            move = negamax_memory_decision(sn, depth, &score);
            gettimeofday(&search_end, NULL);

            fprintf(stderr, LOG_FORMAT_STRING, sn.ply,
                    sn.white.hi,
                    sn.white.low,
                    sn.black.hi,
                    sn.black.low,
                    depth,
                    score);
            fflush(stderr);
            ms = {sn.ply, depth, (search_end.tv_sec + search_end.tv_usec / 1000000.0) - (search_start.tv_sec + search_start.tv_usec / 1000000.0)};

            move_iterations++;

            if (score == WIN_SCORE || score == -WIN_SCORE) break;
        }

        write_move(out, (sn.white | sn.black) & move, ~(sn.white | sn.black) & move);

        sn = sn_apply_move(sn, move);

        if (debug) {
            sn_dump(stderr, &sn);
        }

        if(STORAGE_ID * 2 > STORAGE_SIZE) {
            clear_transposition_table();
            fputs("#Clearing transposition table", stderr);
        }
        statistics.pause();
//        statistics.dump_last(stderr);

        puts(out);
        fflush(stdout);
    }
    statistics.dump_total(stderr);
    free(line);
}

int main(int argc, const char *argv[]) {

    init_stats();

    init_rand(0);

    FILE *fp = stdin;
    if (argc > 1) {
        fp = fopen(argv[1], "r");
    }
    game_loop(fp);

    if (argc > 1) {
        fclose(fp);
    }

    cleanup_stats();

    return 0;
}