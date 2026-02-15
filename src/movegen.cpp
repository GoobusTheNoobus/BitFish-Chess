/**
 * movegen.cpp
 * 
 * MoveGen Implementation
 * Generates moves into a MoveList
 */

#include "movegen.h"

MoveList MoveGen::generate_moves (const Position& pos) {
    MoveList moves;

    generate_pawn_moves(pos, moves);
    generate_knight_moves(pos, moves);
    generate_bishop_moves(pos, moves);
    generate_rook_moves(pos, moves);
    generate_queen_moves(pos, moves);
    generate_king_moves(pos, moves);

    return moves;
}

void MoveGen::generate_pawn_moves (const Position& pos, MoveList& list) {   
    Color us = pos.game_info.side_to_move;
    Color them = opposite(us);

    bool is_white = us == WHITE;

    Piece moved = is_white? W_PAWN: B_PAWN;
    Bitboard pieces = pos.get_bitboard(moved);
    
    // Early exit
    if (!pieces) return;

    Bitboard r3_from_bottom = is_white ? Bitboards::rank3 : Bitboards::rank6;
    Bitboard promo = is_white ? Bitboards::rank8: Bitboards::rank1;

    Square ep = pos.game_info.ep_square;
    

    int push_offset = is_white ? 8: -8;
    int lc_offset = is_white ? 7: -9;
    int rc_offset = is_white ? 9: -7;

    Bitboard enemy_pieces = pos.board.color_bitboards[them];
    Bitboard occupancy = pos.board.occupancy;

    Bitboard single_push = (is_white ? pieces << 8: pieces >> 8) & ~occupancy;

    Bitboard sp_promo = single_push & promo;
    Bitboard sp_reg = single_push & ~promo;

    Bitboard double_push = (is_white ? ((single_push & r3_from_bottom) << 8): ((single_push & r3_from_bottom) >> 8)) & ~occupancy;

    Bitboard left_captures = (is_white ? 
        (pieces & ~Bitboards::file_a) << 7: 
        (pieces & ~Bitboards::file_a) >> 9) 
    & enemy_pieces;


    Bitboard right_captures = (is_white ? 
        (pieces & ~Bitboards::file_h) << 9: 
        (pieces & ~Bitboards::file_h) >> 7) 
    & enemy_pieces;

    Bitboard lc_promo = left_captures & promo;
    Bitboard lc_reg = left_captures & ~promo;
    Bitboard rc_promo = right_captures & promo;
    Bitboard rc_reg = right_captures & ~promo;

    

    while (sp_reg) {
        int square = __builtin_ctzll(sp_reg);
        sp_reg &= sp_reg - 1;
        list.push_back(NORMAL_MOVE(square - push_offset, square, moved, NO_PIECE));
    }

    while (sp_promo) {
        int square = __builtin_ctzll(sp_promo);
        sp_promo &= sp_promo - 1;
        Move move = NORMAL_MOVE(square - push_offset, square, moved, NO_PIECE);
        list.push_back(PROMO_MOVE(move, MOVE_QPROMO_FLAG));
        list.push_back(PROMO_MOVE(move, MOVE_RPROMO_FLAG));
        list.push_back(PROMO_MOVE(move, MOVE_BPROMO_FLAG));
        list.push_back(PROMO_MOVE(move, MOVE_NPROMO_FLAG));
    }

    while (double_push) {
        int square = __builtin_ctzll(double_push);
        double_push &= double_push - 1;
        list.push_back(DOUBLE_PUSH_MOVE(square - push_offset * 2, square, moved));
    }

    while (lc_reg) {
        int square = __builtin_ctzll(lc_reg);
        lc_reg &= lc_reg - 1;
        list.push_back(NORMAL_MOVE(square - lc_offset, square, moved, pos.piece_at(Square(square))));
    }

    while (lc_promo) {
        int square = __builtin_ctzll(lc_promo);
        lc_promo &= lc_promo - 1;
        Move move = NORMAL_MOVE(square - lc_offset, square, moved, pos.piece_at(Square(square)));
        list.push_back(PROMO_MOVE(move, MOVE_QPROMO_FLAG));
        list.push_back(PROMO_MOVE(move, MOVE_RPROMO_FLAG));
        list.push_back(PROMO_MOVE(move, MOVE_BPROMO_FLAG));
        list.push_back(PROMO_MOVE(move, MOVE_NPROMO_FLAG));
    }

    while (rc_reg) {
        int square = __builtin_ctzll(rc_reg);
        rc_reg &= rc_reg - 1;
        list.push_back(NORMAL_MOVE(square - rc_offset, square, moved, pos.piece_at(Square(square))));
    }

    while (rc_promo) {
        int square = __builtin_ctzll(rc_promo);
        rc_promo &= rc_promo - 1;
        Move move = NORMAL_MOVE(square - rc_offset, square, moved, pos.piece_at(Square(square)));
        list.push_back(PROMO_MOVE(move, MOVE_QPROMO_FLAG));
        list.push_back(PROMO_MOVE(move, MOVE_RPROMO_FLAG));
        list.push_back(PROMO_MOVE(move, MOVE_BPROMO_FLAG));
        list.push_back(PROMO_MOVE(move, MOVE_NPROMO_FLAG));
    }

    if (ep != NO_SQUARE) {
        // The opposite color bitboard contains the squares that our pawns have to be to en passant
        Bitboard en_passant_bb = Bitboards::get_pawn_attacks(ep, them) & pieces; 
        while (en_passant_bb) {
            int square = __builtin_ctzll (en_passant_bb);
            en_passant_bb &= en_passant_bb - 1;
            list.push_back(EN_PASSANT(square, ep, moved, is_white ? B_PAWN : W_PAWN));
        }
    }
     
    
}



void MoveGen::generate_knight_moves (const Position& pos, MoveList& list) {
    Color us = pos.game_info.side_to_move;
    Color them = opposite(us);

    Bitboard friendlies = pos.board.color_bitboards[us];
    Bitboard enemies = pos.board.color_bitboards[them];

    Piece moved = us == WHITE ? W_KNIGHT : B_KNIGHT;
    Bitboard pieces = pos.get_bitboard(moved);

    // Early Exit
    if (!pieces) return;

    while (pieces) {
        int from = __builtin_ctzll (pieces);
        Square square_enum = Square(from);

        Bitboard move_bb = Bitboards::get_knight_attacks(square_enum) & ~friendlies;

        while (move_bb) {
            Square to = Square(__builtin_ctzll(move_bb));

            list.push_back(NORMAL_MOVE(from, to, moved, pos.piece_at(to)));
            move_bb &= move_bb - 1;
        }

        // Clear LSB
        pieces &= pieces - 1;
    }
}


void MoveGen::generate_king_moves (const Position& pos, MoveList& list) {
    Color us = pos.game_info.side_to_move;
    Color them = opposite(us);

    Bitboard friendlies = pos.board.color_bitboards[us];
    Bitboard enemies = pos.board.color_bitboards[them];

    Piece moved = us == WHITE ? W_KING : B_KING;
    Bitboard pieces = pos.get_bitboard(moved);

    // early exit
    if (!pieces) return;

    while (pieces) {
        int from = __builtin_ctzll (pieces);
        Square square_enum = Square(from);

        Bitboard move_bb = Bitboards::get_king_attacks(square_enum) & ~friendlies;

        while (move_bb) {
            Square to = Square(__builtin_ctzll(move_bb));

            list.push_back(NORMAL_MOVE(from, to, moved, pos.piece_at(to)));
            move_bb &= move_bb - 1;
        }

        // Clear LSB
        pieces &= pieces - 1;
    }

    // Check castling
    if (pos.can_castle_ks()) {
        list.push_back(CASTLING_MOVE(us == WHITE ? E1: E8, us == WHITE ? G1: G8, moved));
    }

    if (pos.can_castle_qs()) {
        list.push_back(CASTLING_MOVE(us == WHITE ? E1: E8, us == WHITE ? C1: C8, moved));
    }
}


void MoveGen::generate_bishop_moves (const Position& pos, MoveList& list) {
    Color us = pos.game_info.side_to_move;
    Color them = opposite(us);

    Bitboard friendlies = pos.board.color_bitboards[us];
    Bitboard enemies = pos.board.color_bitboards[them];

    Piece moved = us == WHITE ? W_BISHOP : B_BISHOP;
    Bitboard pieces = pos.get_bitboard(moved);

    // Early exit
    if (!pieces) return;

    while (pieces) {

        int from = __builtin_ctzll (pieces);
        Square square_enum = Square(from);

        Bitboard move_bb = Bitboards::get_bishop_attacks(square_enum, pos.board.occupancy) & ~friendlies;

        while (move_bb) {
            Square to = Square(__builtin_ctzll(move_bb));

            list.push_back(NORMAL_MOVE(from, to, moved, pos.piece_at(to)));
            move_bb &= move_bb - 1;
        }

        // Clear LSB
        pieces &= pieces - 1;
    }
}


void MoveGen::generate_rook_moves (const Position& pos, MoveList& list) {
    Color us = pos.game_info.side_to_move;
    Color them = opposite(us);

    Bitboard friendlies = pos.board.color_bitboards[us];
    Bitboard enemies = pos.board.color_bitboards[them];

    Piece moved = us == WHITE ? W_ROOK : B_ROOK;
    Bitboard pieces = pos.get_bitboard(moved);

    // Early exit
    if (!pieces) return;

    while (pieces) {

        int from = __builtin_ctzll (pieces);
        Square square_enum = Square(from);

        Bitboard move_bb = Bitboards::get_rook_attacks(square_enum, pos.board.occupancy) & ~friendlies;

        while (move_bb) {
            Square to = Square(__builtin_ctzll(move_bb));

            list.push_back(NORMAL_MOVE(from, to, moved, pos.piece_at(to)));
            move_bb &= move_bb - 1;
        }

        // clear lsb
        pieces &= pieces - 1;
    }
}


void MoveGen::generate_queen_moves (const Position& pos, MoveList& list) {
    Color us = pos.game_info.side_to_move;
    Color them = opposite(us);

    Bitboard friendlies = pos.board.color_bitboards[us];
    Bitboard enemies = pos.board.color_bitboards[them];

    Piece moved = us == WHITE ? W_QUEEN : B_QUEEN;

    Bitboard pieces = pos.get_bitboard(moved);

    // Early exit
    if (!pieces) return;

    while (pieces) {

        int from = __builtin_ctzll (pieces);
        Square square_enum = Square(from);

        // Combine bishop and rook attacks from that square into one bitboard
        Bitboard move_bb = (Bitboards::get_bishop_attacks(square_enum, pos.board.occupancy) | Bitboards::get_rook_attacks(square_enum, pos.board.occupancy)) & ~friendlies;

        while (move_bb) {
            Square to = Square(__builtin_ctzll(move_bb));

            list.push_back(NORMAL_MOVE(from, to, moved, pos.piece_at(to)));
            move_bb &= move_bb - 1;
        }

        // Clear LSB
        pieces &= pieces - 1;
    }}




