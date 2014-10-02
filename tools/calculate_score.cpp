#define BIG_MEMORY
#define DETERMINISTIC
#define DISABLE_INTERESING_MOVE_COUNT
#include "headers.h"

void codebook_loop(FILE * fp) {
    search_node sn;

    // getline variables
    size_t nbytes = 0;
    char *line = NULL;

    int depth;
    int score;

    board_t move;
    char out[256];

    while (getline(&line, &nbytes, fp) > 0 && read_logstring(line, &sn, &depth, &score)) {
        statistics.resume();
        move = negamax_memory_decision(sn, depth, &score);

        write_move(out, (sn.white | sn.black) & move, ~(sn.white | sn.black) & move);
        fprintf(stdout, "%#lx %s\n", sn.hash, out );

        sn = sn_apply_move(sn, move);

        fprintf(stderr, LOG_FORMAT_STRING, sn.ply,
                sn.white.hi,
                sn.white.low,
                sn.black.hi,
                sn.black.low,
                depth,
                0);

        fflush(stderr);
        fflush(stdout);
        statistics.pause();
//        statistics.dump_last(stderr);

    }
}

int main(int argc, const char *argv[]) {

    init_stats();

    init_rand(0);

    FILE *fp = stdin;
    if (argc > 1) {
        fp = fopen(argv[1], "r");
    }

    codebook_loop(fp);

    // do codebook calculations

    if (argc > 1) {
        fclose(fp);
    }

    cleanup_stats();

    return 0;
}