#include "headers.h"

typedef std::chrono::high_resolution_clock my_clock;

long time_negamax(search_node *node, int depth, board_t *move) {
    int score;

    auto start = my_clock::now();


    *move = negamax_memory_decision(*node, depth, &score);


    return std::chrono::duration_cast<std::chrono::milliseconds>(my_clock::now()-start).count();
}


void game_loop(FILE *fp) {
    board_t move;

    int debug = 0;

    char out[256];

    int depth;
    int move_iterations;
    int fixed_depth = 0;
    int score = 0;

    unsigned int seed;

    board_t moves[MAX_MOVES];
    int num_moves, num_moves_n;


    search_node sn, sn2;
    sn_init(&sn, B_WHITE_START, B_BLACK_START, 0);

    // getline variables
    size_t nbytes = 0;
    char *line = NULL;

    while (getline(&line, &nbytes, fp) > 0) {
//        statistics.resume();
        if (read_quit(line)) {
            break;
        } else if (strlen(line) < 3) {
            continue;
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

        int max_iter = 4;
        long base_time = 0l;
        while (base_time < 1000l && max_iter > 0) {
            max_iter--;

            num_moves = sn_find_moves(&sn, moves);
            int score_me;
            int score_ot;
            int score_tot;
            sn_scores(&sn, &score_me, &score_ot, &score_tot);


            base_time = time_negamax(&sn, depth, &move);
            if (base_time < 50l && max_iter > 1) {
                depth++;
                continue;
            }

            double times[2];
            int i;



            sn2 = sn_apply_move(sn, move);
            sn_find_moves(&sn2, moves);
            sn2 = sn_apply_move(sn2, moves[0]);
            int score_me_n;
            int score_ot_n;
            int score_tot_n;
            sn_scores(&sn2, &score_me_n, &score_ot_n, &score_tot_n);
            num_moves_n = sn_find_moves(&sn2, moves);


            for (i = 0; i < 2; ++i) {
                times[i] = time_negamax(&sn2, depth + i, &move) / (double)base_time;
            }

            fprintf(stderr, "%ld\n", base_time);
            fflush(stderr);

            printf("%d %d %d %d %d %d %d %d %d %d %.2f %.2f\n",
                    sn.ply,
                    depth,
                    num_moves,
                    score_me,
                    score_ot,
                    score_tot,
                    num_moves_n,
                    score_me_n,
                    score_ot_n,
                    score_tot_n,
                    times[0],
                    times[1]
            );
            fflush(stdout);
            depth++;
            if (STORAGE_ID * 2 > STORAGE_SIZE) {
                fprintf(stderr, "#Clearing transposition table %u \n", STORAGE_ID);
                clear_transposition_table();
                fflush(stderr);
            }
        }


//        if (STORAGE_ID * 2 > STORAGE_SIZE) {
//            fprintf(stderr, "#Clearing transposition table %u \n", STORAGE_ID);
//            clear_transposition_table();
//            fflush(stderr);
//        }
//        statistics.pause();

//        puts(out);

    }
//    statistics.dump_total(stderr);
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