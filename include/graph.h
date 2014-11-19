#pragma once

#include "headers.h"

/*
 * Find the direct neighbours of all bits set in board, excluding the bits set in board itself.
 */
inline // inlining seems to be 10% faster
board_t find_neighbors(const board_t &board) {
    // Superfast way of finding all neighbors on a board. The B_HAS_ vectors make sure that a board has the NORTH/WEST/EAST neighbour and the bitshifts do the rest
    return (board | // if we start with a board we can filter it with an xor later
            (board >> 11) | // South goes always right, the shift will drop the extra bits
            ((board & B_HAS_NORTH) << 11) |
            ((board & B_HAS_WEST) >> 1) |
            ((board & B_HAS_EAST) << 1))
            ^ board; // at last the xor to remove the original board
}

/*
 * Find all clusters in board and add them to clusters.
 *
 * This method can be used for incremental updates, it will add the clusters at index num_clusters
 *
 */

int find_clusters(board_t board, board_t clusters[MAX_VERTICES], int num_clusters) {
    board_t cluster;
    board_t neighbors;

    while (board) {
        //initialize cluster with first stone
        cluster = board.lso();
        //remove stones in cluster from board
        board ^= cluster;

        // expand cluster with it's neighbors
        while (1) {
            neighbors = find_neighbors(cluster) & board;

            if (neighbors) {
                // add neighbors that are also on the board
                cluster |= (neighbors);
                // and remove them from the board
                board ^= (neighbors);
            } else { // there is no new neighbors to add
                break;
            }
        }

        clusters[num_clusters++] = cluster;
    }

    return num_clusters;
}


void find_articulation_points_recursive(board_t graph, board_t un, int ui, int *time, board_t *visited, int disc[BOARD_SIZE * BOARD_SIZE],
        int low[BOARD_SIZE * BOARD_SIZE], int parent[BOARD_SIZE * BOARD_SIZE], board_t *articulation_points) {

    int children = 0;

    // mark node visited
    (*visited) |= un;

    disc[ui] = low[ui] = ++(*time);

    board_t neighbors = B_NEIGHBOURS[ui];
    board_t nn;
    int ni;

    while (neighbors) {
        nn = neighbors.lso();
        neighbors ^= nn;

        ni = nn.ctz();

        if (!(nn & graph)) continue;

        if (nn & ~(*visited)) {
            children++;
            parent[ni] = ui;
            find_articulation_points_recursive(graph, nn, ni, time, visited, disc, low, parent, articulation_points);

            low[ui] = min(low[ui], low[ni]);

            if (parent[ui] < 0 && children > 1) {
                (*articulation_points) |= un;
            }

            if (parent[ui] >= 0 && low[ni] >= disc[ui]) {
                (*articulation_points) |= un;
            }

        } else if (ni != parent[ui]) {
            low[ui] = min(low[ui], disc[ni]);
        }
    }
}

/*
 * Algorithm to find the articulation points in a graph. In Ayu, only non-articulation points can be used to move from
 * (except for single stone clusters)
 */

board_t find_articulation_points(board_t graph) {
    int parent[BOARD_SIZE * BOARD_SIZE];
    int disc[BOARD_SIZE * BOARD_SIZE];
    int low[BOARD_SIZE * BOARD_SIZE];

    board_t visited = B_EMPTY;
    board_t articulation_points = B_EMPTY;

    int i;
    for (i = 0; i < (BOARD_SIZE * BOARD_SIZE); ++i) {
        parent[i] = -1;
        disc[i] = 0;
        low[i] = 0;
    }

    board_t un;
    int ui;

    int time = 0;

    board_t wgraph = graph;

    while (wgraph) {
        un = wgraph.lso();
        wgraph ^= un;
        if (un & ~visited) {
            ui = un.ctz();
            find_articulation_points_recursive(graph, un, ui, &time, &visited, disc, low, parent, &articulation_points);
        }
    }

    return articulation_points;
}