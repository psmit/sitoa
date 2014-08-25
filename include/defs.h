#pragma once

#include <cstdarg>
#include <cstdint>
#include <cstring>
#include <cstdio>

#define BOARD_SIZE 11
#define MAX_VERTICES 30
#define MAX_MOVES 500

struct board_t {
    uint64_t hi;
    uint64_t low;

    explicit operator bool() const {
        return hi || low;
    }

    board_t operator~() const {
        return {~hi, ~low};
    }

    board_t operator^(const board_t& rhs) const {
        return {hi ^ rhs.hi, low ^ rhs.low};
    }

    board_t operator|(const board_t & rhs) const {
        return {hi | rhs.hi, low | rhs.low};
    }

    board_t operator&(const board_t & rhs) const {
        return {hi & rhs.hi, low & rhs.low};
    }

    board_t& operator^=(const board_t & rhs) {
        hi ^= rhs.hi;
        low ^= rhs.low;
        
        return *this;
    }

    board_t& operator|=(const board_t & rhs) {
        hi |= rhs.hi;
        low |= rhs.low;

        return *this;
    }

    board_t& operator&=(const board_t & rhs) {
        hi &= rhs.hi;
        low &= rhs.low;

        return *this;
    }

    board_t lso() const {
        if (low) {
            return {0, low & ~(low-1)};
        } else {
            return {hi & ~(hi-1), 0};
        }
    }

    int ctz() const {
        if (low) {
            return __builtin_ctzll(low);
        } else {
            return __builtin_ctzll(hi) + sizeof(low) * 8;
        }
    }

    int popcount() const {
        return __builtin_popcountll(hi) + __builtin_popcountll(low);
    }
};

board_t B_SINGLE_BIT[BOARD_SIZE * BOARD_SIZE] = {
        {0x0000000000000000ul,0x0000000000000001ul},
        {0x0000000000000000ul,0x0000000000000002ul},
        {0x0000000000000000ul,0x0000000000000004ul},
        {0x0000000000000000ul,0x0000000000000008ul},
        {0x0000000000000000ul,0x0000000000000010ul},
        {0x0000000000000000ul,0x0000000000000020ul},
        {0x0000000000000000ul,0x0000000000000040ul},
        {0x0000000000000000ul,0x0000000000000080ul},
        {0x0000000000000000ul,0x0000000000000100ul},
        {0x0000000000000000ul,0x0000000000000200ul},
        {0x0000000000000000ul,0x0000000000000400ul},
        {0x0000000000000000ul,0x0000000000000800ul},
        {0x0000000000000000ul,0x0000000000001000ul},
        {0x0000000000000000ul,0x0000000000002000ul},
        {0x0000000000000000ul,0x0000000000004000ul},
        {0x0000000000000000ul,0x0000000000008000ul},
        {0x0000000000000000ul,0x0000000000010000ul},
        {0x0000000000000000ul,0x0000000000020000ul},
        {0x0000000000000000ul,0x0000000000040000ul},
        {0x0000000000000000ul,0x0000000000080000ul},
        {0x0000000000000000ul,0x0000000000100000ul},
        {0x0000000000000000ul,0x0000000000200000ul},
        {0x0000000000000000ul,0x0000000000400000ul},
        {0x0000000000000000ul,0x0000000000800000ul},
        {0x0000000000000000ul,0x0000000001000000ul},
        {0x0000000000000000ul,0x0000000002000000ul},
        {0x0000000000000000ul,0x0000000004000000ul},
        {0x0000000000000000ul,0x0000000008000000ul},
        {0x0000000000000000ul,0x0000000010000000ul},
        {0x0000000000000000ul,0x0000000020000000ul},
        {0x0000000000000000ul,0x0000000040000000ul},
        {0x0000000000000000ul,0x0000000080000000ul},
        {0x0000000000000000ul,0x0000000100000000ul},
        {0x0000000000000000ul,0x0000000200000000ul},
        {0x0000000000000000ul,0x0000000400000000ul},
        {0x0000000000000000ul,0x0000000800000000ul},
        {0x0000000000000000ul,0x0000001000000000ul},
        {0x0000000000000000ul,0x0000002000000000ul},
        {0x0000000000000000ul,0x0000004000000000ul},
        {0x0000000000000000ul,0x0000008000000000ul},
        {0x0000000000000000ul,0x0000010000000000ul},
        {0x0000000000000000ul,0x0000020000000000ul},
        {0x0000000000000000ul,0x0000040000000000ul},
        {0x0000000000000000ul,0x0000080000000000ul},
        {0x0000000000000000ul,0x0000100000000000ul},
        {0x0000000000000000ul,0x0000200000000000ul},
        {0x0000000000000000ul,0x0000400000000000ul},
        {0x0000000000000000ul,0x0000800000000000ul},
        {0x0000000000000000ul,0x0001000000000000ul},
        {0x0000000000000000ul,0x0002000000000000ul},
        {0x0000000000000000ul,0x0004000000000000ul},
        {0x0000000000000000ul,0x0008000000000000ul},
        {0x0000000000000000ul,0x0010000000000000ul},
        {0x0000000000000000ul,0x0020000000000000ul},
        {0x0000000000000000ul,0x0040000000000000ul},
        {0x0000000000000000ul,0x0080000000000000ul},
        {0x0000000000000000ul,0x0100000000000000ul},
        {0x0000000000000000ul,0x0200000000000000ul},
        {0x0000000000000000ul,0x0400000000000000ul},
        {0x0000000000000000ul,0x0800000000000000ul},
        {0x0000000000000000ul,0x1000000000000000ul},
        {0x0000000000000000ul,0x2000000000000000ul},
        {0x0000000000000000ul,0x4000000000000000ul},
        {0x0000000000000000ul,0x8000000000000000ul},
        {0x0000000000000001ul,0x0000000000000000ul},
        {0x0000000000000002ul,0x0000000000000000ul},
        {0x0000000000000004ul,0x0000000000000000ul},
        {0x0000000000000008ul,0x0000000000000000ul},
        {0x0000000000000010ul,0x0000000000000000ul},
        {0x0000000000000020ul,0x0000000000000000ul},
        {0x0000000000000040ul,0x0000000000000000ul},
        {0x0000000000000080ul,0x0000000000000000ul},
        {0x0000000000000100ul,0x0000000000000000ul},
        {0x0000000000000200ul,0x0000000000000000ul},
        {0x0000000000000400ul,0x0000000000000000ul},
        {0x0000000000000800ul,0x0000000000000000ul},
        {0x0000000000001000ul,0x0000000000000000ul},
        {0x0000000000002000ul,0x0000000000000000ul},
        {0x0000000000004000ul,0x0000000000000000ul},
        {0x0000000000008000ul,0x0000000000000000ul},
        {0x0000000000010000ul,0x0000000000000000ul},
        {0x0000000000020000ul,0x0000000000000000ul},
        {0x0000000000040000ul,0x0000000000000000ul},
        {0x0000000000080000ul,0x0000000000000000ul},
        {0x0000000000100000ul,0x0000000000000000ul},
        {0x0000000000200000ul,0x0000000000000000ul},
        {0x0000000000400000ul,0x0000000000000000ul},
        {0x0000000000800000ul,0x0000000000000000ul},
        {0x0000000001000000ul,0x0000000000000000ul},
        {0x0000000002000000ul,0x0000000000000000ul},
        {0x0000000004000000ul,0x0000000000000000ul},
        {0x0000000008000000ul,0x0000000000000000ul},
        {0x0000000010000000ul,0x0000000000000000ul},
        {0x0000000020000000ul,0x0000000000000000ul},
        {0x0000000040000000ul,0x0000000000000000ul},
        {0x0000000080000000ul,0x0000000000000000ul},
        {0x0000000100000000ul,0x0000000000000000ul},
        {0x0000000200000000ul,0x0000000000000000ul},
        {0x0000000400000000ul,0x0000000000000000ul},
        {0x0000000800000000ul,0x0000000000000000ul},
        {0x0000001000000000ul,0x0000000000000000ul},
        {0x0000002000000000ul,0x0000000000000000ul},
        {0x0000004000000000ul,0x0000000000000000ul},
        {0x0000008000000000ul,0x0000000000000000ul},
        {0x0000010000000000ul,0x0000000000000000ul},
        {0x0000020000000000ul,0x0000000000000000ul},
        {0x0000040000000000ul,0x0000000000000000ul},
        {0x0000080000000000ul,0x0000000000000000ul},
        {0x0000100000000000ul,0x0000000000000000ul},
        {0x0000200000000000ul,0x0000000000000000ul},
        {0x0000400000000000ul,0x0000000000000000ul},
        {0x0000800000000000ul,0x0000000000000000ul},
        {0x0001000000000000ul,0x0000000000000000ul},
        {0x0002000000000000ul,0x0000000000000000ul},
        {0x0004000000000000ul,0x0000000000000000ul},
        {0x0008000000000000ul,0x0000000000000000ul},
        {0x0010000000000000ul,0x0000000000000000ul},
        {0x0020000000000000ul,0x0000000000000000ul},
        {0x0040000000000000ul,0x0000000000000000ul},
        {0x0080000000000000ul,0x0000000000000000ul},
        {0x0100000000000000ul,0x0000000000000000ul}
};
board_t B_NEIGHBOURS[BOARD_SIZE * BOARD_SIZE] = {
        {0x0000000000000000ul, 0x0000000000000802ul},
        {0x0000000000000000ul, 0x0000000000001005ul},
        {0x0000000000000000ul, 0x000000000000200aul},
        {0x0000000000000000ul, 0x0000000000004014ul},
        {0x0000000000000000ul, 0x0000000000008028ul},
        {0x0000000000000000ul, 0x0000000000010050ul},
        {0x0000000000000000ul, 0x00000000000200a0ul},
        {0x0000000000000000ul, 0x0000000000040140ul},
        {0x0000000000000000ul, 0x0000000000080280ul},
        {0x0000000000000000ul, 0x0000000000100500ul},
        {0x0000000000000000ul, 0x0000000000200200ul},
        {0x0000000000000000ul, 0x0000000000401001ul},
        {0x0000000000000000ul, 0x0000000000802802ul},
        {0x0000000000000000ul, 0x0000000001005004ul},
        {0x0000000000000000ul, 0x000000000200a008ul},
        {0x0000000000000000ul, 0x0000000004014010ul},
        {0x0000000000000000ul, 0x0000000008028020ul},
        {0x0000000000000000ul, 0x0000000010050040ul},
        {0x0000000000000000ul, 0x00000000200a0080ul},
        {0x0000000000000000ul, 0x0000000040140100ul},
        {0x0000000000000000ul, 0x0000000080280200ul},
        {0x0000000000000000ul, 0x0000000100100400ul},
        {0x0000000000000000ul, 0x0000000200800800ul},
        {0x0000000000000000ul, 0x0000000401401000ul},
        {0x0000000000000000ul, 0x0000000802802000ul},
        {0x0000000000000000ul, 0x0000001005004000ul},
        {0x0000000000000000ul, 0x000000200a008000ul},
        {0x0000000000000000ul, 0x0000004014010000ul},
        {0x0000000000000000ul, 0x0000008028020000ul},
        {0x0000000000000000ul, 0x0000010050040000ul},
        {0x0000000000000000ul, 0x00000200a0080000ul},
        {0x0000000000000000ul, 0x0000040140100000ul},
        {0x0000000000000000ul, 0x0000080080200000ul},
        {0x0000000000000000ul, 0x0000100400400000ul},
        {0x0000000000000000ul, 0x0000200a00800000ul},
        {0x0000000000000000ul, 0x0000401401000000ul},
        {0x0000000000000000ul, 0x0000802802000000ul},
        {0x0000000000000000ul, 0x0001005004000000ul},
        {0x0000000000000000ul, 0x000200a008000000ul},
        {0x0000000000000000ul, 0x0004014010000000ul},
        {0x0000000000000000ul, 0x0008028020000000ul},
        {0x0000000000000000ul, 0x0010050040000000ul},
        {0x0000000000000000ul, 0x00200a0080000000ul},
        {0x0000000000000000ul, 0x0040040100000000ul},
        {0x0000000000000000ul, 0x0080200200000000ul},
        {0x0000000000000000ul, 0x0100500400000000ul},
        {0x0000000000000000ul, 0x0200a00800000000ul},
        {0x0000000000000000ul, 0x0401401000000000ul},
        {0x0000000000000000ul, 0x0802802000000000ul},
        {0x0000000000000000ul, 0x1005004000000000ul},
        {0x0000000000000000ul, 0x200a008000000000ul},
        {0x0000000000000000ul, 0x4014010000000000ul},
        {0x0000000000000000ul, 0x8028020000000000ul},
        {0x0000000000000001ul, 0x0050040000000000ul},
        {0x0000000000000002ul, 0x0020080000000000ul},
        {0x0000000000000004ul, 0x0100100000000000ul},
        {0x0000000000000008ul, 0x0280200000000000ul},
        {0x0000000000000010ul, 0x0500400000000000ul},
        {0x0000000000000020ul, 0x0a00800000000000ul},
        {0x0000000000000040ul, 0x1401000000000000ul},
        {0x0000000000000080ul, 0x2802000000000000ul},
        {0x0000000000000100ul, 0x5004000000000000ul},
        {0x0000000000000200ul, 0xa008000000000000ul},
        {0x0000000000000401ul, 0x4010000000000000ul},
        {0x0000000000000802ul, 0x8020000000000000ul},
        {0x0000000000001001ul, 0x0040000000000000ul},
        {0x0000000000002008ul, 0x0080000000000000ul},
        {0x0000000000004014ul, 0x0100000000000000ul},
        {0x0000000000008028ul, 0x0200000000000000ul},
        {0x0000000000010050ul, 0x0400000000000000ul},
        {0x00000000000200a0ul, 0x0800000000000000ul},
        {0x0000000000040140ul, 0x1000000000000000ul},
        {0x0000000000080280ul, 0x2000000000000000ul},
        {0x0000000000100500ul, 0x4000000000000000ul},
        {0x0000000000200a00ul, 0x8000000000000000ul},
        {0x0000000000401401ul, 0x0000000000000000ul},
        {0x0000000000800802ul, 0x0000000000000000ul},
        {0x0000000001004004ul, 0x0000000000000000ul},
        {0x000000000200a008ul, 0x0000000000000000ul},
        {0x0000000004014010ul, 0x0000000000000000ul},
        {0x0000000008028020ul, 0x0000000000000000ul},
        {0x0000000010050040ul, 0x0000000000000000ul},
        {0x00000000200a0080ul, 0x0000000000000000ul},
        {0x0000000040140100ul, 0x0000000000000000ul},
        {0x0000000080280200ul, 0x0000000000000000ul},
        {0x0000000100500400ul, 0x0000000000000000ul},
        {0x0000000200a00800ul, 0x0000000000000000ul},
        {0x0000000400401000ul, 0x0000000000000000ul},
        {0x0000000802002000ul, 0x0000000000000000ul},
        {0x0000001005004000ul, 0x0000000000000000ul},
        {0x000000200a008000ul, 0x0000000000000000ul},
        {0x0000004014010000ul, 0x0000000000000000ul},
        {0x0000008028020000ul, 0x0000000000000000ul},
        {0x0000010050040000ul, 0x0000000000000000ul},
        {0x00000200a0080000ul, 0x0000000000000000ul},
        {0x0000040140100000ul, 0x0000000000000000ul},
        {0x0000080280200000ul, 0x0000000000000000ul},
        {0x0000100500400000ul, 0x0000000000000000ul},
        {0x0000200200800000ul, 0x0000000000000000ul},
        {0x0000401001000000ul, 0x0000000000000000ul},
        {0x0000802802000000ul, 0x0000000000000000ul},
        {0x0001005004000000ul, 0x0000000000000000ul},
        {0x000200a008000000ul, 0x0000000000000000ul},
        {0x0004014010000000ul, 0x0000000000000000ul},
        {0x0008028020000000ul, 0x0000000000000000ul},
        {0x0010050040000000ul, 0x0000000000000000ul},
        {0x00200a0080000000ul, 0x0000000000000000ul},
        {0x0040140100000000ul, 0x0000000000000000ul},
        {0x0080280200000000ul, 0x0000000000000000ul},
        {0x0100100400000000ul, 0x0000000000000000ul},
        {0x0000800800000000ul, 0x0000000000000000ul},
        {0x0001401000000000ul, 0x0000000000000000ul},
        {0x0002802000000000ul, 0x0000000000000000ul},
        {0x0005004000000000ul, 0x0000000000000000ul},
        {0x000a008000000000ul, 0x0000000000000000ul},
        {0x0014010000000000ul, 0x0000000000000000ul},
        {0x0028020000000000ul, 0x0000000000000000ul},
        {0x0050040000000000ul, 0x0000000000000000ul},
        {0x00a0080000000000ul, 0x0000000000000000ul},
        {0x0140100000000000ul, 0x0000000000000000ul},
        {0x0080200000000000ul, 0x0000000000000000ul}};

board_t B_EMPTY = {0,0};
board_t B_BLACK_START = {0x00002aa800aaa002ul, 0xaa800aaa002aa800ul};
board_t B_WHITE_START  = {0x00aa8002aa000aa8ul, 0x002aa000aa8002aaul};

board_t B_CIRCLES[BOARD_SIZE] = {
        {0x01ffe00c01803006ul, 0x00c0180300600ffful},
        {0x00001ff202404809ul, 0x01202404809ff000ul},
        {0x00000001fc208410ul, 0x821042087f000000ul},
        {0x00000000001f0220ul, 0x440881f000000000ul},
        {0x00000000000001c0ul, 0x2807000000000000ul},
        {0x0000000000000000ul, 0x1000000000000000ul}
};