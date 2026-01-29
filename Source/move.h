#pragma once
#include "type.h"
#include "position.h"
#include <vector>

namespace Encode {
    /*
     Encoding Format:
     0-5: From
     6-11: To
     12-15: Moved
     16-19: Captured
     20-23: Flags

     24-27: Prev Castling
     28-34: Prev EnPassantSquare (7 bits instead of 6 for no_square)
     35-41: Prev 50-Move-Clock

     Flags:
     0b0000 Normal
     0b0001 Castling
     0b0010 EnPassant
     0b0011 DoublePawnPush
     0b0100 PromoKnight
     0b0101 PromoBishop
     0b0110 PromoRook
     0b0111 PromoQueen

     just allocated 4 bits instead of 3 for flags in case I add more
    */
    

    inline Move normal (const Position& pos, Square from, Square to) {
        Move move = 0ULL;

        move = from;
        move |= to << 6;
        move |= pos.piece_at(from) << 12;
        move |= pos.piece_at(to) << 16;

        if ((pos.piece_at(from) == W_PAWN || pos.piece_at(from) == B_PAWN) && abs(from - to) == 16) {
            move |= MOVE_DOUBLE_PUSH_FLAG << 20;
        }
        move |= pos.game_info.castling << 24;
        move |= pos.game_info.ep_square << 28;

        // 50 move clock
        move |= uint64_t(pos.game_info.rule_50_clock) << 35;

        return move;

    }

    inline Move castling (const Position& pos, bool king_side) {
        Move move = 0ULL;

        bool white = pos.game_info.side_to_move == WHITE;

        move |= white ? E1: E8;
        move |= (white ? (king_side ? G1: C1): (king_side ? G8: C8)) << 6;
        move |= (white ? W_KING: B_KING) << 12;
        move |= NO_PIECE << 16;

        // flag
        move |= MOVE_CASTLING_FLAG << 20;

        move |= pos.game_info.castling << 24;
        move |= pos.game_info.ep_square << 28;

        // 50 move clock
        move |= uint64_t(pos.game_info.rule_50_clock) << 35;

        return move;
    }

    inline Move en_passant (const Position& pos, Square from, Square to) {
        Move move = 0ULL;

        bool white = pos.game_info.side_to_move == WHITE;

        move |= from;
        move |= to << 6;
        move |= pos.piece_at(Square(from)) << 12;
        move |= (white ? B_PAWN: W_PAWN) << 16;

        // flag
        move |= MOVE_ENPASSANT_FLAG << 20;

        move |= pos.game_info.castling << 24;
        move |= pos.game_info.ep_square << 28;

        // 50 move clock
        move |= uint64_t(pos.game_info.rule_50_clock) << 35;

        return move;


    }

    inline Move promotion (const Position& pos, Square from, Square to, int flag) {
        Move move = 0ULL;

        bool white = pos.game_info.side_to_move == WHITE;

        move |= from;
        move |= to << 6;
        move |= pos.piece_at(Square(from)) << 12;
        move |= pos.piece_at(Square(to)) << 16;

        // flag
        move |= flag << 20;

        move |= pos.game_info.castling << 24;
        move |= pos.game_info.ep_square << 28;

        // 50 move clock
        move |= uint64_t(pos.game_info.rule_50_clock) << 35;

        return move;


    }


    // lookups
    inline Square from (Move move) {
        return Square(move & 63);
    }

    inline Square to (Move move) {
        return Square((move >> 6) & 63);
    }

    inline Piece moved (Move move) {
        return Piece((move >> 12) & 15);
    } 

    inline Piece captured (Move move) {
        return Piece((move >> 16) & 15);
    }

    inline int get_flag (Move move) {
        return (move >> 20) & 15;
    }

    inline CastlingRights get_prev_cr (Move move) {
        return (move >> 24) & 15;
    }

    inline Square get_prev_eps (Move move) {
        return Square((move >> 28) & 127);
    }

    inline int get_prev_frc (Move move) {
        return move >> 35;
    }

    inline bool is_promotion (Move move) {
        return get_flag(move) >= MOVE_NPROMO_FLAG;
    }

    inline std::string to_string (Move move) {
        


        if (is_promotion(move)) {
            static constexpr std::array<char, 4> promo_chars = {'n', 'b', 'r', 'q'};

            return square_to_str(from(move)) + square_to_str(to(move)) + promo_chars[get_flag(move) - MOVE_NPROMO_FLAG];
        }

        return square_to_str(from(move)) + square_to_str(to(move));
    }

    inline void print_move_list (std::vector<Move> list) {
        for (Move move: list) {
            std::cout << to_string(move) << "\n";
        }
        std::cout << "Size: " << list.size() << std::endl;
    }

    
}
