// ------------------------------------ MY SECOND CHESS ENGINE ---------------------------------------

#pragma once

#include <array>
#include <string>

#include "type.h"
#include "constants.h"


struct Position
{

    // Stuff
    Board board;

    GameInfo game_info;

    // Constructors, parses FEN, or else sets the starting position
    Position() {
        set_start_pos();
    }

    Position(std::string fen) {
        parse_fen(fen);
    }

    // idk i thnk this is what you do
    void set_start_pos();
    void update_occupancies();
    void set_square();
    void parse_fen(const std::string& fen);
    Bitboard get_bitboard (Piece piece) const;
    
    void clear_board();

    std::string to_string() const; // const = readonly
};

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
