#include "headers.h"

void expand_loop(FILE * fp) {
    search_node sn;


    // getline variables
    size_t nbytes = 0;
    char *line = NULL;

    int depth;
    int score;

    board_t move;

    board_t moves[MAX_MOVES];
    int num_moves;
    while (getline(&line, &nbytes, fp) > 0 && read_logstring(line, &sn, &depth, &score)) {
        statistics.resume();

        num_moves = sn_find_moves(&sn, moves);

        for(int i = 0; i < num_moves; ++i) {
            search_node newsn = sn_apply_move(sn, moves[i]);
            fprintf(stdout, LOG_FORMAT_STRING, newsn.ply,
                    newsn.white.hi,
                    newsn.white.low,
                    newsn.black.hi,
                    newsn.black.low,
                    depth,
                    0);

        }
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

    expand_loop(fp);

    // do codebook calculations

    if (argc > 1) {
        fclose(fp);
    }

    cleanup_stats();

    return 0;
}