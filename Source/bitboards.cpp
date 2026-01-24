#include "bitboards.h"
#include <array>

namespace {
    
}

namespace Bitboards {

    // Constants and stuff
    std::array<Bitboard, BOARD_SIZE> knight_table;
    std::array<Bitboard, BOARD_SIZE> king_table;
    std::array<std::array<Bitboard, BOARD_SIZE>, COLOR_NUM> pawn_table;

    // Precomputed Slider Tables
    std::array<std::array<Bitboard, BISHOP_BLOCKER_COMBINATIONS>, BOARD_SIZE> bishop_table;
    std::array<std::array<Bitboard, ROOK_BLOCKER_COMBINATIONS>, BOARD_SIZE> rook_table;
    
    static void precomputeLeapers () {
        std::array<int, 8> knight_f = {
            1, 1, 2, 2, -1, -1, -2, -2
        };

        std::array<int, 8> knight_r = {
            2, -2, 1, -1, 2, -2, 1, -1
        };

        std::array<int, 8> king_f = {
            1, 1, 1, 0, 0, -1, -1, -1
        };

        std::array<int, 8> king_r = {
            1, 0, -1, 1, -1, 1, 0, -1
        };

        

        // Knight in shining armor

    }
    


}