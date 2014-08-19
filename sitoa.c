#include <string.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>


#define BOARD_SIZE 11
#define MAX_VERTICES 30

typedef __uint128_t board_col_t;

board_col_t NEIGHBORS[BOARD_SIZE * BOARD_SIZE];

board_col_t ZERO_128 = 0;
board_col_t ONE_128 = 1;

board_col_t BLACK_START;
board_col_t WHITE_START;

void init() {
    int x, y;
    board_col_t neighbors;
    for (x = 0; x < BOARD_SIZE; ++x) {
        for (y = 0; y < BOARD_SIZE; ++y) {
            neighbors = 0;

            if(x > 0) {
                neighbors |= ONE_128 << (x-1) * BOARD_SIZE + y;
            }

            if(y > 0) {
                neighbors |= ONE_128 << x * BOARD_SIZE + (y-1);
            }

            if(x < BOARD_SIZE - 1) {
                neighbors |= ONE_128 << (x+1) * BOARD_SIZE + y;
            }

            if(y < BOARD_SIZE - 1) {
                neighbors |= ONE_128 << x * BOARD_SIZE + (y+1);
            }

            NEIGHBORS[x* BOARD_SIZE + y] = neighbors;
        }
    }

//    BLACK_START = 426;
    BLACK_START = 682;
    BLACK_START = BLACK_START | BLACK_START << 22 | BLACK_START << 44 | BLACK_START << 66 | BLACK_START << 88 | BLACK_START << 110;
    WHITE_START = 1365;
    WHITE_START = WHITE_START << 11 | WHITE_START << 33 | WHITE_START << 55 | WHITE_START << 77 | WHITE_START << 99;
}

int ctz128(board_col_t x) {
    // Find trailing zeros for 128 bit variable
    union {
        board_col_t y;
        uint64_t yp[2];
    } u;
    u.y = x;

    if (u.yp[0]) {
        return __builtin_ctzll(u.yp[0]);
    } else{
        return 64 + __builtin_ctzll(u.yp[1]);
    }

}

board_col_t find_neighbors(board_col_t board) {
    // initialize neighbors with board to be able to filter them on the end
    board_col_t neighbors = board;
    // create working board b
    board_col_t b = board;

    board_col_t piece;

    // loop through all pieces one at a time
    while (b) {
        //identify single piece
        piece = b & (-b);
        // remove piece from working board
        b ^= piece;

        // Add neighbors of piece to neighbors collection
        neighbors |= NEIGHBORS[ctz128(piece)];
    }

    // remove board from neighbors before returning it
    return neighbors ^ board;
}


int find_clusters(board_col_t board, board_col_t clusters[MAX_VERTICES]) {
    board_col_t cluster;
    board_col_t neighbors;
    int num_clusters = 0;

    memset(clusters, 0, sizeof clusters);

    while(board) {
        //initialize cluster with first stone
        cluster = board & (-board);
        //remove stones in cluster from board
        board ^= cluster;

        // expand cluster with it's neighbors
        while (1) {
            // TODO would it be more efficient to search with only new neighbors?
            neighbors = find_neighbors(cluster);

            if (board & neighbors) {
                // add neighbors that are also on the board
                cluster |= (board & neighbors);
                // and remove them from the board
                board ^= (board & neighbors);
            } else {
                break;
            }
        }

        clusters[num_clusters++] = cluster;
    }

    return num_clusters;
}

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

int min(int a, int b){
    if (a < b) {
        return a;
    }
    return b;
}

void find_articulation_points_recursive(board_col_t graph, board_col_t un, int ui, int * time, board_col_t * visited, int disc[MAX_VERTICES],
        int low[MAX_VERTICES], int parent[MAX_VERTICES], board_col_t * articulation_points) {

    int children = 0;

    // mark node visited
    (*visited) |= un;

    disc[ui] = low[ui] = ++(*time);

    board_col_t neighbors = NEIGHBORS[un];
    board_col_t nn;
    int ni;

    while(neighbors) {
        nn = neighbors & (-neighbors);
        neighbors ^= nn;

        ni = ctz128(nn);

        if(! (nn & graph)) continue;

        if(nn & ~(*visited)) {
            children++;
            parent[ni] = ui;
            find_articulation_points_recursive(graph, nn, ni, time, visited, disc, low, parent, articulation_points);

            low[ui] = min(low[ui], low[ni]);

            if (parent[ui] < 0 && children > 1) {
                (*articulation_points) |= un;
            }

            if (parent[ui] >= 0 && low[ni] >= disc[ui]) {
                (*articulation_points) |= un;
            }

        } else if (ni != parent[ui]) {
            low[ui] = min(low[ui], disc[ni]);
        }
    }
}

board_col_t find_articulation_points(board_col_t graph) {
    // max vertices = 30

    int parent[MAX_VERTICES];
    int disc[MAX_VERTICES];
    int low[MAX_VERTICES];

    board_col_t visited = 0;
    board_col_t articulation_points = 0;

    int i;
    for (i = 0; i < MAX_VERTICES; ++i) {
        parent[i] = -1;
    }

    board_col_t un;
    int ui;

    int time = 0;

    board_col_t wgraph = graph;

    while(wgraph) {
        un = wgraph & (-wgraph);
        wgraph ^= un;
        if(un & ~visited) {
            ui = ctz128(un);
            find_articulation_points_recursive(graph, un, ui, &time, &visited, disc, low, parent, &articulation_points);
        }
    }

    return articulation_points;
}

void print_board(board_col_t white, board_col_t black) {
    int row, col;
    board_col_t cur_row_black;
    board_col_t cur_row_white;
    char chars[BOARD_SIZE + 1];
    chars[BOARD_SIZE] = '\0';
    for(row = BOARD_SIZE -1; row >= 0; --row) {
        cur_row_black = black >> row * 11;
        cur_row_white = white >> row * 11;
        for(col = 0; col < BOARD_SIZE; ++col) {
            if (cur_row_white & cur_row_black & 1 << (col)) {
                chars[col] = '!';
            }
            else if (cur_row_black & 1 << (col)) {
                chars[col] = 'B';
            } else if (cur_row_white & 1 << (col)) {
                chars[col] = 'W';
            }
            else chars[col] = '.';
        }
        puts(chars);
    }
}

void do_move(board_col_t mycolor, board_col_t othercolor, board_col_t * from, board_col_t  * to) {
    board_col_t clusters[MAX_VERTICES];
    int num_clusters = find_clusters(mycolor, clusters);
    *from = ONE_128;
    *to = ONE_128;
}

board_col_t parse_move(char * in) {
    int x = in[0] - 'A';
    int y = atoi(in + 1);

    return ONE_128 << x * BOARD_SIZE + y;
}

void print_loc(board_col_t loc) {
    int idx = ctz128(loc);
    int x = idx / BOARD_SIZE;
    int y = idx % BOARD_SIZE;
    putc('A'+x,stdout);
    printf("%i", y+1);
}



void game_loop() {
    size_t nbytes = 0;
    char *line = NULL;

    board_col_t mycolor = BLACK_START;
    board_col_t othercolor = WHITE_START;

    board_col_t from, to;
    int idx;

    while(getline(&line, &nbytes, stdin))
    {
        if (strcmp(line, "Start\n") == 0) {
            mycolor = WHITE_START;
            othercolor = BLACK_START;
        } else if (strcmp(line, "Quit\n") == 0) {
            return;
        } else {
            char* fromc = strtok(line, "-");
            char* toc = strtok(NULL, "-");

            from = parse_move(fromc);
            to = parse_move(toc);

            othercolor ^= from;
            othercolor |= to;
        }

        do_move(mycolor, othercolor, &from, &to);
        print_loc(from);
        putc('-', stdout);
        print_loc(to);
        puts("");
        fflush(stdout);
    }
    free(line);
}

board_col_t parse_hex_board(char * s) {
    board_col_t board = 0;
    
}

void print_hex_board(board_col_t board) {

}

int main( int argc, const char* argv[] )
{
    init();
    game_loop();
//    board_col_t empty = 0;
//    board_col_t black = BLACK_START;
//    board_col_t white = WHITE_START;
//
//    int i = 0;
//
//    print_board(white, black);
//
//    board_col_t clusters[MAX_VERTICES];
//    int num_clusters = find_clusters(black, clusters);
//
//    for (i = 0; i < num_clusters; ++i) {
//        printf("\ncluster %i\n", i);
//        print_board(find_moves(clusters[i], black, white), clusters[i]);
//    }
//
//    printf( "\nHello World %i\n\n", num_clusters );

    return 0;
}