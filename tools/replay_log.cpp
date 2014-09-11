#include "headers.h"

void test_loop(FILE * fp) {
    board_t white = B_WHITE_START;
    board_t black = B_BLACK_START;
    char out[256];
    sprintf(out, "%c %016" PRIx64 "%016" PRIx64 " %016" PRIx64 "%016" PRIx64 "\n", 'B', white.hi, white.low, black.hi, black.low);
    fputs(out, stderr);
    fflush(stderr);

    board_t n_white;
    board_t n_black;
    char c;
    int count = sscanf(out, "%c %016" PRIx64 "%016" PRIx64 " %016" PRIx64 "%016" PRIx64 "\n", &c, &n_white.hi, &n_white.low, &n_black.hi, &n_black.low);
    printf("%d\n", count);
}


void replay_loop(FILE * fp) {

    size_t nbytes = 0;
    char *line = NULL;

    unsigned int seed;

    board_t white;
    board_t black;

    char turn;
    int ply;

    board_t possible_moves[MAX_MOVES];

    int num_moves;

    while(getline(&line, &nbytes, fp) > 0)
    {
        if(strlen(line) < 3) continue;

        if (strncmp("Quit", line, 4) == 0 || strncmp("#TRACE", line, 6) == 0) {
            break;
        } else if(line[0] == '#') {
            continue;
        } else if (sscanf(line, "Randseed %x", &seed)) {
            srand(seed);
        } else if (sscanf(line, LOG_FORMAT_STRING, &ply, &turn, &white.hi, &white.low, &black.hi, &black.low) == 6) {
            if (turn == 'B') {

                num_moves = best_negamax_moves(black, white, possible_moves, ply);
            } else {
                num_moves = best_negamax_moves(white, black, possible_moves, ply);
            }
        } else  {
            puts("EROROROROROR");
            exit(1);
        }

    }
    free(line);
}


int main( int argc, const char* argv[] )
{
    FILE * fp = stdin;
    if(argc > 1) {
        fp =fopen(argv[1], "r");
    }
    replay_loop(fp);

    if (argc > 1) {
        fclose(fp);
    }

    return 0;
}