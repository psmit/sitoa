#pragma once

#include <cstdlib>
#include <sys/time.h>

#define USE_STATS true

#if USE_STATS
struct stats {

    stats * prev_stats;
    double time_spent;

    struct timeval last_start;

    unsigned long long find_neighbours_count;
    unsigned long long negamax_count;
    unsigned long long find_moves_count;
    unsigned long long find_solution_distance_count;

    void resume() {
        gettimeofday(&last_start, NULL);

        prev_stats->time_spent = time_spent;
        prev_stats->find_neighbours_count = find_neighbours_count;
        prev_stats->negamax_count = negamax_count;
        prev_stats->find_moves_count = find_moves_count;
        prev_stats->find_solution_distance_count = find_solution_distance_count;
    }

    void pause() {
        struct timeval end;
        gettimeofday(&end, NULL);
        time_spent += (end.tv_sec + end.tv_usec / 1000000.0) - (last_start.tv_sec + last_start.tv_usec / 1000000.0);
    }

    void dump_last(FILE * fp) {
        fprintf(fp, "Since last time: %.4f seconds; Nbs: %li Negam: %li Fmv: %li Fsoldis: %li\nFindNei/second %.1f; Nodes/second %.1f;\n",
        time_spent - prev_stats->time_spent,
        find_neighbours_count - prev_stats->find_neighbours_count,
        negamax_count - prev_stats->negamax_count,
        find_moves_count - prev_stats->find_moves_count,
        find_solution_distance_count - prev_stats->find_solution_distance_count,
        (find_neighbours_count - prev_stats->find_neighbours_count) / (time_spent - prev_stats->time_spent),
        (negamax_count - prev_stats->negamax_count) / (time_spent - prev_stats->time_spent)
        );
    }

    void dump_total(FILE * fp) {
        fprintf(fp, "Total time: %.4f seconds;\n", time_spent);
    }

    void cleanup() {
        delete prev_stats;
    }
};

stats statistics = {0};

void init_stats() {
    statistics.prev_stats = new stats();
}

#endif