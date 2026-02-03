#include "bitfish.h"



namespace BitFish {

    float Eval::eg_weight (Position& pos) {
        int phase = 
            __builtin_popcountll(pos.get_bitboard(W_KNIGHT) | pos.get_bitboard(B_KNIGHT)) +
            __builtin_popcountll(pos.get_bitboard(W_BISHOP) | pos.get_bitboard(B_BISHOP)) +
            __builtin_popcountll(pos.get_bitboard(W_ROOK)   | pos.get_bitboard(B_ROOK))   +
            __builtin_popcountll(pos.get_bitboard(W_QUEEN)  | pos.get_bitboard(B_QUEEN));
        
        const int max_phase = 16;

        return 1 - std::min (1.0f, static_cast<float> (phase) / max_phase);
    }

    int Eval::evaluate (Position& pos) {
        float endgame = eg_weight(pos);

        int score = 0;

        for (int square = 0; square < BOARD_SIZE; ++square) {
            Piece piece = pos.piece_at(Square(square));

            if (piece == NO_PIECE) continue;

            switch (piece)
            {
            case W_PAWN:
                score += static_cast<int>(endgame * pawn_table_eg[square] + (1 - endgame) * pawn_table_mg[square]);
                break;
            case B_PAWN:
                score -= static_cast<int>(endgame * pawn_table_eg[63 - square] + (1 - endgame) * pawn_table_mg[63 - square]);
                break;
            case W_KNIGHT:
                score += knight_table[square];
                break;
            case B_KNIGHT:
                score -= knight_table[63 - square];
                break;
            case W_BISHOP:
                score += bishop_table[square];
                break;
            case B_BISHOP:
                score -= bishop_table[63 - square];
                break;
            case W_ROOK:
                score += rook_table[square];
                break;
            case B_ROOK:
                score -= rook_table[63 - square];
                break;
            case W_QUEEN:
                score += queen_table[square];
                break;
            case B_QUEEN:
                score -= queen_table[63 - square];
                break;
            case W_KING:
                score += static_cast<int>(endgame * king_table_eg[square] + (1 - endgame) * king_table_mg[square]);
                break;
            case B_KING:
                score -= static_cast<int>(endgame * king_table_eg[63 - square] + (1 - endgame) * king_table_mg[63 - square]);
            default:
                break;
            }

            score += material[piece];
        }

        score = std::max(-MAX_CP, std::min(score, MAX_CP));
        score = (pos.game_info.side_to_move == WHITE) ? score: -score;

        return score;
    }

    // no qsearch yet. keyword yet
    int Searcher::minimax (Position& pos, int depth, int alpha, int beta) {

        if (depth <= 0) {
            nodes++;
            return Eval::evaluate(pos);
        }

        MoveList moves = MoveGen::generate_moves(pos);

        Color color_moving = pos.game_info.side_to_move;

        int legal_moves = 0;

        int best_score = -INF;
        
        for (int i = 0; i < moves.size; i++) {
            
            Move move = moves[i];

            
            pos.make_move(move);

            // skip illegal moves that leave king in check
            if (pos.is_in_check(color_moving)) {
                pos.undo_move();
                continue;
            }

            nodes ++;
            int score = -minimax(pos, depth - 1, -beta, -alpha);

            pos.undo_move();

            best_score = std::max(best_score, score);

            legal_moves ++;

            

            alpha = std::max(alpha, score);

            // beta cutoff
            if (alpha >= beta) break;

            

        }

        if (legal_moves == 0) {
            if (pos.is_in_check(color_moving)) return -(MATE_EVAL - (current_depth - depth));
            if (pos.is_in_check(opposite(color_moving))) return MATE_EVAL - (current_depth - depth);
            return 0;
        }

        return best_score;
    }

    std::pair<Move, int> Searcher::get_best_move(Position& pos, int depth) {
        

        Move best_move = -1;
        int best_score = -INF;

        MoveList moves = MoveGen::generate_moves(pos);
        
        Color side_moving = pos.game_info.side_to_move;

        current_depth = depth;
        

        for (int i = 0; i < moves.size; ++i) {
            Move m = moves[i];

            pos.make_move(m);

            if (pos.is_in_check(side_moving)) {
                pos.undo_move();
                continue;
            }

            int score = -minimax(pos, depth - 1, -INF, INF);

            pos.undo_move();

            if (score > best_score) {
                best_score = score;
                best_move = m;
            }

        }

        std::cout << "Nodes: " << nodes;

        return {best_move, best_score};

    }
}