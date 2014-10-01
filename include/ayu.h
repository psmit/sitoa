#pragma once

#include "headers.h"

board_t find_moves_targets(board_t source, board_t targets, board_t blockades) {
    board_t distances[BOARD_SIZE * BOARD_SIZE];
    board_t cum_field = source;

    unsigned distance = 0;
    distances[distance] = source;

    while (distances[distance] && !(distances[distance] & targets)) {
        distances[++distance] = find_neighbors(cum_field);
        distances[distance] ^= distances[distance] & blockades;
        cum_field |= distances[distance];
    }

    if (!distances[distance]) {
        return B_EMPTY;
    }

    targets &= distances[distance];
    distances[distance] = targets;
    for (; distance > 1; --distance) {
        distances[distance - 1] &= find_neighbors(distances[distance]);
    }

    return distances[1];
}

int find_possible_moves_precalc(board_t board_mover, board_t board_other, board_t *moves, board_t clusters[MAX_VERTICES], int num_clusters, board_t articulation_points[MAX_VERTICES]) {
    statistics.find_moves_count++;

    int num_moves = 0;

    board_t targets, sources, target, source, sources_tmp;

    // Moves are found cluster by cluster
    int cluster_i;
    for (cluster_i = 0; cluster_i < num_clusters; ++cluster_i) {

        //Find target locations (neighbouring, empty spots that bring the cluster one step closer to the closest cluster-neighbour)
        targets = find_moves_targets(clusters[cluster_i], board_mover ^ clusters[cluster_i], board_other);

        //Find source locations in cluster (not breaking up the cluster)
        sources = ~articulation_points[cluster_i] & clusters[cluster_i];

        // Combine sources and targets and check that they are valid

        while (targets) {
            //identify single target
            target = targets.lso();

            // remove target from targets
            targets ^= target;

            // special case if cluster is of size 1, move is always valid
            if (clusters[cluster_i].popcount() == 1) {
                moves[num_moves++] = clusters[cluster_i] | target;
                continue;
            }

            // iterate through different sources
            sources_tmp = sources;
            while (sources_tmp) {
                source = sources_tmp.lso();
                sources_tmp ^= source;

                // source and target combination is valid, if target has another neighbor in cluster besides source
                if ((B_NEIGHBOURS[target.ctz()] ^ source) & clusters[cluster_i]) {
                    moves[num_moves++] = source | target;
                }
            }
        }
    }

    return num_moves;
}

int find_solution_distance(board_t board, board_t other) {
    statistics.find_solution_distance_count++;

    // Solution distance is the amounts of steps needed on the current board to end with 0 possible moves

    // Building minimum spanning tree by taking a base cluster
    //  - add closest cluster and record distance
    // - repeat
    // If no closest clusters, but there are clusters left: start new base cluster
    int sol_distance = 0;

    board_t clusters[MAX_VERTICES];
    int num_clusters = find_clusters(board, clusters, 0);

    int base_cluster = 0;

    // we stop when we have the same amount base clusters and clusters
    while (base_cluster < num_clusters - 1) {

        board_t cum_field = clusters[base_cluster];
        int distance = 0;

        board_t neighbours = find_neighbors(cum_field);
        neighbours &= ~other;

        // Grow our field until we find a piece of our own
        while (neighbours && !(neighbours & board)) {
            cum_field |= neighbours;
            neighbours = find_neighbors(cum_field);
            neighbours &= ~cum_field;
            neighbours &= ~other;
            distance++;
        }

        // either we found a reachable cluster, or we can't reach anything anymore
        if (neighbours & board) {
            // we know we can reach a cluster, find out which one it is
            int cluster_i;
            for (cluster_i = base_cluster + 1; cluster_i < num_clusters; ++cluster_i) {
                if (neighbours & clusters[cluster_i]) {
                    // add the cluster to the base and remove it from the list
                    clusters[base_cluster] |= clusters[cluster_i];
                    clusters[cluster_i] = clusters[--num_clusters];

                    sol_distance += distance;
                    // add only one cluster at a time
                    break;
                }
            }
        } else {
            // create a new base cluster
            base_cluster++;
        }

    }

    return sol_distance;
}

int find_solution_distance_precalc(board_t board, board_t other, int num_clusters, board_t clusters[MAX_VERTICES]) {
    statistics.find_solution_distance_count++;

    // Solution distance is the amounts of steps needed on the current board to end with 0 possible moves

    // Building minimum spanning tree by taking a base cluster
    //  - add closest cluster and record distance
    // - repeat
    // If no closest clusters, but there are clusters left: start new base cluster
    int sol_distance = 0;
    int base_cluster = 0;

    // we stop when we have the same amount base clusters and clusters
    while (base_cluster < num_clusters - 1) {

        board_t cum_field = clusters[base_cluster];
        int distance = 0;

        board_t neighbours = find_neighbors(cum_field);
        neighbours &= ~other;

        // Grow our field until we find a piece of our own
        while (neighbours && !(neighbours & board)) {
            cum_field |= neighbours;
            neighbours = find_neighbors(cum_field);
            neighbours &= ~cum_field;
            neighbours &= ~other;
            distance++;
        }

        // either we found a reachable cluster, or we can't reach anything anymore
        if (neighbours & board) {
            // we know we can reach a cluster, find out which one it is
            int cluster_i;
            for (cluster_i = base_cluster + 1; cluster_i < num_clusters; ++cluster_i) {
                if (neighbours & clusters[cluster_i]) {
                    // add the cluster to the base and remove it from the list
                    clusters[base_cluster] |= clusters[cluster_i];
                    clusters[cluster_i] = clusters[--num_clusters];

                    sol_distance += distance;
                    // add only one cluster at a time
                    break;
                }
            }
        } else {
            // create a new base cluster
            base_cluster++;
        }

    }

    return sol_distance;
}