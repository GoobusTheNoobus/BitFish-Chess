/**
 * movegen.h
 * 
 * MoveGen interface
 * Generates moves into a MoveList
 */

#pragma once
#include "position.h"
#include "type.h"
#include "constants.h"
#include "move.h"
#include "bitboards.h"

namespace MoveGen {


    MoveList generate_moves (const Position& pos);
    void generate_pawn_moves (const Position& pos, MoveList& list);
    void generate_knight_moves (const Position& pos, MoveList& list);
    void generate_bishop_moves (const Position& pos, MoveList& list);
    void generate_rook_moves (const Position& pos, MoveList& list);
    void generate_queen_moves (const Position& pos, MoveList& list);
    void generate_king_moves (const Position& pos, MoveList& list);
}