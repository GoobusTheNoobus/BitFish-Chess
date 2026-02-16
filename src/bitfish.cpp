/**
 * bitfish.cpp
 * 
 * Search Implementation
 * Uses minimax with negamax impl
 * Uses classical evaluation
 */

#include "bitfish.h"


using namespace std::chrono;


HashTable::HashTable(size_t mb) {
    // 1 << 20 is 1024^2
    size_t entries = (mb * (1 << 20)) / sizeof(HTEntry);

    table.resize(entries);
}

void HashTable::clear() {
    std::fill (table.begin(), table.end(), HTEntry{0ULL, 0, 0, EXACT, NO_MOVE});
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

void HashTable::store (Key hash, int depth, int score, HTFlag flag, Move best_move) {
    size_t idx = index(hash);

    
    if (table[idx].hash == 0 || 
        table[idx].hash == hash || 
        depth >= table[idx].depth) {
        table[idx] = HTEntry{hash, depth, score, flag, best_move};
    }
}

void SearchInfo::reset () {
    depth = 0;
    stop.store (false, std::memory_order_relaxed);
    nodes = 0;
}


// Main Search Impl
namespace BitFish {
    
    // Contains information for timing, depth, and when to stop
    SearchInfo search_info;

    // The current position, set by using the 'position' uci command
    Position current_pos(STARTING_POS_FEN);

    // Transposition Table for move ordering
    HashTable tt;

    // Killer Moves: If one move is good at this depth in this branch
    // Try it another branch
    // Reset every search
    std::array<std::array<Move, 2>, MAX_DEPTH> killers;

    // Killer functions
    void reset_killers () {
        std::memset(killers.begin(), NO_MOVE, sizeof(killers));
    }

    void store_killer (Move move, int ply) {
        if (killers[ply][0] == move) return;
        if (killers[ply][1] == move) return;

        // shift
        killers[ply][1] = killers[ply][0];
        killers[ply][0] = move;
    }

    // UCI related function impl
    void position (std::string_view fen) {
        current_pos.parse_fen(fen);
    }

    void stop () {
        search_info.stop.store(true, std::memory_order_relaxed);
    }

    bool should_stop() {
        if (search_info.stop.load(std::memory_order_relaxed)) return true;

        if (search_info.max_time_ms > 0) {
            auto now = steady_clock::now();
            auto elapsed_ms = duration_cast<std::chrono::milliseconds>(now - search_info.start_time).count();
            if (elapsed_ms >= search_info.max_time_ms) {
                return true;
            }
        }
        return false;
    }

    // Endgame weight for evaluation
    float eg_weight (Position& pos) {

        // Include every piece except for kings
        int phase = 
            __builtin_popcountll(pos.get_bitboard(W_KNIGHT) | pos.get_bitboard(B_KNIGHT)) * KNIGHT_GAME_PHASE +
            __builtin_popcountll(pos.get_bitboard(W_BISHOP) | pos.get_bitboard(B_BISHOP)) * BISHOP_GAME_PHASE +
            __builtin_popcountll(pos.get_bitboard(W_ROOK)   | pos.get_bitboard(B_ROOK)) * ROOK_GAME_PHASE +
            __builtin_popcountll(pos.get_bitboard(W_QUEEN)  | pos.get_bitboard(B_QUEEN)) * QUEEN_GAME_PHASE;
        
        

        return 1 - std::min (1.0f, static_cast<float> (phase) / MAX_GAME_PHASE);
    }

    
    // Returns a static evaluation of the current position
    int evaluate (Position& pos) {
        // Draw by 50 move rule
        if (pos.game_info.rule_50_clock >= 100) {
            return 0;
        }

        float endgame = eg_weight(pos);

        int score = 0;

        // Single pass through the board
        // Considers mobility and material
        for (int square = 0; square < BOARD_SIZE; ++square) {
            Piece piece = pos.piece_at(Square(square));

            if (piece == NO_PIECE) continue;

            switch (piece)
            {
            case W_PAWN:
                score += static_cast<int>(endgame * EvalTables::pawn_table_eg[square] + (1 - endgame) * EvalTables::pawn_table_mg[square]);
                break;
            case B_PAWN:
                score -= static_cast<int>(endgame * EvalTables::pawn_table_eg[63 - square] + (1 - endgame) * EvalTables::pawn_table_mg[63 - square]);
                
                break;
            case W_KNIGHT:
                score += EvalTables::knight_table[square];
                score += __builtin_popcountll(Bitboards::get_knight_attacks(Square(square))) * KNIGHT_MOB_BONUS;
                break;
            case B_KNIGHT:
                score -= EvalTables::knight_table[63 - square];
                score -= __builtin_popcountll(Bitboards::get_knight_attacks(Square(square))) * KNIGHT_MOB_BONUS;
                break;
            case W_BISHOP:
                score += EvalTables::bishop_table[square];
                score += __builtin_popcountll(Bitboards::get_bishop_attacks(Square(square), pos.board.occupancy)) * BISHOP_MOB_BONUS;
                break;
            case B_BISHOP:
                score -= EvalTables::bishop_table[63 - square];
                score -= __builtin_popcountll(Bitboards::get_bishop_attacks(Square(square), pos.board.occupancy)) * BISHOP_MOB_BONUS;
                break;
            case W_ROOK:
                score += EvalTables::rook_table[square];
                score += __builtin_popcountll(Bitboards::get_rook_attacks(Square(square), pos.board.occupancy)) * ROOK_MOB_BONUS;
                break;
            case B_ROOK:
                score -= EvalTables::rook_table[63 - square];
                score -= __builtin_popcountll(Bitboards::get_rook_attacks(Square(square), pos.board.occupancy)) * ROOK_MOB_BONUS;
                break;
            case W_QUEEN:
                score += EvalTables::queen_table[square];
                score += __builtin_popcountll(Bitboards::get_rook_attacks(Square(square), pos.board.occupancy) | Bitboards::get_bishop_attacks(Square(square), pos.board.occupancy)) * QUEEN_MOB_BONUS;
                break;
            case B_QUEEN:
                score -= EvalTables::queen_table[63 - square];
                score -= __builtin_popcountll(Bitboards::get_rook_attacks(Square(square), pos.board.occupancy) | Bitboards::get_bishop_attacks(Square(square), pos.board.occupancy)) * QUEEN_MOB_BONUS;
                break;
            case W_KING:
                score += static_cast<int>(endgame * EvalTables::king_table_eg[square] + (1 - endgame) * EvalTables::king_table_mg[square]);
                break;
            case B_KING:
                score -= static_cast<int>(endgame * EvalTables::king_table_eg[63 - square] + (1 - endgame) * EvalTables::king_table_mg[63 - square]);
                break;
            default:
                break;
            }

            score += material[piece];
        }

        // Castling Right Bonus
        score += ((WKS_RIGHT & pos.game_info.castling) >> 0) * KINGSIDE_CASTLING_BONUS;
        score += ((WQS_RIGHT & pos.game_info.castling) >> 1) * QUEENSIDE_CASTLING_BONUS;
        score -= ((BKS_RIGHT & pos.game_info.castling) >> 2) * KINGSIDE_CASTLING_BONUS;
        score -= ((BQS_RIGHT & pos.game_info.castling) >> 3) * QUEENSIDE_CASTLING_BONUS;

        if (__builtin_popcount(pos.get_bitboard(W_BISHOP)) >= 2) {
            score += BISHOP_PAIR_BONUS;
        }

        if (__builtin_popcount(pos.get_bitboard(B_BISHOP)) >= 2) {
            score -= BISHOP_PAIR_BONUS;
        }

        // Isolated Pawn
        Bitboard white_pawns = pos.get_bitboard(W_PAWN);
        Bitboard white_pawns_copy = white_pawns;
        Bitboard black_pawns = pos.get_bitboard(B_PAWN);
        Bitboard black_pawns_copy = black_pawns;

        while (white_pawns) {
            Square square = Square(__builtin_ctzll(white_pawns));
            int file = square & 7;

            Bitboard adjacent = 0ULL;

            if (file > 0) {
                adjacent |= Bitboards::file_a >> (file - 1);
            } 
            if (file < 7) {
                adjacent |= Bitboards::file_a >> (file + 1);
            }

            if (!(adjacent & white_pawns_copy)) {
                score -= ISOLATED_PAWN_PENALTY;
            }

            white_pawns &= white_pawns - 1;
        }

        while (black_pawns) {
            Square square = Square(__builtin_ctzll (black_pawns));
            int file = square & 7;

            Bitboard adjacent = 0ULL;

            if (file > 0) {
                adjacent |= Bitboards::file_a >> (file - 1);
            } 
            if (file < 7) {
                adjacent |= Bitboards::file_a >> (file + 1);
            }

            if (!(adjacent & black_pawns_copy)) {
                score += ISOLATED_PAWN_PENALTY;
            }

            black_pawns &= black_pawns - 1;
        }



        // Bishop blocking pawn, trapping other bishop
        if (endgame < 0.5) {
            if (pos.piece_at(E3) == W_BISHOP && pos.piece_at(E2) == W_PAWN) {
                score -= 30;
            }
            if (pos.piece_at(D3) == W_BISHOP && pos.piece_at(D2) == W_PAWN) {
                score -= 30;
            }
            if (pos.piece_at(E6) == B_BISHOP && pos.piece_at(E7) == B_PAWN) {
                score += 30;
            }
            if (pos.piece_at(D6) == B_BISHOP && pos.piece_at(D7) == B_PAWN) {
                score += 30;
            }
        }

        

        // Clamp the score so it doesn't get interpreted as a mate
        score = std::max(-MAX_CP, std::min(score, MAX_CP));

        // Negamax
        score = (pos.game_info.side_to_move == WHITE) ? score: -score;

        return score;
    }

    

    // technically negamax
    int minimax (Position& pos, int depth, int alpha, int beta, bool null_ok) {
        search_info.nodes ++;

        if (should_stop ()) return 0;

        // copy value for later use
        int original_alpha = alpha;

        if (depth <= 0) {
            
            return qsearch(pos, MAX_QDEPTH, alpha, beta);
        }

        // Probe from transposition table 
        HTEntry* entry = tt.probe(pos.hash);
        Move tt_move = NO_MOVE;

        if (entry != nullptr) {
            tt_move = entry->best_move;
        }

        int ply_from_root = search_info.depth - depth;

        // Search moves
        Color color_moving = pos.game_info.side_to_move;
        MoveList moves = MoveGen::generate_moves(pos);
        moves.sort(tt_move, killers[ply_from_root][0], killers[ply_from_root][1]);

        // Keep track how many legal moves there are, in case it is a checkmate or stalemate
        int legal_moves = 0;

        bool in_check = pos.is_in_check(pos.game_info.side_to_move);

        // Null Move Pruning
        if (null_ok && !in_check && depth >= 3 && eg_weight(pos) < 0.7) {
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

        int best_score = -INF;
        Move best_move = NO_MOVE;

        int i = 0; 
        for (Move move: moves) {
            
            
            pos.make_move(move);

            // skip illegal moves that leave king in check
            if (pos.is_in_check(color_moving)) {
                pos.undo_move();
                continue;
            }

            
            int score;

            // Late move reduction
            if (i > 3 && depth >= 3 && !in_check && CAPTURED(move) == NO_PIECE && FLAG(move) < MOVE_NPROMO_FLAG) {
                score = -minimax(pos, depth - 2, -alpha - 1, -alpha);

                if (score > alpha) {
                    score = -minimax(pos, depth - 1, -beta, -alpha);
                }
            } 
            
            // No late move reduction
            else {
                score = -minimax(pos, depth - 1, -beta, -alpha);
            }

            pos.undo_move();

            if (score > best_score) {
                best_score = score;
                best_move = move;
            }

            legal_moves ++;

            // Update alpha
            alpha = std::max(alpha, score);

            // Beta cutoff
            if (alpha >= beta) {

                if (CAPTURED(move) == NO_PIECE)
                store_killer(move, ply_from_root);
                
                i++;
                break;
            };

            if (search_info.nodes % 1024 == 0 && should_stop ()) {
                return 0;
            }

            i++;

        }

        if (legal_moves == 0) {
            // Checkmate
            if (pos.is_in_check(color_moving)) return - (MATE_EVAL - ply_from_root);
        
            // Stalemate
            return 0;
        }

        HTFlag flag;
        if (best_score <= original_alpha) {
            flag = AT_MOST;
        } else if (best_score >= beta) {
            flag = AT_LEAST;
        } else {
            flag = EXACT;
        }

        // store in tt 
        tt.store(pos.hash, depth, best_score, flag, best_move);

        return best_score;
    }

    // Quiescence search to fix horizon effect
    int qsearch (Position& pos, int depth, int alpha, int beta) {
        search_info.nodes ++;

        if (should_stop ()) {
            stop();
            return 0;
        }
      
        if (depth == 0) {
            return evaluate(pos);
        }

        int stand_pat = evaluate (pos);

        // Beta cutoff
        if (stand_pat >= beta) return beta;

        // Update alpha with standpat score
        alpha = std::max (alpha, stand_pat);

        // Generate and search noisy moves
        Color side_moving = pos.game_info.side_to_move;
        MoveList moves = MoveGen::generate_moves (pos);
        moves.sort(NO_MOVE);

        for (Move move: moves) {
            bool is_noisy = (CAPTURED(move) != NO_PIECE) || (FLAG(move) >= MOVE_NPROMO_FLAG);
            
            if (!is_noisy) continue;

            int gain = std::abs(material[CAPTURED(move)])  - std::abs(material[MOVED(move)] + 100);

            if (stand_pat + gain + 200 < alpha) continue;

            pos.make_move(move);

            if (pos.is_in_check(side_moving)) {
                pos.undo_move();
                continue;
            }

            int score = -qsearch(pos, depth - 1, -beta, -alpha);

            pos.undo_move();

            if (score >= beta) {
                return beta;
            }

            if (search_info.nodes % 1024 == 0 && should_stop ()) {
                return 0;
            }

            alpha = std::max(alpha, score);
        }

        return alpha;
    }

    
    // Call at root, gets both eval and best move
    std::pair<Move, int> get_best_move(Position& pos, int depth, Move pv, int alpha, int beta) {
    
        Move best_move = NO_MOVE;
        int best_score = -INF;

        Color side_moving = pos.game_info.side_to_move;

        search_info.depth = depth;

        HTEntry* ttentry = tt.probe(pos.hash);
        Move tt_move = NO_MOVE;

        if (ttentry != nullptr) {
            tt_move = ttentry->best_move;
        }
        MoveList moves = MoveGen::generate_moves(pos);
        moves.sort(pv, NO_MOVE, NO_MOVE, tt_move);

        for (Move move: moves) {
            

            pos.make_move(move);

            if (pos.is_in_check(side_moving)) {
                pos.undo_move();
                continue;
            }

            int score = -minimax(pos, depth - 1, -beta, -alpha);

            

            pos.undo_move();

            if (search_info.nodes % 1024 == 0 && should_stop ()) {
                stop();
                
            }



            if (search_info.stop.load(std::memory_order_relaxed)) return {NO_MOVE, 0};

            if (score > best_score) {
                best_score = score;
                best_move = move;
            }

            if (score > alpha)
                alpha = score;

            if (alpha >= beta)
                break;

        }

        
        if (best_move != NO_MOVE) {
            tt.store(pos.hash, depth, best_score, EXACT, best_move);
        }


        return {best_move, best_score};

    }

    void go (int depth_lim, int move_time) {

        reset_killers();

        search_info.reset();

        
        search_info.start_time = steady_clock::now();
        search_info.max_time_ms = move_time;

        bool mate_found = false;

        Move best_move = NO_MOVE;
        int eval = 0;
        int aspiration_window = 67;

        for (int depth = 1; depth <= depth_lim; ++depth) {
            std::pair<Move, int> result;
            if (depth == 1) {
                result = get_best_move(current_pos, depth, best_move, -INF, INF);
            }
            else {
                int alpha = eval - 67;
                int beta  = eval + 67;

                result = get_best_move(current_pos, depth, best_move, alpha, beta);

                int score = result.second;

                if (score <= alpha || score >= beta) {
                    // fallback to full window
                    result = get_best_move(current_pos, depth, best_move, -INF, INF);
                }
            }

            if (result.first != NO_MOVE) {
                eval = result.second;
                best_move = result.first;
            }

            

            if (std::abs(eval) > MAX_CP)
                mate_found = true;

            auto elapsed = duration_cast<milliseconds>(
                steady_clock::now() - search_info.start_time).count();

            std::vector<Move> pv = { best_move };

            UCI::info_depth(depth,
                            eval,
                            search_info.nodes,
                            elapsed,
                            pv);

            if (should_stop())
                break;
        }


        
        std::cout << "bestmove " << move_to_string (best_move) << "\n";
        
        

    }


    
}