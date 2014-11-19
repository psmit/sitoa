#include "headers.h"

typedef std::chrono::high_resolution_clock my_clock;

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

    auto start = my_clock::now();
    auto total_time = start-start;


    auto move_time = start-start;
    move_time += std::chrono::seconds(1);
    int debug = 0;

    char out[256];

    int depth = 2;
    int fixed_depth = 0;
    int score = 0;

    unsigned int seed;

    search_node sn;
    sn_init(&sn, B_WHITE_START, B_BLACK_START, 0);


    // getline variables
    size_t nbytes = 0;
    char *line = NULL;

    while (getline(&line, &nbytes, fp) > 0) {
        start = my_clock::now();

        fixed_depth = 0;
        if (read_move(line, &move)) {
            sn = sn_apply_move(sn, move);
        } else if (read_quit(line)) {
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

        move = B_EMPTY;

        if (fixed_depth != 0) {
            depth = fixed_depth;
        } else {

            if (total_time > std::chrono::seconds(25)) {
                depth = 4;

            } else {

                int score1, score2, score_tot;

                sn_scores(&sn, &score1, &score2, &score_tot);
                int expected_moves_left = max(1,min(score1, score2)) * 2;

                auto time_per_move = std::chrono::milliseconds(29000) / expected_moves_left;

                fprintf(stderr, "#%d %ld %ld %ld\n", expected_moves_left,
                        std::chrono::duration_cast<std::chrono::milliseconds>(move_time).count(),
                        std::chrono::duration_cast<std::chrono::milliseconds>(time_per_move).count(),
                        std::chrono::duration_cast<std::chrono::milliseconds>(total_time).count());
                if (move_time * 3 < time_per_move) {
                    depth++;
                } else if (move_time > 3 * time_per_move) {
                    depth--;
                }
            }
        }

        move = negamax_memory_decision(sn, depth, &score);
        move_time = my_clock::now() - start;
        fprintf(stderr, LOG_FORMAT_STRING, sn.ply,
                sn.white.hi,
                sn.white.low,
                sn.black.hi,
                sn.black.low,
                depth,
                score);

//        fprintf(stderr, "#Move took %ld milliseconds\n", std::chrono::duration_cast<std::chrono::milliseconds>(move_time).count());
        fflush(stderr);

        write_move(out, (sn.white | sn.black) & move, ~(sn.white | sn.black) & move);

        sn = sn_apply_move(sn, move);

        if (debug) {
            sn_dump(stderr, &sn);
        }

        if(STORAGE_ID * 2 > STORAGE_SIZE) {
            fprintf(stderr, "#Clearing transposition table %u \n", STORAGE_ID);
            clear_transposition_table();
        }

        puts(out);
        fflush(stdout);
        total_time += my_clock::now() - start;
    }
    total_time += my_clock::now() - start;

    fprintf(stderr, "Total time: %.2f seconds\n", std::chrono::duration_cast<std::chrono::milliseconds>(total_time).count() / 1000.0);
    fflush(stderr);

    free(line);
}

int main(int argc, const char *argv[]) {

    init_rand(0);

    FILE *fp = stdin;
    if (argc > 1) {
        fp = fopen(argv[1], "r");
    }
    game_loop(fp);

    if (argc > 1) {
        fclose(fp);
    }

    return 0;
}