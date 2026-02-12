// ------------------------------------ BITFISH ---------------------------------------

#pragma once
#include "position.h"
#include <string>
#include <string_view>
#include "movegen.h"
#include <chrono>
#include <cstring>
#include <vector>

using namespace std::chrono;

enum HTFlag {
    EXACT,
    AT_LEAST,
    AT_MOST
};

struct HTEntry {
    Key hash;
    int depth;
    int score;
    HTFlag flag;
    Move best_move;
};

class HashTable {
    std::vector <HTEntry> table;

    public: 
        HashTable (size_t mb = 64);
        void clear();
        HTEntry* probe (Key hash);
        void store (Key hash, int depth, int score, HTFlag flag, Move best_move);
    private:
        size_t index (Key hash) const;
};


namespace BitFish {

    constexpr int MAX_DEPTH = 16;
    constexpr int MAX_QDEPTH = 20;
    
    extern int current_depth;
    extern bool stop_flag;
    extern steady_clock::time_point start_time;
    extern int max_time;
    extern uint64_t nodes;
    extern Position current_pos;

    extern HashTable tt;

    extern std::array<std::array<Move, 2>, MAX_DEPTH> killers;

    inline void reset_killers () {
        std::memset(killers.begin(), NO_MOVE, sizeof(killers));
    }

    inline void store_killer (Move move, int ply) {
        if (killers[ply][0] == move) return;
        if (killers[ply][1] == move) return;

        // shift
        killers[ply][1] = killers[ply][0];
        killers[ply][0] = move;
    }
    

    void go (int depth_lim = MAX_DEPTH, int move_time = 100000);
    Move iterative_deepen (int move_time);
    void stop ();
    bool should_stop () ;
    int minimax (Position& pos, int depth, int alpha, int beta, bool null_ok=true);
    int qsearch (Position& pos, int depth, int alpha, int beta);
    void position (std::string_view fen);

    std::pair<Move, int> get_best_move (Position& pos, int max_depth, Move pv);


    
    constexpr std::array<int, BOARD_SIZE> pawn_table_mg = {
        0,  0,  0,  0,  0,  0,  0,  0,
        5, 10, 10,-30,-30, 10, 10,  5,
        5,  0,-10,  0,  0,-10,  0,  5,
        0,  0, 10, 30, 30, 10,  0,  0,
        5,  5, 10, 30, 30, 10,  5,  5,
        10, 10, 20, 30, 30, 20, 10, 10,
        50, 50, 55, 55, 55, 55, 50, 50,
        0,  0,  0,  0,  0,  0,  0,  0
    };

    constexpr std::array<int, BOARD_SIZE> pawn_table_eg = {
        0,  0,  0,   0,   0,  0,  0, 0,
        5, 10, 10,  10,  10, 10, 10, 5,
        10, 15, 20,  25,  25, 20, 15, 10,
        20, 30, 35,  40,  40, 35, 30, 20,
        30, 45, 55,  60,  60, 55, 45, 30,
        50, 60, 75,  90,  90, 75, 60, 50,
        75, 80, 90, 120, 120, 90, 80, 75,
        0, 0,   0,   0,   0,  0,  0, 0
    };

    constexpr std::array<int, BOARD_SIZE> knight_table = {
        -30,-30,-10,  0,  0,-10,-30,-30,
        -20,-10,  0,  5,  5,  0,-10,-20,
        -10,  0, 15, 10, 10, 20,  0,-10,
        -10,  5, 10, 25, 25, 10,  5,-10,
        -10,  5, 10, 25, 25, 10,  5,-10,
        -10,  0, 15, 10, 10, 15,  0,-10,
        -20,-10,  0,  5,  5,  0,-10,-20,
        -30,-20,-10,  0,  0,-10,-20,-30
    };

    constexpr std::array<int, BOARD_SIZE> bishop_table = {
        -30,-20,-10,  0,  0,-10,-20,-30,
        -20,-10,  0,  5,  5,  0,-10,-20,
        -10,  0,  5, 10, 10,  5,  0,-10,
        -10,  5, 10, 25, 25, 10,  5,-10,
        -10,  5, 10, 25, 25, 10,  5,-10,
        -10,  0,  5, 10, 10,  5,  0,-10,
        -20,-10,  0,  5,  5,  0,-10,-20,
        -30,-20,-10,  0,  0,-10,-20,-30
    };

    constexpr std::array<int, BOARD_SIZE> rook_table = {
        0,  0,  5, 10, 10,  5,  0,  0,
        0,  0,  5, 10, 10,  5,  0,  0,
        0,  0,  5, 10, 10,  5,  0,  0,
        0,  0,  5, 10, 10,  5,  0,  0,
        0,  0,  5, 10, 10,  5,  0,  0,
        0,  0,  5, 10, 10,  5,  0,  0,
        5,  5, 10, 15, 15, 10,  5,  5,
        0,  0,  5, 10, 10,  5,  0,  0
    };

    constexpr std::array<int, BOARD_SIZE> queen_table = {
        -20,-10,-10, 0, 0,-10,-10,-20,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -10,  0,  -10,  5,  5,  -10,  0,-10,
        -5,  0,  5, 10, 10,  5,  0, -5,
        0,  0,  5, 10, 10,  5,  0, -5,
        -10,  5,  5,  5,  5,  5,  0,-10,
        -10,  0,  5,  0,  0,  0,  0,-10,
        -20,-10,-10, 0 , -5,-10,-10,-20
    };

    constexpr std::array<int, BOARD_SIZE> king_table_mg = {
        -20,   10,    0,  -20,  -20,    0,  10, -20,
        -30,  -40,  -40,  -60,  -60,  -40, -40, -30,
        -50,  -60,  -75,  -90,  -90,  -75, -60, -50,
        -50,  -60,  -90,  -90,  -90,  -90, -60, -50,
        -60,  -75,  -90, -120, -120,  -90, -75, -60,
        -70,  -75, -100, -150, -150, -100, -75, -70,
        -70,  -75, -100, -150, -150, -100, -75, -70,
        -80, -100, -120, -150, -150, -120, -100, -80
    };

    constexpr std::array<int, BOARD_SIZE> king_table_eg = {
        -50, -30,  -20, -10, -10, -20, -30, -50,
        -30, -10,    0,  10,  10,   0, -10, -30,
        -20,   0,   20,  30,  30,  20,   0, -20,
        -10,   0,   20,  50,  50,  20,   0, -10,
        -10,   0,   20,  50,  50,  20,   0, -10,
          0,  10,   20,  20,  20,  20,  10,   0,
          0,   0,   10,  10,  10,  10,   0,   0,
        -10,  -5,    0,   0,   0,   0,  -5, -10
    };

    int evaluate (Position& pos);
    float eg_weight (Position& pos);
    

    

    

} 
