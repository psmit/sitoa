#include "graph.h"

int main( int argc, const char* argv[] ) {

    FILE * fp;

    fp = fopen(argv[1], "r");

    char out[256];

    size_t nbytes = 0;
    char *line = NULL;

    board_t board, articulation_points;

    while(getline(&line, &nbytes, fp) != -1) {
        board = hex_to_board(line);
        visualize_board(out, "B", board);
        puts(out);
        fflush(stdout);

        articulation_points = find_articulation_points(board);
        visualize_board(out, "BA", board ^ articulation_points, articulation_points);
        puts(out);
        fflush(stdout);
    }

    return 0;

}