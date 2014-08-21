#pragma once
#include "defs.h"
#include "graph.h"

board_col_t find_moves_targets(board_col_t source, board_col_t targets, board_col_t blockades) {
//    targets ^= source;

    board_col_t distances[BOARD_SIZE * BOARD_SIZE];
    board_col_t cum_field = source;

    unsigned distance = 0;
    distances[distance] = source;

    while(distances[distance] && !(distances[distance] & targets)) {
        distances[++distance] = find_neighbors(cum_field);
        distances[distance] ^= distances[distance] & blockades;
        cum_field |= distances[distance];
    }

    if(!distances[distance]) {
        return ZERO_128;
    }

    targets &= distances[distance];
    distances[distance] = targets;
    for (; distance > 1; --distance) {
        distances[distance - 1] &= find_neighbors(distances[distance]);
    }

    return distances[1];
}

int find_possible_moves(board_col_t board_mover, board_col_t board_other, board_col_t * moves) {

    int num_moves = 0;

    board_col_t targets, sources, target, source, sources_tmp;

    // Moves are found cluster by cluster
    board_col_t clusters[MAX_VERTICES];
    int num_clusters = find_clusters(board_mover, clusters);

    int cluster_i;
    for (cluster_i = 0; cluster_i < num_clusters; ++cluster_i) {

        //Find target locations (neighbouring, empty spots that bring the cluster one step closer to the closest cluster-neighbour)
        targets = find_moves_targets(clusters[cluster_i], board_mover ^ clusters[cluster_i], board_other);

        //Find source locations in cluster (not breaking up the cluster)
        sources = ~find_articulation_points(clusters[cluster_i]) & clusters[cluster_i];

        // Combine sources and targets and check that they are valid

        while (targets) {
            //identify single target
            target = targets & (-targets);
            // remove target from targets
            targets ^= target;

            // special case if cluster is of size 1, move is always valid
            if (popcount128(clusters[cluster_i]) == 1) {
                moves[num_moves++] = clusters[cluster_i] | target;
                continue;
            }

            // iterate through different sources
            sources_tmp = sources;
            while(sources_tmp) {
                source = sources_tmp & (-sources_tmp);
                sources_tmp ^= source;

                // source and target combination is valid, if target has another neighbor in cluster besides source
                if ((NEIGHBORS[ctz128(target)] ^ source) & clusters[cluster_i]) {
                    moves[num_moves++] = source | target;
                }
            }
        }
    }

    return num_moves;
}