#include <string.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include "defs.h"
#include "print.h"
#include "util.h"
#include "graph.h"


board_col_t find_moves(board_col_t source, board_col_t targets, board_col_t blockades) {
    // make sure the source does not overlap with the targets
    targets ^= source;

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




board_col_t get_move(board_col_t mycolor, board_col_t othercolor) {
    board_col_t clusters[MAX_VERTICES];
    int num_clusters = find_clusters(mycolor, clusters);
    return read_move("A2-B2");
}




void game_loop() {
    size_t nbytes = 0;
    char *line = NULL;

    board_col_t mycolor = WHITE_START;
    board_col_t othercolor = BLACK_START;

    board_col_t move;

    char out[256];

    while(getline(&line, &nbytes, stdin))
    {
        if (strcmp(line, "Start\n") == 0) {
            mycolor = BLACK_START;
            othercolor = WHITE_START;
        } else if (strcmp(line, "Quit\n") == 0) {
            return;
        } else {
            move = read_move(line);
            othercolor ^= move;
        }

        move = get_move(mycolor, othercolor);
        write_move(out, mycolor & move, ~mycolor & move);
        puts(out);
        fflush(stdout);
    }
    free(line);
}

int main( int argc, const char* argv[] )
{
    init();
    game_loop();
    return 0;
}