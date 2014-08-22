#include <cstdint>

struct board_t {
    uint64_t hi;
    uint64_t low;

//    board_t (uint64_t hi, uint64_t low) : hi(hi), low(low) { }


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

    board_t& operator|=(const board_t & rhs) {
        hi |= rhs.hi;
        low |= rhs.low;

        return *this;
    }


};




int main( int argc, const char* argv[] ) {
    board_t board = {0x00aa8002aa000aa8 ,0x002aa000aa8002aa};
    board_t board2 = board;

    return 0;

}