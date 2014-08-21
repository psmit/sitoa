#include "defs.h"
#include "ayu.h"

#include <string.h>


board_col_t get_move(board_col_t my_color, board_col_t other_color) {
    board_col_t possible_moves[500];
    int num_moves = find_possible_moves(my_color, other_color, possible_moves);
    return possible_moves[0];
}

void game_loop() {
    size_t nbytes = 0;
    char *line = NULL;

    board_col_t mycolor = BLACK_START;
    board_col_t othercolor = WHITE_START;

    board_col_t move;

    char out[256];

    while(getline(&line, &nbytes, stdin))
    {
        if (strcmp(line, "Start\n") == 0) {
            mycolor = WHITE_START;
            othercolor = BLACK_START;
        } else if (strcmp(line, "Quit\n") == 0) {
            return;
        } else {
            move = read_move(line);
            othercolor ^= move;
        }

        move = get_move(mycolor, othercolor);
        write_move(out, mycolor & move, ~mycolor & move);

        mycolor ^= move;

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