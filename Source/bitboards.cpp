#include "bitboards.h"
#include "type.h"
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
        std::array<std::array<int, 2>, 8> knight_vectors = {{
            {1, 2},
            {1, -2},
            {-1, 2},
            {-1, -2},
            {2, 1},
            {2, -1},
            {-2, 1},
            {-2, -1}
        }};

        std::array<std::array<int, 2>, 8> king_vectors = {{
            {1, 0},
            {1, 1},
            {1, -1},
            {0, 1},
            {0, -1},
            {-1, 1},
            {-1, 0},
            {-1, -1}
        }};

        std::array<std::array<int, 2>, 8> white_pawn_vectors = {{
            {1, 1},
            {1, -1}
        }};

        std::array<std::array<int, 2>, 8> black_pawn_vectors = {{
            {-1, 1},
            {-1, -1}
        }};

        for (int square = 0; square < 64; square++) {
            // Initialize
            int r;
            int f;

            Bitboard mask = 0ULL;

            // Knight
            for (std::array<int, 2> knight_vector: knight_vectors) {
                r = square >> 3 + knight_vector[0];
                f = square & 7 + knight_vector[1];

                // Bound Check
                if (r < 0 || r > 7 || f < 0 || f > 7) {
                    continue;
                }

                mask |= 1ULL << parse_square(r, f);

            }

            knight_table[square] = mask;

            mask = 0ULL;
        }

    }
    


}
