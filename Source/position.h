// ------------------------------------ MY SECOND CHESS ENGINE ---------------------------------------

#pragma once

#include <array>
#include <string_view>

#include "type.h"
#include "constants.h"

struct Board 
{
    std::array <Bitboard, PIECE_NUM> piece_bitboards;
    std::array <Piece, BOARD_SIZE> mailbox;
    std::array <Bitboard, COLOR_NUM> color_bitboards;
    
    Bitboard occupancy;
};

struct GameInfo
{
    Color side_to_move;
    CastlingRights castling;
    Square ep_square;

    int rule_50_clock;
};

struct Position
{

    // Stuff
    Board board;

    GameInfo game_info;

    // Constructors, parses FEN, or else sets the starting position
    Position() {
        set_start_pos();
    }

    Position(std::string_view fen) {
        parse_fen(fen);
    }

    // Getting Functions
    Bitboard get_bitboard (Piece piece) const;
    Piece piece_at (Square square) const;

    // To String
    std::string to_string() const;

    // Basic Editing Functions
    void update_occupancies();
    void set_square(Square square, Piece piece);
    void clear_square (Square square);
    void clear_board();

    // More specific functions
    void set_start_pos();
    void parse_fen(const std::string_view fen);
    
    
};


