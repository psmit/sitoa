#include "headers.h"

int main(int argc, const char *argv[]) {

    FILE *fp;

    fp = fopen(argv[1], "r");

    char out[256];

    size_t nbytes = 0;
    char *line = NULL;

    board_t board;
    board_t clusters[30];

    int num_clusters;
    int cluster_i;

    while (getline(&line, &nbytes, fp) != -1) {
        board = hex_to_board(line);
        visualize_board(out, "B", board);
        puts(out);
        fflush(stdout);

        num_clusters = find_clusters(board, clusters);
        for (cluster_i = 0; cluster_i < num_clusters; ++cluster_i) {
            visualize_board(out, "BC", board ^ clusters[cluster_i], clusters[cluster_i]);
            puts(out);
        }
        fflush(stdout);
    }

    return 0;

}