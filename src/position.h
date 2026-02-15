/**
 * position.h
 * 
 * Position representation interface as well as other related stuff
 */

#pragma once

#include <array>
#include <string_view>

#include "type.h"
#include "constants.h"
#include "move.h"

// BitFish uses 12 unsigned long long bitboards, one per piece, and a mailbox for piece lookups
// Extra bitboards for faster Move Generation
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

// A stack for GameInfos packed into a 32 bit integer
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
    
    // Hash Brown
    uint64_t hash;

    // Constructors, parses FEN, or else sets the starting position
    Position() {
        set_start_pos();
    }

    Position(std::string_view fen) {
        parse_fen(fen);
    }

    // Lookup functions
    Bitboard get_bitboard (Piece piece) const;
    Piece piece_at (Square square) const;

    // For printing the position
    std::string to_string() const;

    // Editting functions
    void update_occupancies();
    void set_square(Square square, Piece piece);
    void clear_square (Square square);
    void clear_pos();

    void set_start_pos();
    void parse_fen(const std::string_view fen = STARTING_POS_FEN);
    
    bool is_square_attacked (Square square, Color color) const;
    bool is_in_check (Color color) const;
    bool can_cap_king () const;
    bool can_castle_ks () const;
    bool can_castle_qs () const;

    

    void make_move (Move move);
    void undo_move ();

    void null_move ();
};


