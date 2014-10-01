#pragma once

#include "headers.h"

int read_log(const char * line) {
    return strncmp("LOG", line, 3) == 0;
}

int read_dump(const char * line) {
    return strncmp("DUMP", line, 4) == 0;
}

int read_debug(const char * line, int * debug) {
    return sscanf(line, "DEBUG %d", debug) == 1;
}

int read_logstring(const char * line, search_node *sn, int * depth, int * score) {
    int ply;
    board_t white, black;
    int d, s;
    if (sscanf(line, LOG_FORMAT_STRING, &ply, &white.hi, &white.low, &black.hi, &black.low, &d, &s) == 7) {
        *depth = d;
        *score = s;
        sn_init(sn, white, black, ply);
        return 1;
    }
    return 0;
}

int read_move(const char * line, board_t * move) {
    char c_from, c_to;
    int i_from, i_to;
    if (sscanf(line, "%c%d-%c%d", &c_from, &i_from, &c_to, &i_to) == 4) {
        *move = B_SINGLE_BIT[(c_from - 'A') + (i_from - 1) * BOARD_SIZE] | B_SINGLE_BIT[(c_to - 'A') + (i_to - 1) * BOARD_SIZE];
        return 1;
    }
    return 0;
}

int read_start(const char * line) {
    return strncmp("Start", line, 5) == 0;
}

int read_quit(const char * line) {
    return strncmp("Quit", line, 4) == 0;
}

int read_comment(const char * line) {
    return strncmp("#", line, 1) == 0;
}

int read_randseed(const char * line, unsigned int * seed) {
    return sscanf(line, "Randseed %x", seed) == 1;
}

void write_move(char *out_string, board_t from, board_t to) {

    int idx_from = from.ctz();
    int idx_to = to.ctz();

    sprintf(out_string, "%c%d-%c%d",
            'A' + idx_from % BOARD_SIZE, (idx_from / BOARD_SIZE) + 1,
            'A' + idx_to % BOARD_SIZE, (idx_to / BOARD_SIZE) + 1);
}

