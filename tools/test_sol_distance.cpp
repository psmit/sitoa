#include "headers.h"

int main(int argc, const char *argv[]) {

    FILE *fp;

    fp = fopen(argv[1], "r");

    char out[256];

    size_t nbytes = 0;
    char *line = NULL;

    board_t black, white;

    int num_moves;
    int move_i;

    while (getline(&line, &nbytes, fp) != -1) {

        black = hex_to_board(line);
        white = hex_to_board(line + 33);

        visualize_board(out, "BW", black, white);
        puts(out);
        fflush(stdout);


        board_t possible_moves[500];
        num_moves = find_possible_moves(black, white, possible_moves);

        for (move_i = 0; move_i < num_moves; ++move_i) {
            write_move(out, black & possible_moves[move_i], ~black & possible_moves[move_i]);
            puts(out);
        }

        printf("Sol distance: %d\n", find_solution_distance(black, white));
        fflush(stdout);
    }

    return 0;

}