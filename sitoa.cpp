#include "headers.h"

typedef std::chrono::high_resolution_clock my_clock;

void game_loop(FILE *fp) {
    clear_transposition_table();
    board_t move;

    auto start = my_clock::now();
    long total_time = 0l;
    long move_time = 1000l;

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
                write_move(out, (sn.board[C_WHITE] | sn.board[C_BLACK]) & moves[m], ~(sn.board[C_WHITE] | sn.board[C_BLACK]) & moves[m]);
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
                    sn.board[C_WHITE].hi,
                    sn.board[C_WHITE].low,
                    sn.board[C_BLACK].hi,
                    sn.board[C_BLACK].low,
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

            if (total_time > 25 * 1000) {
                depth = max(4, depth-2);
            } else {
                int expected_moves_left = max(1, sn_min_solution_distance(&sn)) * 2;

                long time_per_move = (29*1000 - total_time) / (expected_moves_left / 2);

                fprintf(stderr, "#%d %ld %ld %ld\n", expected_moves_left,
                        move_time, time_per_move, total_time);
                if (move_time * 3 < time_per_move) {
                    depth++;
                } else if (move_time > 3 * time_per_move) {
                    depth--;
                }
            }
        }

        move = negamax_memory_decision(sn, depth, &score);
        move_time = std::chrono::duration_cast<std::chrono::milliseconds>(my_clock::now() - start).count();
        fprintf(stderr, LOG_FORMAT_STRING, sn.ply,
                sn.board[C_WHITE].hi,
                sn.board[C_WHITE].low,
                sn.board[C_BLACK].hi,
                sn.board[C_BLACK].low,
                depth,
                score);

        fflush(stderr);

        write_move(out, (sn.board[C_WHITE] | sn.board[C_BLACK]) & move, ~(sn.board[C_WHITE] | sn.board[C_BLACK]) & move);

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
        total_time += std::chrono::duration_cast<std::chrono::milliseconds>(my_clock::now() - start).count();
    }
    total_time += std::chrono::duration_cast<std::chrono::milliseconds>(my_clock::now() - start).count();

    fprintf(stderr, "Total time: %.2f seconds\n", total_time / 1000.0);
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