#include "bitboards.h"
#include "type.h"
#include "magic.h"
#include <array>
#include <iostream>
#include <vector>

namespace Bitboards {

    std::array<Bitboard, BOARD_SIZE> square_bb;

    // precomputed attack tables at runtime
    std::array<Bitboard, BOARD_SIZE> knight_table;
    std::array<Bitboard, BOARD_SIZE> king_table;
    std::array<std::array<Bitboard, BOARD_SIZE>, COLOR_NUM> pawn_table;

    // magic tables
    std::array<std::array<Bitboard, BISHOP_BLOCKER_COMBINATIONS>, BOARD_SIZE> bishop_table;
    std::array<std::array<Bitboard, ROOK_BLOCKER_COMBINATIONS>, BOARD_SIZE> rook_table;
    
    // lookup functions
    Bitboard get_bishop_attacks (Square square, Bitboard occupancy) {
        
        return bishop_table[square][Sliders::hash_bishop(square, occupancy)];
    }

    Bitboard get_rook_attacks (Square square, Bitboard occupancy) {
        return rook_table[square][Sliders::hash_rook(square, occupancy)];
    }

    // pretty a bitboard
    std::string to_string (Bitboard bitboard) {
        std::ostringstream string;

        string << "\n  +-----------------+\n";
        for (int rank = 7; rank >= 0; rank--) {
            string << rank + 1 << " | ";
            for (int file = 0; file < 8; file++) {
                int square = rank * 8 + file;
                string << (((bitboard >> square) & 1ULL) ? "O " : ". ");
            }
            string << "|\n";
        }
        string << "  +-----------------+\n";
        string << "    a b c d e f g h\n\n";

        return string.str();
    }

    // initialize
    void init () {
        for (int square = 0; square < BOARD_SIZE; square++) {
            Square sqEnum = Square(square);

            Leapers::pc_knight_attacks(sqEnum);
            Leapers::pc_king_attacks(sqEnum);
            Leapers::pc_white_pawn_attacks(sqEnum);
            Leapers::pc_black_pawn_attacks(sqEnum);

            Sliders::generate_bishop_mask(sqEnum);
            Sliders::generate_rook_mask(sqEnum);

            Sliders::pc_bishop_attacks(sqEnum);
            Sliders::pc_rook_attacks(sqEnum);

            square_bb[square] = 1ULL << square;
            
        }

        

        
    }

    


    


}

namespace Leapers{
    constexpr std::array<std::array<int, 2>, 8> knight_vectors = {{
        {1, 2},
        {1, -2},
        {-1, 2},
        {-1, -2},
        {2, 1},
        {2, -1},
        {-2, 1},
        {-2, -1}
    }};

    constexpr std::array<std::array<int, 2>, 8> king_vectors = {{
        {1, 0},
        {1, 1},
        {1, -1},
        {0, 1},
        {0, -1},
        {-1, 1},
        {-1, 0},
        {-1, -1}
    }};

    constexpr std::array<std::array<int, 2>, 2> white_pawn_vectors = {{
        {1, 1},
        {1, -1}
    }};

    constexpr std::array<std::array<int, 2>, 2> black_pawn_vectors = {{
        {-1, 1},
        {-1, -1}
    }};

    void pc_knight_attacks (Square square) {
        int r;
        int f;

        Bitboard mask = 0ULL;

        
        for (const auto& vector: knight_vectors) {
            r = (square >> 3) + vector[0];
            f = (square & 7) + vector[1];

            // Bound Check
            if (r < 0 || r > 7 || f < 0 || f > 7) {
                   continue;
            }

            mask |= 1ULL << (r << 3 | f);

        }

        Bitboards::knight_table[square] = mask;
    }

    void pc_king_attacks (Square square) {
        int r;
        int f;

        Bitboard mask = 0ULL;
            
        for (const auto& vector: king_vectors) {
            r = (square >> 3) + vector[0];
            f = (square & 7) + vector[1];

            // Bound Check
            if (r < 0 || r > 7 || f < 0 || f > 7) {
                continue;
            }

            mask |= 1ULL << (r << 3 | f);

        }

            

        Bitboards::king_table[square] = mask;
    }

    void pc_white_pawn_attacks (Square square) {
        Bitboard mask = 0ULL;
        
        int r;
        int f;
        // White Pawn
            
            
        for (const auto& vector: white_pawn_vectors) {
                
            r = (square >> 3) + vector[0];
            f = (square & 7) + vector[1];

            // Bound Check
            if (r < 0 || r > 7 || f < 0 || f > 7) {
                continue;
            }

            mask |= 1ULL << (r << 3 | f);

        }

            

        Bitboards::pawn_table[WHITE][square] = mask;

        
    }

    void pc_black_pawn_attacks (Square square) {
        Bitboard mask = 0ULL;

        int r;
        int f;

        for (const auto& vector: black_pawn_vectors) {
            r = (square >> 3) + vector[0];
            f = (square & 7) + vector[1];

            // Bound Check
            if (r < 0 || r > 7 || f < 0 || f > 7) {
                continue;
            }

            mask |= 1ULL << (r << 3 | f);

        }

        Bitboards::pawn_table[BLACK][square] = mask;
    }
}

namespace Sliders {
    std::array<Bitboard, BOARD_SIZE> rook_masks;
    std::array<Bitboard, BOARD_SIZE> bishop_masks;

    std::array<int, BOARD_SIZE> rook_relevancy;
    std::array<int, BOARD_SIZE> bishop_relevancy;

    void generate_bishop_mask (Square square) {
        int rank = square >> 3;
        int file = square & 7;

        Bitboard mask = 0ULL;

        for (int newRank = rank + 1, newFile = file + 1; newRank <= 6 && newFile <= 6; ++newRank, ++newFile) {
            mask |= 1ULL << (newRank << 3 | newFile);
        }

        for (int newRank = rank + 1, newFile = file - 1; newRank <= 6 && newFile >= 1; ++newRank, --newFile) {
            mask |= 1ULL << (newRank << 3 | newFile);
        }

        for (int newRank = rank - 1, newFile = file + 1; newRank >= 1 && newFile <= 6; --newRank, ++newFile) {
            mask |= 1ULL << (newRank << 3 | newFile);
        }

        for (int newRank = rank - 1, newFile = file - 1; newRank >= 1 && newFile >= 1; --newRank, --newFile) {
            mask |= 1ULL << (newRank << 3 | newFile);
        }

        Sliders::bishop_masks[square] = mask;
        Sliders::bishop_relevancy[square] = __builtin_popcountll(mask);
    }

    void generate_rook_mask (Square square) {
        int rank = square >> 3;
        int file = square & 7;

        Bitboard mask = 0ULL;

        // North
        for (int newRank = rank + 1; newRank <= 6; ++newRank) {
            mask |= 1ULL << (newRank << 3 | file);
        }

        // South
        for (int newRank = rank - 1; newRank >= 1; --newRank) {
            mask |= 1ULL << (newRank << 3 | file);
        }

        // East 
        for (int newFile = file + 1; newFile <= 6; ++newFile) {
            mask |= 1ULL << (rank << 3 | newFile);
        }

        // West
        for (int newFile = file - 1; newFile >= 1; --newFile) {
            mask |= 1ULL << (rank << 3 | newFile);
        }

        Sliders::rook_masks[square] = mask;

        Sliders::rook_relevancy[square] = __builtin_popcountll(mask);
    }

    int hash_bishop (Square square, Bitboard blockers) {
        return ((blockers & Sliders::bishop_masks[square]) * bishop_magic[square]) >> (64 - Sliders::bishop_relevancy[square]);
    }

    

    int hash_rook (Square square, Bitboard blockers) {
        return ((blockers & Sliders::rook_masks[square]) * rook_magic[square]) >> (64 - Sliders::rook_relevancy[square]);
    }
    

    Bitboard raycast_bishop (Square square, Bitboard blockers) {

        Bitboard mask = 0ULL;

        int rank = square >> 3;
        int file = square & 7;

        

        for (int newRank = rank + 1, newFile = file + 1; newRank <= 7 && newFile <= 7; ++newRank, ++newFile) {

            Bitboard newSquareMask = 1ULL << (newRank << 3 | newFile);

            if ((newSquareMask & blockers) != 0ULL) {
                mask |= newSquareMask;
                break;
            }
            mask |= newSquareMask;
        }

        for (int newRank = rank + 1, newFile = file - 1; newRank <= 7 && newFile >= 0; ++newRank, --newFile) {
            
            Bitboard newSquareMask = 1ULL << (newRank << 3 | newFile);

            if ((newSquareMask & blockers) != 0ULL) {
                mask |= newSquareMask;
                break;
            }
            mask |= newSquareMask;
        }

        for (int newRank = rank - 1, newFile = file + 1; newRank >= 0 && newFile <= 7; --newRank, ++newFile) {
            
            Bitboard newSquareMask = 1ULL << (newRank << 3 | newFile);

            if ((newSquareMask & blockers) != 0ULL) {
                mask |= newSquareMask;
                break;
            }
            mask |= newSquareMask;
        }

        for (int newRank = rank - 1, newFile = file - 1; newRank >= 0 && newFile >= 0; --newRank, --newFile) {
            
            Bitboard newSquareMask = 1ULL << (newRank << 3 | newFile);

            if ((newSquareMask & blockers) != 0ULL) {
                mask |= newSquareMask;
                break;
            }
            mask |= newSquareMask;
        }

        return mask;
        
    }

    Bitboard raycast_rook (Square square, Bitboard blockers) {
        int rank = square >> 3;
        int file = square & 7;

        Bitboard mask = 0ULL;

        // North
        for (int newRank = rank + 1; newRank <= 7; ++newRank) {
            Bitboard newSquareMask = 1ULL << (newRank << 3 | file);

            if ((newSquareMask & blockers) != 0ULL) {
                mask |= newSquareMask;
                break;
            }
            mask |= newSquareMask;
        }

        // South
        for (int newRank = rank - 1; newRank >= 0; --newRank) {
            Bitboard newSquareMask = 1ULL << (newRank << 3 | file);

            if ((newSquareMask & blockers) != 0ULL) {
                mask |= newSquareMask;
                break;
            }
            mask |= newSquareMask;
        }

        // East 
        for (int newFile = file + 1; newFile <= 7; ++newFile) {
            Bitboard newSquareMask = 1ULL << (rank << 3 | newFile);

            if ((newSquareMask & blockers) != 0ULL) {
                mask |= newSquareMask;
                break;
            }
            mask |= newSquareMask;
        }

        // West
        for (int newFile = file - 1; newFile >= 0; --newFile) {
            Bitboard newSquareMask = 1ULL << (rank << 3 | newFile);

            if ((newSquareMask & blockers) != 0ULL) {
                mask |= newSquareMask;
                break;
            }
            mask |= newSquareMask;
        }

        return mask;
    }

    Bitboard index_to_blocker(int index, Bitboard mask) {
        Bitboard blockers = 0ULL;

        int bit_num = 0;

        while (mask) {
            int square = __builtin_ctzll (mask);

            if (index & (1 << bit_num)) {
                blockers |= 1ULL << square;
            }

            mask &= mask - 1;
            bit_num++;
        }

        return blockers;
    }

    

    

    

    void pc_bishop_attacks (Square square) {
        for (int i = 0; i < (1 << bishop_relevancy[square]); i++) {
            Bitboard blockers = index_to_blocker(i, bishop_masks[square]);

            Bitboard attacks = raycast_bishop(square, blockers);

            Bitboards::bishop_table[square][hash_bishop(square, blockers)] = attacks;
        }
    }

    

    void pc_rook_attacks (Square square) {
        for (int i = 0; i < (1 << rook_relevancy[square]); i++) {
            Bitboard blockers = index_to_blocker(i, rook_masks[square]);

            Bitboard attacks = raycast_rook(square, blockers);

            Bitboards::rook_table[square][hash_rook(square, blockers)] = attacks;
        }
    }


    

    

    void pc_bishop_attacks (Square square) {
        for (int i = 0; i < (1 << bishop_relevancy[square]); i++) {
            Bitboard blockers = index_to_blocker(i, bishop_masks[square]);

            Bitboard attacks = raycast_bishop(square, blockers);

            Bitboards::bishop_table[square][hash_bishop(square, blockers)] = attacks;
        }
    }

    

    void pc_rook_attacks (Square square) {
        for (int i = 0; i < (1 << rook_relevancy[square]); i++) {
            Bitboard blockers = index_to_blocker(i, rook_masks[square]);

            Bitboard attacks = raycast_rook(square, blockers);

            Bitboards::rook_table[square][hash_rook(square, blockers)] = attacks;
        }
    }


    


}


}

