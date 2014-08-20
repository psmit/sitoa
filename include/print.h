#pragma once

#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <util.h>

#include "defs.h"
#include "util.h"

#define MAX_ARGUMENTS 128

board_col_t read_move(char * in_string) {
    char c_from, c_to;
    int i_from, i_to;

    sscanf(in_string, "%c%d-%c%d", &c_from, &i_from, &c_to, &i_to);

    board_col_t from = (ONE_128 << (c_from - 'A') + (i_from - 1) * BOARD_SIZE);
    board_col_t to = ONE_128 << (c_to - 'A') + (i_to - 1) * BOARD_SIZE;
    return  from | to ;
}

void write_move(char * out_string, board_col_t from, board_col_t to) {

    int idx_from = ctz128(from);
    int idx_to = ctz128(to);

    sprintf(out_string, "%c%d-%c%d",
            'A'+ idx_from % BOARD_SIZE, (idx_from / BOARD_SIZE) + 1,
            'A'+ idx_to % BOARD_SIZE, (idx_to / BOARD_SIZE) + 1);
}

void visualize_board(char * out_string, char * print_chars, ...) {
    // The size of out_string should be BOARD_SIZE * (BOARD_SIZE + 1) + 1 or bigger

    // The number of characters in print_chars is the amount of boards we expect
    int num_boards = strlen(print_chars);

    board_col_t boards[MAX_ARGUMENTS];

    va_list arguments;
    va_start(arguments, print_chars);

    int board_i;

    for (board_i = 0; board_i < num_boards; ++board_i) {
        boards[board_i] = va_arg(arguments, board_col_t);
    }
    va_end ( arguments );


    // Print boards into string
    int row, col;

    board_col_t cur_cell;

    char cur_char;

    for (row = BOARD_SIZE -1; row >= 0; --row) {
        for (col = 0; col < BOARD_SIZE; ++col) {
            cur_cell = ONE_128 << col + (row* BOARD_SIZE);
            // . is the default 'empty' char
            cur_char = '.';
            for(board_i = 0; board_i < num_boards; ++board_i) {
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

void board_to_hex(char * out_string, board_col_t board) {
    // Use datatype that stores exactly 8 hex chars
    uint32_t p;

    int part;

    for (part = 3; part >= 0; part--) {
        // Move current part to least significant bits, and truncate it with cast
        p = (uint32_t) (board >> part * 32);
        // Print part
        sprintf(out_string, "%08x", p);
        out_string += 8;
    }
    *out_string = '\0';

}

board_col_t hex_to_board(char * in_string) {
    char work_string[33];
    strcpy(work_string, in_string);

    board_col_t board = ZERO_128;

    long long ll;
    int part;

    for (part = 3; part >= 0; part--) {
        // null terminate string of 8 characters that we want to read
        work_string[(part + 1) * 8] = '\0';
        ll = strtoll(work_string + part * 8, NULL, 16);
        // Shift bits to right location
        board |= ((board_col_t) ll ) << (3-part) * 32;
    }

    return board;
}