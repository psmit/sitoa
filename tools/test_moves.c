#include "defs.h"
#include "print.h"


int main( int argc, const char* argv[] ) {
    init();

    board_col_t b = BLACK_START;
    board_col_t move;

    char out[256];

    visualize_board(out, "B", b);
    puts(out);
    fflush(stdout);

    size_t nbytes = 0;
    char *line = NULL;

    while(getline(&line, &nbytes, stdin)) {
        move = read_move(line);
        write_move(out, b & move, ~b & move);
        puts(out);

        b ^= move;

        visualize_board(out, "B", b);
        puts(out);

        board_to_hex(out, b);
        puts(out);

        fflush(stdout);
    }
}