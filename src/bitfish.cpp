// ------------------------------------ BITFISH ---------------------------------------

#include "bitfish.h"

using namespace std::chrono;

HashTable::HashTable(size_t mb) {
    size_t entries = (mb * (1 << 20)) / sizeof(HTEntry);

    table.resize(entries);
}

void HashTable::clear() {
    std::fill (table.begin(), table.end(), HTEntry{0, 0, 0, EXACT, NO_MOVE});
}

size_t HashTable::index(Key hash) const {
    return hash % table.size();
}

HTEntry* HashTable::probe (Key hash) {
    size_t idx = index(hash);
    if (table[idx].hash == hash) {
        return &table[idx];
    }

    return nullptr;
}


namespace BitFish {
    int current_depth = 0;
    bool stop_flag = false;
    steady_clock::time_point start_time;
    int max_time;
    uint64_t nodes;
    Position current_pos(STARTING_POS_FEN);

    std::array<std::array<Move, 2>, MAX_DEPTH> killers;

    bool should_stop() {
        if (stop_flag) return true;

        if (max_time > 0) {
            auto now = steady_clock::now();
            auto elapsed_ms = duration_cast<std::chrono::milliseconds>(now - start_time).count();
            if (elapsed_ms >= max_time) {
                stop_flag = true;  
                return true;
            }
        }
        return false;
    }

    void stop () {
        stop_flag = true;
    }

    float eg_weight (Position& pos) {
        int phase = 
            __builtin_popcountll(pos.get_bitboard(W_KNIGHT) | pos.get_bitboard(B_KNIGHT)) +
            __builtin_popcountll(pos.get_bitboard(W_BISHOP) | pos.get_bitboard(B_BISHOP)) +
            __builtin_popcountll(pos.get_bitboard(W_ROOK)   | pos.get_bitboard(B_ROOK))   +
            __builtin_popcountll(pos.get_bitboard(W_QUEEN)  | pos.get_bitboard(B_QUEEN));
        
        const int max_phase = 14;

        return 1 - std::min (1.0f, static_cast<float> (phase) / max_phase);
    }

    int evaluate (Position& pos) {
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

        score += ((WKS_RIGHT & pos.game_info.castling) >> 0) * 20;
        score += ((WQS_RIGHT & pos.game_info.castling) >> 1) * 20;
        score -= ((BKS_RIGHT & pos.game_info.castling) >> 2) * 20;
        score -= ((BQS_RIGHT & pos.game_info.castling) >> 3) * 20;


        score = std::max(-MAX_CP, std::min(score, MAX_CP));
        score = (pos.game_info.side_to_move == WHITE) ? score: -score;

        return score;
    }

    int qsearch (Position& pos, int depth, int alpha, int beta) {
        
        
        int stand_pat = evaluate (pos);

        if (should_stop ()) {
            return stand_pat;
        }
        nodes ++;
        if (depth == 0) {
            
            return stand_pat;
        }

        if (stand_pat >= beta) return beta;

        alpha = std::max (alpha, stand_pat);

        MoveList moves = MoveGen::generate_moves (pos);
        moves.sort(NO_MOVE);

        Color side_moving = pos.game_info.side_to_move;

        

        for (int i = 0; i < moves.size; ++i) {
            Move move = moves[i];

            bool is_noisy = (CAPTURED(move) != NO_PIECE) || (FLAG(move) >= MOVE_NPROMO_FLAG);
            
            // deltaprune

            int gain = std::abs(material[CAPTURED(move)])  - std::abs(material[MOVED(move)] + 100);

            if (stand_pat + gain + 200 < alpha) continue;

            if (!is_noisy) continue;

            pos.make_move(move);

            if (pos.is_in_check(side_moving)) {
                pos.undo_move();
                continue;
            }

            

            int score = -qsearch(pos, depth - 1, -beta, -alpha);

            pos.undo_move();

            nodes ++;

            if (score >= beta) {
                return beta;
            }

            if (nodes % 1024 == 0 && should_stop ()) {
                return 0;
            }

            alpha = std::max(alpha, score);
        }

        return alpha;
    }

    // no qsearch yet. keyword yet
    int minimax (Position& pos, int depth, int alpha, int beta, bool null_ok) {
        nodes++;

        
        if (should_stop ()) return 0;

        if (depth <= 0) {
            
            return qsearch(pos, MAX_QDEPTH, alpha, beta);
        }

        int ply_from_root = current_depth - depth;

        MoveList moves = MoveGen::generate_moves(pos);
        moves.sort(NO_MOVE, killers[ply_from_root][0], killers[ply_from_root][1]);

        Color color_moving = pos.game_info.side_to_move;

        int legal_moves = 0;

        int best_score = -INF;

        bool in_check = pos.is_in_check(pos.game_info.side_to_move);

        if (null_ok && !in_check && depth >= 3 && eg_weight(pos) < 0.6) {
            pos.null_move();
            int null_score = -minimax(pos, depth - 3, -beta, -beta + 1, false);
            pos.undo_move();
            
            if (null_score >= beta && std::abs(null_score) < MAX_CP) {
                //std::cout << "Beta: " << beta << "\n";
                //std::cout << "Null Score: " << null_score << "\n";
                //::cout << "Depths from root: " << ply_from_root << "\n";
                return beta;  // null prune
            }
        }
        
        for (int i = 0; i < moves.size; i++) {
            
            

            Move move = moves[i];


            
            pos.make_move(move);

            // skip illegal moves that leave king in check
            if (pos.is_in_check(color_moving)) {
                pos.undo_move();
                continue;
            }

            
            int score = -minimax(pos, depth - 1, -beta, -alpha);

            pos.undo_move();

            

            best_score = std::max(best_score, score);

            legal_moves ++;

            

            alpha = std::max(alpha, score);

            // beta cutoff
            if (alpha >= beta) {

                if (CAPTURED(move) == NO_PIECE)
                store_killer(move, ply_from_root);

                break;
            };

            if (nodes % 1024 == 0 && should_stop ()) {
                
                return 0;
            }

        }

        if (legal_moves == 0) {
            if (pos.is_in_check(color_moving)) return - (MATE_EVAL - ply_from_root);
            
            return 0;
        }

        return best_score;
    }

    

    std::pair<Move, int> get_best_move(Position& pos, int depth, Move pv) {
        

        Move best_move = NO_MOVE;
        int best_score = -INF;

        MoveList moves = MoveGen::generate_moves(pos);
        moves.sort(pv);
        
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

            if (nodes % 1024 == 0 && should_stop ()) {
                stop_flag = true;
                
            }

            if (stop_flag) return {NO_MOVE, 0};

            if (score > best_score) {
                best_score = score;
                best_move = m;
            }

        }

        



        return {best_move, best_score};

    }

    void go (int depth_lim, int move_time) {

        reset_killers();

        nodes = 0;
        stop_flag = false;
        start_time = steady_clock::now();
        max_time = move_time;

        bool mate_found = false;

        Move best_move = NO_MOVE;
        int eval;
        int multiplier = (current_pos.game_info.side_to_move == WHITE) ? 1: -1;

        int local_nodes = 0;

        for (int depth = 1; depth < depth_lim; ++depth) {

            
            if (!mate_found){
                auto result = get_best_move(current_pos, depth, best_move);
            
                if (should_stop()) {
                    break;
                }

                eval = result.second * multiplier;
                best_move = result.first;
            }
            
            

            if (std::abs(eval) > MAX_CP) {
                mate_found = true;
            }

            auto elapsed = duration_cast<microseconds> (steady_clock::now() - start_time).count();

            std::cout << "info depth " << depth << " score cp " << (eval) << " nodes " << (nodes - local_nodes) << " nps " << (nodes * 1000000 / elapsed) << " time " << (elapsed / 1000) << " pv " << move_to_string (best_move) << "\n"; 

            
        }

        std::cout << "bestmove " << move_to_string (best_move) << "\n";
        
        

    }

    Move iterative_deepen (int move_time) {

        reset_killers();

        nodes = 0;
        stop_flag = false;
        start_time = steady_clock::now();
        max_time = move_time;

        bool mate_found = false;

        Move best_move = NO_MOVE;
        int eval;
        int multiplier = (current_pos.game_info.side_to_move == WHITE) ? 1: -1;

        int local_nodes = 0;

        for (int depth = 1; depth < MAX_DEPTH; ++depth) {

            
            if (!mate_found){
                auto result = get_best_move(current_pos, depth, best_move);
            
                if (should_stop()) {
                    break;
                }

                eval = result.second * multiplier;
                best_move = result.first;
            }
            
            

            if (std::abs(eval) > MAX_CP) {
                mate_found = true;
            }

            

            
        }

        return best_move;
        
        

    }

    void position (std::string_view fen) {
        current_pos.parse_fen(fen);
    }
}