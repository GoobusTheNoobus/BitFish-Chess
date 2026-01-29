#pragma once
#include "position.h"
#include "type.h"
#include "constants.h"
#include "move.h"
#include "bitboards.h"

#include <vector>

// i want to be cool like stockfish :D
enum GenType {
    PSEUDO_LEGAL,
    CAPTURES_PL,
};

namespace MoveGen {
    std::vector<Move> generate_moves (GenType gen_type, const Position& pos);

    void generate_pawn_moves (GenType gen_type, const Position& pos, std::vector<Move>& list);
    void generate_knight_moves (GenType gen_type, const Position& pos, std::vector<Move>& list);
    void generate_bishop_moves (GenType gen_type, const Position& pos, std::vector<Move>& list);
    void generate_rook_moves (GenType gen_type, const Position& pos, std::vector<Move>& list);
    void generate_queen_moves (GenType gen_type, const Position& pos, std::vector<Move>& list);
    void generate_king_moves (GenType gen_type, const Position& pos, std::vector<Move>& list);
}