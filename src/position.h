// ------------------------------------ MY SECOND CHESS ENGINE ---------------------------------------

#pragma once

#include <array>
#include <string_view>

#include "type.h"
#include "constants.h"
#include "move.h"

struct Board {
    std::array <Bitboard, PIECE_NUM> piece_bitboards;
    std::array <Piece, BOARD_SIZE> mailbox;
    std::array <Bitboard, COLOR_NUM> color_bitboards;
    
    Bitboard occupancy;
};

struct GameInfo {
    Color side_to_move;
    CastlingRights castling;
    Square ep_square;

    uint8_t rule_50_clock;
};

struct UndoStack {
    std::array<PackedGI, 256> list;
    int size = 0;

    inline void push_back (PackedGI info) {
        list[size++] = info;
    }

    inline Move pop () {
        size --;
        return list[size];
    }

    inline Move peek () {
        return list[size - 1];
    }

    inline void clear () {
        size = 0;
    }

    inline Move operator[](int index) const {
        return list[index];
    }
    
    inline Move& operator[](int index) {
        return list[index];
    }

    

};

struct Position {

    // board and gameinfo
    Board board;
    GameInfo game_info;

    // Move History
    MoveList move_stack;
    UndoStack undo_stack;

    // constructors, parses FEN, or else sets the starting position
    Position() {
        set_start_pos();
    }

    Position(std::string_view fen) {
        parse_fen(fen);
    }

    // lookups
    Bitboard get_bitboard (Piece piece) const;
    Piece piece_at (Square square) const;

    // printing
    std::string to_string() const;

    // editing functions
    void update_occupancies();
    void set_square(Square square, Piece piece);
    void clear_square (Square square);
    void clear_board();

    void set_start_pos();
    void parse_fen(const std::string_view fen);
    
    bool is_square_attacked (Square square, Color color) const;
    bool is_in_check (Color color) const;
    bool can_cap_king () const;
    bool can_castle_ks () const;
    bool can_castle_qs () const;

    

    void make_move (Move move);
    void undo_move ();
};


