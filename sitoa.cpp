#include <search.h>
#include "headers.h"


void game_loop(FILE *fp) {
    board_t move;

    int debug = 0;

    char out[256];

    int depth;
    int score;

    unsigned int seed;

    search_node sn;
    sn_init(&sn, B_WHITE_START, B_BLACK_START, 0);

    // getline variables
    size_t nbytes = 0;
    char *line = NULL;

    while (getline(&line, &nbytes, fp) > 0) {
        depth = 0;
        statistics.resume();
        if (read_move(line, &move)) {
            sn = sn_apply_move(sn, move);
        } else if (read_quit(line)) {
            statistics.dump_total(stderr);
            break;
        } else if (read_start(line)) {

        } else if (read_comment(line) || read_debug(line, &debug)) {
            continue;
        } else if (read_logstring(line, &sn, &depth, &score)) {

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
                    depth,
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

        if (depth == 0) {
            board_t moves[MAX_MOVES];
            int num_possible_moves = sn_find_moves(&sn, moves);
            if (num_possible_moves > 1) {
                depth = (int)(log((double) NODES_PER_MOVE) / log((double) num_possible_moves + DEPTH_BREAKER)) -1;
                depth = max(depth, 1);
            }
        }

        move = negamax_memory_decision(sn, depth, &score);

        fprintf(stderr, LOG_FORMAT_STRING, sn.ply,
                sn.white.hi,
                sn.white.low,
                sn.black.hi,
                sn.black.low,
                depth,
                score);

        write_move(out, (sn.white | sn.black) & move, ~(sn.white | sn.black) & move);

        sn = sn_apply_move(sn, move);

        if (debug) {
            sn_dump(stderr, &sn);
        }

        statistics.pause();
        statistics.dump_last(stderr);

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