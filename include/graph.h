#pragma once
#include "defs.h"
#include "print.h"

board_col_t find_neighbors(board_col_t board) {
    // initialize neighbors with board to be able to filter them on the end
    board_col_t neighbors = board;
    // create working board b
    board_col_t b = board;

    board_col_t piece;

    // loop through all pieces one at a time
    while (b) {
        //identify single piece
        piece = b & (-b);
        // remove piece from working board
        b ^= piece;

        // Add neighbors of piece to neighbors collection
        neighbors |= NEIGHBORS[ctz128(piece)];
    }

    // remove board from neighbors before returning it
    return neighbors ^ board;
}

int find_clusters(board_col_t board, board_col_t clusters[MAX_VERTICES]) {
    board_col_t cluster;
    board_col_t neighbors;
    int num_clusters = 0;

    memset(clusters, 0, sizeof clusters);

    while(board) {
        //initialize cluster with first stone
        cluster = board & (-board);
        //remove stones in cluster from board
        board ^= cluster;

        // expand cluster with it's neighbors
        while (1) {
            // TODO would it be more efficient to search with only new neighbors?
            neighbors = find_neighbors(cluster);

            if (board & neighbors) {
                // add neighbors that are also on the board
                cluster |= (board & neighbors);
                // and remove them from the board
                board ^= (board & neighbors);
            } else {
                break;
            }
        }

        clusters[num_clusters++] = cluster;
    }

    return num_clusters;
}


void find_articulation_points_recursive(board_col_t graph, board_col_t un, int ui, int * time, board_col_t * visited, int disc[BOARD_SIZE * BOARD_SIZE],
        int low[BOARD_SIZE * BOARD_SIZE], int parent[BOARD_SIZE * BOARD_SIZE], board_col_t * articulation_points) {

    int children = 0;

    // mark node visited
    (*visited) |= un;

    disc[ui] = low[ui] = ++(*time);

    board_col_t neighbors = NEIGHBORS[ui];
    board_col_t nn;
    int ni;

    while(neighbors) {
        nn = neighbors & (-neighbors);
        neighbors ^= nn;

        ni = ctz128(nn);

        if(! (nn & graph)) continue;

        if(nn & ~(*visited)) {
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

board_col_t find_articulation_points(board_col_t graph) {
    // max vertices = 30

    int parent[BOARD_SIZE * BOARD_SIZE];
    int disc[BOARD_SIZE * BOARD_SIZE];
    int low[BOARD_SIZE * BOARD_SIZE];

    board_col_t visited = ZERO_128;
    board_col_t articulation_points = ZERO_128;

    int i;
    for (i = 0; i < (BOARD_SIZE * BOARD_SIZE); ++i) {
        parent[i] = -1;
        disc[i] = 0;
        low[i] = 0;
    }

    board_col_t un;
    int ui;

    int time = 0;

    board_col_t wgraph = graph;

    while(wgraph) {
        un = wgraph & (-wgraph);
        wgraph ^= un;
        if(un & ~visited) {
            ui = ctz128(un);
            find_articulation_points_recursive(graph, un, ui, &time, &visited, disc, low, parent, &articulation_points);
        }
    }

    return articulation_points;
}



