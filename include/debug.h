#pragma once

#include "headers.h"
void visualize_board(char *out_string, const char *print_chars, ...) {
    // The size of out_string should be BOARD_SIZE * (BOARD_SIZE + 1) + 1 or bigger

    // The number of characters in print_chars is the amount of boards we expect
    int num_boards = strlen(print_chars);

    board_t boards[MAX_ARGUMENTS];

    va_list arguments;
    va_start(arguments, print_chars);

    int board_i;

    for (board_i = 0; board_i < num_boards; ++board_i) {
        boards[board_i] = va_arg(arguments, board_t);
    }
    va_end (arguments);


    // Print boards into string
    int row, col;

    board_t cur_cell;

    char cur_char;

    for (row = BOARD_SIZE - 1; row >= 0; --row) {
        for (col = 0; col < BOARD_SIZE; ++col) {
            cur_cell = B_SINGLE_BIT[col + row * BOARD_SIZE];
            // . is the default 'empty' char
            cur_char = '.';
            for (board_i = 0; board_i < num_boards; ++board_i) {
                if (cur_cell & boards[board_i]) {
                    if (cur_char == '.') {
                        // if the char hasn't been changed yet we make it the appropriate char
                        cur_char = print_chars[board_i];
                    } else {
                        // else there is an overlap and we use !
                        cur_char = '!';
                    }
                }
            }
            *(out_string++) = cur_char;
        }
        // Terminate every row with a newline
        *(out_string++) = '\n';
    }
    // Terminate the string properly
    *(out_string) = '\0';
}

void visualize_board_int(char *out_string, int *board) {
    // The size of out_string should be BOARD_SIZE * (BOARD_SIZE + 1) + 1 or bigger

    // Print boards into string
    int row, col;

    int cell_id;


    for (row = BOARD_SIZE - 1; row >= 0; --row) {
        for (col = 0; col < BOARD_SIZE; ++col) {
            cell_id = col + row * BOARD_SIZE;
            out_string += sprintf(out_string, "%2d ", board[cell_id]);
        }
        // Terminate every row with a newline
        *(out_string++) = '\n';
    }
    // Terminate the string properly
    *(out_string) = '\0';
}

void board_to_hex(char *out_string, board_t board) {
    // Use datatype that stores exactly 8 hex chars

    sprintf(out_string, BOARD_FORMAT_STRING, board.hi, board.low);
    out_string += 32;
    *out_string = '\0';
}