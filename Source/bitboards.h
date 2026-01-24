#pragma once

#include "type.h"
#include "constants.h"
#include <array>

namespace Bitboards {

    // Precomputed Leaper Tables
    extern std::array<Bitboard, BOARD_SIZE> knight_table;
    extern std::array<Bitboard, BOARD_SIZE> king_table;
    extern std::array<std::array<Bitboard, BOARD_SIZE>, COLOR_NUM> pawn_table;

    // Precomputed Slider Tables
    extern std::array<std::array<Bitboard, BISHOP_BLOCKER_COMBINATIONS>, BOARD_SIZE> bishop_table;
    extern std::array<std::array<Bitboard, ROOK_BLOCKER_COMBINATIONS>, BOARD_SIZE> rook_table;

    Bitboard get_rook_attacks (Square square, Bitboard occupancy);
    Bitboard get_bishop_attacks (Square square, Bitboard occupancy);

    void init();
    
}

