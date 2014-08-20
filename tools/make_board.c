#include "defs.h"
#include "print.h"
#include "string.h"


int main( int argc, const char* argv[] ) {
    init();

    char out[256];

    board_col_t board = ZERO_128;

    board_col_t move;

    size_t nbytes = 0;
    char *line = NULL;

    while(getline(&line, &nbytes, stdin)) {
        if (strcmp(line, "Write\n") == 0) {
            visualize_board(out, "B", board);
            puts(out);
            board_to_hex(out, board);
            puts(out);
            fflush(stdout);
        } else if (strcmp(line, "Quit\n") == 0) {
            return;
        } else {
            move = read_move(line);
            board ^= move;
        }
    }

}