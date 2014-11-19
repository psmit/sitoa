#pragma once

#include "headers.h"

struct search_node {
    board_t board[2];
    hash_t hash;
    int ply;

    int num_clusters[2];
    board_t clusters[2][MAX_VERTICES];
    board_t cluster_neighbours[2][MAX_VERTICES];
    board_t cluster_articulation_points[2][MAX_VERTICES];

};

search_node sn_apply_move(search_node n, board_t move) {
    int color = n.ply % 2 ? C_BLACK : C_WHITE;
    n.board[color] ^= move;
    n.hash ^= rand_table[(n.board[color] & move).ctz()][color] ^ rand_table[(~n.board[color] & move).ctz()][color];


    board_t reval = n.board[color];
    n.ply++;


    board_t move_to = n.board[color] & move;


    // Keep the clusters and cluster_neighbours variables up-to-date
    int cluster = 0;


    while (cluster < n.num_clusters[color]) {
        // If the clusters borders with the "to" location,
        if (n.cluster_neighbours[color][cluster] & move_to) {
            // Remove the cluster by overriding it with the last cluster and decrementing the amount of clusters
            --n.num_clusters[color];
            n.clusters[color][cluster] = n.clusters[color][n.num_clusters[color]];
            // dont forget to do the same for parallel arrays
            n.cluster_neighbours[color][cluster] = n.cluster_neighbours[color][n.num_clusters[color]];
            n.cluster_articulation_points[color][cluster] = n.cluster_articulation_points[color][n.num_clusters[color]];
        } else {
            // If this cluster is not affected we remove it from reval, it does not need to be re-evaluated again.
            reval ^= n.clusters[color][cluster];
            // go to the next cluster index
            cluster++;
        }
    }

    //Everything in reval are new clusters, find them
    int new_clusters = find_clusters(reval, n.clusters[color], n.num_clusters[color]);

    // update parallel arrays
    for (cluster = n.num_clusters[color]; cluster < new_clusters; ++cluster) {
        n.cluster_neighbours[color][cluster] = find_neighbors(n.clusters[color][cluster]);
        n.cluster_articulation_points[color][cluster] = find_articulation_points(n.clusters[color][cluster]);
    }
    n.num_clusters[color] = new_clusters;
    return n;
}

int sn_find_moves(search_node const * node, board_t * moves) {
    int color = node->ply % 2 ? C_BLACK : C_WHITE;

    return find_possible_moves_precalc(node->board[color], node->board[1-color], moves,
            node->clusters[color], node->num_clusters[color], node->cluster_articulation_points[color]);
}

int sn_score(search_node const * node) {
    int score_black = find_solution_distance_precalc(node->board[C_BLACK], node->board[C_WHITE], node->num_clusters[C_BLACK], node->clusters[C_BLACK]);
    int score_white = find_solution_distance_precalc(node->board[C_WHITE], node->board[C_BLACK], node->num_clusters[C_WHITE], node->clusters[C_WHITE]);

    if (node->ply % 2 == 1) {
        return score_white - score_black;
    } else {
        return score_black - score_white;
    }
}

void sn_scores(search_node const * node, int * score_me, int * score_oth, int * score_tot) {
    int score_black = find_solution_distance_precalc(node->board[C_BLACK], node->board[C_WHITE], node->num_clusters[C_BLACK], node->clusters[C_BLACK]);
    int score_white = find_solution_distance_precalc(node->board[C_WHITE], node->board[C_BLACK], node->num_clusters[C_WHITE], node->clusters[C_WHITE]);

    if (node->ply % 2 == 1) {
        *score_me = score_white;
        *score_oth = score_black;
        *score_tot = score_white - score_black;
    } else {
        *score_me = score_black;
        *score_oth = score_white;
        *score_tot = score_black - score_white;
    }
}

int sn_min_solution_distance(search_node const * node) {
    int score_black = find_solution_distance_precalc(node->board[C_BLACK], node->board[C_WHITE], node->num_clusters[C_BLACK], node->clusters[C_BLACK]);
    int score_white = find_solution_distance_precalc(node->board[C_WHITE], node->board[C_BLACK], node->num_clusters[C_WHITE], node->clusters[C_WHITE]);
    return min(score_black, score_white);
}

void sn_dump(FILE * out, search_node * node) {
    fprintf(out, "Dump Search node\nPly %d; hash %016" PRIx64 "\n", node->ply, node->hash);
    char out_s[256];
    visualize_board(out_s, "BW", node->board[C_BLACK], node->board[C_WHITE]);
    fputs(out_s, out);

    fprintf(out, "Score Black: %d, White: %d, Node score: %d Player: %s\n",
            find_solution_distance(node->board[C_BLACK], node->board[C_WHITE]),
            find_solution_distance(node->board[C_WHITE], node->board[C_BLACK]),
            sn_score(node), node->ply % 2 ? "Black": "White");
}

void sn_init(search_node * sn, board_t white, board_t black, int ply) {
    sn->board[C_WHITE] = white;
    sn->board[C_BLACK] = black;
    sn->ply = ply;
    sn->hash = init_hash(white, black);
    sn->num_clusters[C_WHITE] = find_clusters(white, sn->clusters[C_WHITE], 0);
    sn->num_clusters[C_BLACK] = find_clusters(black, sn->clusters[C_BLACK], 0);
    int color, cluster;
    for(color = 0; color < 2; ++color) {
        for(cluster = 0; cluster < sn->num_clusters[color]; ++cluster) {
            sn->cluster_neighbours[color][cluster] = find_neighbors(sn->clusters[color][cluster]);
            sn->cluster_articulation_points[color][cluster] = find_articulation_points(sn->clusters[color][cluster]);
        }
    }
}