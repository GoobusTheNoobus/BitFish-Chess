/**
 * bitfish.h
 * 
 * Search Interface
 * Includes a transposition table and killers
 */

#pragma once

#include "uci.h"
#include "movegen.h"

#include <string>
#include <string_view>
#include <chrono>
#include <cstring>
#include <vector>


using namespace std::chrono;

namespace BitFish {

    // Global State
    extern SearchInfo search_info;
    extern Position current_pos;
    extern HashTable tt;
    extern std::array<std::array<Move, 2>, MAX_DEPTH> killers;

    // Killer functions
    void reset_killers ();
    void store_killer (Move move, int ply);

    // UCI Interface
    void position (std::string_view fen);
    void go (int depth_lim, int move_time);
    void stop ();
    bool should_stop ();

    // Evaluation Functions
    int evaluate (Position& pos);
    float eg_weight (Position& pos);

    // Search Functions
    int minimax (Position& pos, int depth, int alpha, int beta, bool null_ok=true);
    int qsearch (Position& pos, int depth, int alpha, int beta);
    std::pair<Move, int> get_best_move (Position& pos, int max_depth, Move pv);

} 
