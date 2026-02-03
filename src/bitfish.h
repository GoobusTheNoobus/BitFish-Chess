#pragma once
#include "position.h"
#include <string>
#include <string_view>
#include "movegen.h"

namespace BitFish {
    
    struct SearchInfo {
        int depth = 0;
        int seldepth = 0;
        uint64_t nodes = 0;
        
        int score = 0;
        int time_ms = 0;
        std::string pv;

        inline int get_nps () const {
            return static_cast <int> (nodes * 1000 / time_ms);
        }
    };

    class Searcher {
        public:
            Searcher() = default;

            inline void reset() {
                current_depth = 0;
                max_time = 0;
                stop_flag = false;
            }
            
            // doesn't return anything, prints results
            void search (Position& pos, int max_depth, int max_time_ms);

            // UCI command stop (i dont actually know how to impl this)
            void stop();

        // private:
            // internal stuff

            // technically negamax
            int minimax (Position& pos, int depth, int alpha, int beta);

            std::pair<Move, int> get_best_move (Position& pos, int max_depth);

            // can't spell
            int qsearch (Position& pos, int max_depth, int alpha, int beta);

            bool should_stop() const;

            int current_depth = 0;
            bool stop_flag = false;
            uint64_t start_time = 0;
            uint64_t end_time = 0;
            int max_time = 0;
            uint64_t nodes = 0;

    };

    namespace Eval {

        constexpr std::array<int, PIECE_NUM + 1> material = {
            100, 300, 320, 500, 900, 0, -100, -300, -320, -500, -900, 0, 0
        };
        constexpr std::array<int, BOARD_SIZE> pawn_table_mg = {
            0,  0,  0,  0,  0,  0,  0,  0,
            5, 10, 10,-15,-15, 10, 10,  5,
            5,  0,-10,  0,  0,-10,  0,  5,
            0,  0, 10, 30, 30, 10,  0,  0,
            5,  5, 10, 30, 30, 10,  5,  5,
            10, 10, 20, 30, 30, 20, 10, 10,
            50, 50, 55, 55, 55, 55, 50, 50,
            0,  0,  0,  0,  0,  0,  0,  0
        };

        constexpr std::array<int, BOARD_SIZE> pawn_table_eg = {
            0,  0,  0,   0,   0,  0,  0, 0,
            5, 10, 10,  15,  15, 10, 10, 5,
            10, 15, 20,  25,  25, 20, 15, 10,
            20, 30, 35,  40,  40, 35, 30, 20,
            30, 45, 55,  60,  60, 55, 45, 30,
            50, 60, 75,  90,  90, 75, 60, 50,
            75, 80, 90, 120, 120, 90, 80, 75,
            0, 0,   0,   0,   0,  0,  0, 0
        };

        constexpr std::array<int, BOARD_SIZE> knight_table = {
            -50,-40,-30,-30,-30,-30,-40,-50,
            -40,-20,  0,  0,  0,  0,-20,-40,
            -30,  0, 0, 15, 15, 0,  0,-30,
            -30,  5, 15, 20, 20, 15,  5,-30,
            -30,  5, 15, 20, 20, 15,  5,-30,
            -30,  0, 10, 15, 15, 10,  0,-30,
            -40,-20,  0,  0,  0,  0,-20,-40,
            -50,-40,-30,-30,-30,-30,-40,-50
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

    class Engine {
        
        
        public:
        Engine() = default;
        // default max time is 1000ms
        void go (int depth=6, int max_time=1000);

        Searcher searcher;
        Position pos;

    };

    

} 
