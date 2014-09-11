#include <cstdint>
#include <cstdio>

typedef uint64_t hash_t;
struct board_t {
    uint64_t hi;
    uint64_t low;
};

struct trans_node {
    hash_t hash;
    int16_t value;
    uint16_t flag : 3;
    uint16_t depth : 5;
    uint16_t round : 5;
    uint16_t : 3;

    trans_node *next;
//
//    board_t black;
//    board_t white;
};


int main(int argc, const char *argv[]) {
    trans_node *t;
    fprintf(stderr, "Size of board %d\n", sizeof(board_t));
    fprintf(stderr, "Size of trans_node %d\n", sizeof(trans_node));
    fprintf(stderr, "Size of *trans_node %d\n", sizeof(t));

    fflush(stderr);
    return 0;

}