/**
 * bitboard.h
 * 
 * Main Bitboard Interface 
 * Contains rank & file bitboards
 * Lookup functions for O(1) MoveGen
 */

#pragma once

#include "type.h"
#include "constants.h"
#include <array>
#include <string>





namespace Bitboards {

    constexpr Bitboard rank1 = 0xFF;
    constexpr Bitboard rank2 = rank1 << 8;
    constexpr Bitboard rank3 = rank2 << 8;
    constexpr Bitboard rank4 = rank3 << 8;
    constexpr Bitboard rank5 = rank4 << 8;
    constexpr Bitboard rank6 = rank5 << 8;
    constexpr Bitboard rank7 = rank6 << 8;
    constexpr Bitboard rank8 = rank7 << 8;

    constexpr Bitboard file_a = 0x0101010101010101ULL;
    constexpr Bitboard file_h = 0x8080808080808080ULL;
    

    extern std::array<Bitboard, BOARD_SIZE> square_bb;

    // lookups
    Bitboard get_rook_attacks (Square square, Bitboard occupancy);
    Bitboard get_bishop_attacks (Square square, Bitboard occupancy);


    Bitboard get_knight_attacks (Square square);
    Bitboard get_king_attacks (Square square);
    Bitboard get_pawn_attacks (Square square, Color color);

    // initialize
    void init();

    // utility
    std::string to_string (Bitboard bitboard);
    
}


