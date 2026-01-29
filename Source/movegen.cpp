#include "movegen.h"

namespace MoveGen {

    void generate_pawn_moves (GenType gen_type, const Position& pos, std::vector<Move>& list) {
        Color us = pos.game_info.side_to_move;
        Color them = opposite(us);

        Bitboard pieces = us == WHITE ? pos.get_bitboard(W_PAWN): pos.get_bitboard(B_PAWN);

        // early exit
        if (!pieces) return;

        Bitboard enemy_pieces = pos.board.color_bitboards[them];
        Bitboard occupancy = pos.board.occupancy;

        int pawn_start_rank = us == WHITE ? 1: 6;
        int pawn_promo_rank = us == WHITE ? 6: 1;
        
        while (pieces) {
            int from = __builtin_ctzll (pieces);
            int from_rank = from >> 3;

            bool is_promo = from_rank == pawn_promo_rank;

            Square squEnum = Square(from);

            
            if (gen_type != CAPTURES_PL) {
                // pushes
                Square single_push = Square(from + (pos.game_info.side_to_move == WHITE ? 8: -8));

                if ((occupancy & (1ULL << single_push)) == 0ULL) {
                    if (is_promo) {
                        list.push_back(Encode::promotion(pos, squEnum, single_push, MOVE_QPROMO_FLAG));
                        list.push_back(Encode::promotion(pos, squEnum, single_push, MOVE_RPROMO_FLAG));
                        list.push_back(Encode::promotion(pos, squEnum, single_push, MOVE_NPROMO_FLAG));
                        list.push_back(Encode::promotion(pos, squEnum, single_push, MOVE_BPROMO_FLAG));
                    }
                        
                    else {
                        list.push_back(Encode::normal(pos, squEnum, single_push));
                    }

                    int double_push = from + (pos.game_info.side_to_move == WHITE ? 16: -16);

                    // if single push is achievable, it assumes that the square in between is empty
                    if (from_rank == pawn_start_rank && (occupancy & (1ULL << double_push)) == 0ULL) {
                        list.push_back (Encode::normal(pos, squEnum, Square(double_push)));
                    }
                }


            }

            // Captures now

            Bitboard capture_bitboard = Bitboards::get_pawn_attacks(squEnum, pos.game_info.side_to_move);

            Bitboard regular_captures = capture_bitboard & enemy_pieces;

            

            while (regular_captures) {
                Square captured_square = Square(__builtin_ctzll(regular_captures));
                

                if (is_promo) {
                    list.push_back(Encode::promotion(pos, squEnum, captured_square, MOVE_QPROMO_FLAG));
                    list.push_back(Encode::promotion(pos, squEnum, captured_square, MOVE_RPROMO_FLAG));
                    list.push_back(Encode::promotion(pos, squEnum, captured_square, MOVE_NPROMO_FLAG));
                    list.push_back(Encode::promotion(pos, squEnum, captured_square, MOVE_BPROMO_FLAG));
                } else {
                    list.push_back(Encode::normal(pos, squEnum, captured_square));
                }

                regular_captures &= regular_captures - 1;
                
            }

            // en peasant you peasant

            if (pos.game_info.ep_square != NO_SQUARE) {
                Bitboard en_passant_bitboard = (1ULL << pos.game_info.ep_square) & capture_bitboard;

                if (en_passant_bitboard != 0ULL) {
                    list.push_back(Encode::en_passant(pos, squEnum, Square(__builtin_ctzll(en_passant_bitboard))));
                }
            }

            

            // clear lsb
            pieces &= pieces - 1;

        }
    }

    void generate_knight_moves (GenType gen_type, const Position& pos, std::vector<Move>& list) {
        Color us = pos.game_info.side_to_move;
        Color them = opposite(us);

        Bitboard friendlies = pos.board.color_bitboards[us];
        Bitboard enemies = pos.board.color_bitboards[them];

        Bitboard pieces = us == WHITE ? pos.get_bitboard(W_KNIGHT): pos.get_bitboard(B_KNIGHT);

        // early exit
        if (!pieces) return;

        while (pieces) {
            int from = __builtin_ctzll (pieces);
            Square squEnum = Square(from);

            Bitboard move_bb = Bitboards::get_knight_attacks(squEnum) & ((gen_type != CAPTURES_PL) ? ~friendlies: enemies);

            while (move_bb) {
                Square to = Square(__builtin_ctzll(move_bb));

                list.push_back(Encode::normal(pos, squEnum, to));
                move_bb &= move_bb - 1;
            }

            // clear lsb
            pieces &= pieces - 1;
        }
    }

    void generate_king_moves (GenType gen_type, const Position& pos, std::vector<Move>& list) {
        Color us = pos.game_info.side_to_move;
        Color them = opposite(us);

        Bitboard friendlies = pos.board.color_bitboards[us];
        Bitboard enemies = pos.board.color_bitboards[them];

        Bitboard pieces = us == WHITE ? pos.get_bitboard(W_KING): pos.get_bitboard(B_KING);

        // early exit
        if (!pieces) return;

        while (pieces) {
            int from = __builtin_ctzll (pieces);
            Square squEnum = Square(from);

            Bitboard move_bb = Bitboards::get_king_attacks(squEnum) & ((gen_type != CAPTURES_PL) ? ~friendlies: enemies);

            while (move_bb) {
                Square to = Square(__builtin_ctzll(move_bb));

                list.push_back(Encode::normal(pos, squEnum, to));
                move_bb &= move_bb - 1;
            }

            // clear lsb
            pieces &= pieces - 1;
        }

        // TO-DO: CASTLING
    }

    void generate_bishop_moves (GenType gen_type, const Position& pos, std::vector<Move>& list) {
        Color us = pos.game_info.side_to_move;
        Color them = opposite(us);

        Bitboard friendlies = pos.board.color_bitboards[us];
        Bitboard enemies = pos.board.color_bitboards[them];

        Bitboard pieces = us == WHITE ? pos.get_bitboard(W_BISHOP): pos.get_bitboard(B_BISHOP);

        // early exit
        if (!pieces) return;

        while (pieces) {

            int from = __builtin_ctzll (pieces);
            Square squEnum = Square(from);

            Bitboard move_bb = Bitboards::get_bishop_attacks(squEnum, pos.board.occupancy) & ((gen_type != CAPTURES_PL) ? ~friendlies: enemies);

            while (move_bb) {
                Square to = Square(__builtin_ctzll(move_bb));

                list.push_back(Encode::normal(pos, squEnum, to));
                move_bb &= move_bb - 1;
            }

            // clear lsb
            pieces &= pieces - 1;
        }
    }

    void generate_rook_moves (GenType gen_type, const Position& pos, std::vector<Move>& list) {
        Color us = pos.game_info.side_to_move;
        Color them = opposite(us);

        Bitboard friendlies = pos.board.color_bitboards[us];
        Bitboard enemies = pos.board.color_bitboards[them];

        Bitboard pieces = us == WHITE ? pos.get_bitboard(W_ROOK): pos.get_bitboard(B_ROOK);

        // early exit
        if (!pieces) return;

        while (pieces) {

            int from = __builtin_ctzll (pieces);
            Square squEnum = Square(from);

            Bitboard move_bb = Bitboards::get_rook_attacks(squEnum, pos.board.occupancy) & ((gen_type != CAPTURES_PL) ? ~friendlies: enemies);

            while (move_bb) {
                Square to = Square(__builtin_ctzll(move_bb));

                list.push_back(Encode::normal(pos, squEnum, to));
                move_bb &= move_bb - 1;
            }

            // clear lsb
            pieces &= pieces - 1;
        }
    }

    void generate_queen_moves (GenType gen_type, const Position& pos, std::vector<Move>& list) {
        Color us = pos.game_info.side_to_move;
        Color them = opposite(us);

        Bitboard friendlies = pos.board.color_bitboards[us];
        Bitboard enemies = pos.board.color_bitboards[them];

        Bitboard pieces = us == WHITE ? pos.get_bitboard(W_QUEEN): pos.get_bitboard(B_QUEEN);

        // early exit
        if (!pieces) return;

        while (pieces) {

            int from = __builtin_ctzll (pieces);
            Square squEnum = Square(from);

            Bitboard move_bb = (Bitboards::get_bishop_attacks(squEnum, pos.board.occupancy) | Bitboards::get_rook_attacks(squEnum, pos.board.occupancy)) & ((gen_type != CAPTURES_PL) ? ~friendlies: enemies);

            while (move_bb) {
                Square to = Square(__builtin_ctzll(move_bb));

                list.push_back(Encode::normal(pos, squEnum, to));
                move_bb &= move_bb - 1;
            }

            // clear lsb
            pieces &= pieces - 1;
        }
    }

    std::vector<Move> generate_moves (GenType gen_type, const Position& pos) {
        std::vector<Move> moves;
        generate_queen_moves(gen_type, pos, moves);
        generate_pawn_moves(gen_type, pos, moves);
        generate_rook_moves(gen_type, pos, moves);
        generate_bishop_moves(gen_type, pos, moves);
        generate_knight_moves(gen_type, pos, moves);
        generate_king_moves(gen_type, pos, moves);

        return moves;
    }
} 
