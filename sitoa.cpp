#include <search.h>
#include "headers.h"


void game_loop(FILE *fp) {
    size_t nbytes = 0;
    char *line = NULL;

    search_node sn = {B_WHITE_START, B_BLACK_START, init_hash(B_BLACK_START, B_WHITE_START), 0};

    board_t move;
    int score;
    int debug = 0;
    char out[256];

    char trace[256][10];
    int num_trace = 0;

    int ply;
    int d,s;
    board_t white, black;
    unsigned int seed;
    int depth = 1;


    while (getline(&line, &nbytes, fp)) {
#ifdef USE_STATS
        statistics.resume();
#endif
        strcpy(trace[num_trace++], line);
        if (read_move(line, &move)) {
            sn = sn_apply_move(sn, move);
        } else if (read_quit(line)) {
            statistics.dump_total(stderr);
            break;
        } else if (read_start(line)) {

        } else if (read_comment(line) || read_debug(line, &debug)) {
            continue;
        } else if (read_logstring(line, &sn, &d, &s)) {
            depth = d;
        } else if (read_randseed(line, &seed)) {
            srand(seed);
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


//        depth = 1;
//        board_t moves[MAX_MOVES];
//        int num_possible_moves = sn_find_moves(&sn, moves);
//
//        if (num_possible_moves > 1) {
//            depth = (int)(log((double) NODES_PER_MOVE) / log((double) num_possible_moves + DEPTH_BREAKER)) -1;
//            depth = max(depth, 1);
//        }
//        depth = 1;

        move = negamax_ab_decision(sn, depth, &score);

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

#ifdef USE_STATS
        statistics.pause();
        statistics.dump_last(stderr);
#endif
        puts(out);
        fflush(stdout);
    }
    free(line);
}

int main(int argc, const char *argv[]) {

#ifdef USE_STATS
    init_stats();
#endif

    init_rand(argc > 1 ? argv[1] : "");

    FILE *fp = stdin;
    if (argc > 2) {
        fp = fopen(argv[2], "r");
    }
    game_loop(fp);

    if (argc > 2) {
        fclose(fp);
    }

#ifdef USE_STATS
    cleanup_stats();
#endif

    return 0;
}