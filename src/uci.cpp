#include "uci.h"
#include "bitfish.h"
#include "movegen.h"

namespace {
    Move parse_move (const Position& pos, const std::string& str) {
        MoveList list = MoveGen::generate_moves(pos);

        for (Move move: list) {
            if (move_to_string(move) == str) {
                return move;
            }
        }

        throw std::invalid_argument("Cannot parse string " + str);

    }
}

void UCI::info_depth (int depth, int eval, uint64_t nodes, uint64_t elapsed, const std::vector<Move>& pv) {
    std::cout << "info depth " << depth << " score cp " << (eval) << " nodes " << (nodes) << " nps " << (nodes * 1000000 / elapsed) << " time " << (elapsed / 1000) << " pv ";
            
    for (Move move: pv) {
        std::cout << move_to_string(move) << " ";
    }

    std::cout << "\n"; 
}

void UCI::info_string (const std::string& message) {
    std::cout << "info string " << message;
}

void UCI::uci () {
    std::cout << "id name BitFish " << VERSION << "\n";
    std::cout << "id author GoobusTheNoobus\n";
}

void UCI::isready () {
    std::cout << "readyok\n";
}

void UCI::ucinewgame () {
    BitFish::tt.clear();
    BitFish::reset_killers();
}

void UCI::parse_position (const std::string& fen, const std::vector<std::string>& moves) {
    BitFish::position(fen);
    
    for (const std::string& str: moves) {
        BitFish::current_pos.make_move(parse_move(BitFish::current_pos, str));
    }
}