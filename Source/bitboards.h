#pragma once

#include "type.h"
#include "constants.h"
#include <array>
#include <string>

namespace Leapers {
    // precompute every square's attacks, simple cuz these pieces cannot be blocked
    void pc_knight_attacks (Square square);
    void pc_king_attacks (Square square);
    void pc_white_pawn_attacks (Square square);
    void pc_black_pawn_attacks (Square square);
}

namespace Sliders {
    // mask & square-relevancy generation
    void generate_bishop_mask (Square square);
    void generate_rook_mask (Square square);

    // hash function for quick table lookups
    int hash_bishop (Square square, Bitboard occupancy);
    int hash_rook (Square square, Bitboard occupancy);

    // slow raycasting at compile tiem
    Bitboard raycast_bishop(Square square, Bitboard occupancy);
    Bitboard raycast_rook (Square square, Bitboard occupancy);

    Bitboard index_to_blocker(int index, Bitboard mask);

    // generate every possible blocker configuration for that given square
    void pc_bishop_attacks (Square square);
    void pc_rook_attacks (Square square);

    // slider masks
    extern std::array<Bitboard, BOARD_SIZE> bishop_masks;
    extern std::array<Bitboard, BOARD_SIZE> rook_masks;

    // bitcount of masks for iterating through blocker configs & hashing 
    extern std::array<int, BOARD_SIZE> bishop_relevancy;
    extern std::array<int, BOARD_SIZE> rook_relevancy;
}


namespace Bitboards {

    // precomputed leaper tables
    extern std::array<Bitboard, BOARD_SIZE> knight_table;
    extern std::array<Bitboard, BOARD_SIZE> king_table;
    extern std::array<std::array<Bitboard, BOARD_SIZE>, COLOR_NUM> pawn_table;

    // slider tables
    extern std::array<std::array<Bitboard, BISHOP_BLOCKER_COMBINATIONS>, BOARD_SIZE> bishop_table;
    extern std::array<std::array<Bitboard, ROOK_BLOCKER_COMBINATIONS>, BOARD_SIZE> rook_table;

    // lookups
    Bitboard get_rook_attacks (Square square, Bitboard occupancy);
    Bitboard get_bishop_attacks (Square square, Bitboard occupancy);


    inline Bitboard get_knight_attacks (Square square) {
        return knight_table[square];
    };

    inline Bitboard get_king_attacks (Square square) {
        return king_table[square];
    };

    inline Bitboard get_pawn_attacks (Square square, Color color) {
        return pawn_table[color][square];
    };

    // initialize
    void init();

    // utility
    void pretty_print (Bitboard bitboard);
    
}

