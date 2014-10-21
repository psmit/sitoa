#include "headers.h"


double time_negamax(search_node *node, int depth, board_t *move) {
    int score;
    struct timeval search_start, search_end;

    gettimeofday(&search_start, NULL);
    *move = negamax_memory_decision(*node, depth, &score);
    gettimeofday(&search_end, NULL);

    double t = (search_end.tv_sec + search_end.tv_usec / 1000000.0) - (search_start.tv_sec + search_start.tv_usec / 1000000.0);
    return t;
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

    board_t moves[MAX_MOVES];
    int num_moves, num_moves_n;


    search_node sn;
    sn_init(&sn, B_WHITE_START, B_BLACK_START, 0);

    // getline variables
    size_t nbytes = 0;
    char *line = NULL;

    while (getline(&line, &nbytes, fp) > 0) {
        statistics.resume();
        if (read_quit(line)) {
            break;
        } else if (read_comment(line) || read_debug(line, &debug)) {
            continue;
        } else if (read_logstring(line, &sn, &depth, &score)) {

        } else {
            fprintf(stderr, "Unknown line: %s", line);
            continue;
        }

        //6 cases. cur+1 cur+2, next-1, next-0, next+1, next+2
        //colums:
        // ply, #moves, depth, sol_dis_me, sol_dis_other, sol_score, time_cur, time_cur+1, time_cur+2, #moves_nextply, next-1, next-0, next+1, next+2

        num_moves = sn_find_moves(&sn, moves);
        int score_me;
        int score_ot;
        int score_tot;
        sn_scores(&sn, &score_me, &score_ot, &score_tot);

        double base_time = time_negamax(&sn, depth, &move);
        double times[7];
        int i;
        for (i = 1; i < 3; ++i) {
            times[i] = time_negamax(&sn, depth + i, &move) / base_time;
        }

        sn = sn_apply_move(sn, move);
        sn_find_moves(&sn, moves);
        sn = sn_apply_move(sn, moves[0]);
        int score_me_n;
        int score_ot_n;
        int score_tot_n;
        sn_scores(&sn, &score_me_n, &score_ot_n, &score_tot_n);
        num_moves_n = sn_find_moves(&sn, moves);

        for (i = -1; i < 3; ++i) {
            times[i + 4] = time_negamax(&sn, depth + i, &move) / base_time;
        }

        printf("%d %d %d %d %d %d %.2f %.2f %d %d %d %d %.2f %.2f %.2f %.2f\n",
                sn.ply,
                depth,
                num_moves,
                score_me,
                score_ot,
                score_tot,
                times[1],
                times[2],
                num_moves_n,
                score_me_n,
                score_ot_n,
                score_tot_n,
                times[3],
                times[4],
                times[5],
                times[6]
        );


        if (STORAGE_ID * 2 > STORAGE_SIZE) {
            clear_transposition_table();
            fputs("#Clearing transposition table", stderr);
            fflush(stderr);
        }
        statistics.pause();

//        puts(out);
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