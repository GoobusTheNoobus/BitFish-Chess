#include "bitboards.h"
#include <array>


namespace Bitboards {

    // Constants and stuff
    
    extern std::array<Bitboard, BOARD_SIZE> square_masks;

    void init() {
        for (int i = 0; i < BOARD_SIZE; i++) {
            square_masks[i] = 1ULL << i;
        }
    }

    


}