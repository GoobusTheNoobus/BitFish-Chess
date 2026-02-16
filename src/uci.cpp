/**
 * uci.cpp
 * 
 * Universal Chess Interface Implementation
 * Should work fine, but bugs expected
 */

#include "uci.h"
#include "bitfish.h"
#include "movegen.h"
#include "constants.h"

#include <sstream>
#include <string>
#include <thread>
#include <atomic>

namespace {
    std::thread search_thread;
    std::atomic<bool> is_searching{false};
    
    Move parse_move (const Position& pos, const std::string& str) {
        MoveList list = MoveGen::generate_moves(pos);

        for (Move move: list) {
            if (move_to_string(move) == str) {
                return move;
            }
        }

        throw std::invalid_argument("Cannot parse string " + str);
    }
    
    void cleanup_search_thread() {
        if (search_thread.joinable()) {
            search_thread.join();
        }
        is_searching = false;
    }
}

void UCI::info_depth (int depth, int eval, uint64_t nodes, uint64_t elapsed, const std::vector<Move>& pv) {
    std::string score_str;

    if (std::abs(eval) > MAX_CP){
        int mate_distance = (MATE_EVAL - std::abs(eval) + 1) / 2;

        if (eval < 0) {
            mate_distance = -mate_distance;
        }

        score_str = "mate " + std::to_string(mate_distance);

    }
    else score_str = "cp " + std::to_string(eval);

    std::cout << "info depth " << depth << " score " << score_str << " nodes " << (nodes) << " nps " << (nodes * 1000 / std::max<uint64_t>(elapsed, 1)) << " time " << elapsed << " pv ";
            
    for (Move move: pv) {
        std::cout << move_to_string(move) << " ";
    }

    std::cout << "\n" << std::flush; 
}

void UCI::info_string (const std::string& message) {
    std::cout << "info string " << message << std::endl << std::flush;
}

void UCI::uci () {
    std::cout << "id name BitFish " << VERSION << std::endl;
    std::cout << "id author GoobusTheNoobus" << std::endl;
    std::cout << "uciok" << std::endl << std::flush;
}

void UCI::isready () {
    std::cout << "readyok" << std::endl << std::flush;
}

void UCI::ucinewgame () {
    // Wait for any ongoing search to finish
    cleanup_search_thread();
    
    BitFish::tt.clear();
    BitFish::reset_killers();
}

void UCI::parse_position (const std::string& command) {
    std::istringstream iss (command);

    std::string token;

    // position
    iss >> token;

    // startpos/fen
    iss >> token;

    if (token == "startpos") {
        BitFish::position(STARTING_POS_FEN);

        iss >> token;

        if (token == "moves") {
            while (iss >> token) {
                BitFish::current_pos.make_move(parse_move(BitFish::current_pos, token));
            }
        } 
    } else if (token == "fen") {
        std::string fen;

        while ((iss >> token) && token != "moves") {
            fen += token + " ";
        }

        try {
            BitFish::position(fen);
        } catch (std::exception e) {
            info_string("Error parsing fen");
        }

        if (token == "moves") {
            while (iss >> token) {
                BitFish::current_pos.make_move(parse_move(BitFish::current_pos, token));
            }
        }
    }
}

void UCI::parse_go(const std::string& command) {
    // Stop any ongoing search first
    if (is_searching) {
        BitFish::stop();
        
    }
    cleanup_search_thread();
    
    std::istringstream iss (command);
    std::string token;

    int depth = MAX_DEPTH;
    int movetime = 0;

    int wtime = 0;
    int btime = 0;

    int winc = 0;
    int binc = 0;
    
    // skip go
    iss >> token;

    while (iss >> token) {
        if (token == "depth") {
            iss >> depth;
        } else if (token == "movetime") {
            iss >> movetime;
        } else if (token == "wtime") {
            iss >> wtime;
        } else if (token == "btime") {
            iss >> btime;
        } else if (token == "winc") {
            iss >> winc;
        } else if (token == "binc") {
            iss >> binc;
        }
    }

    int time_limit = 0;
    
    if (movetime > 0) {
        time_limit = movetime;
    } else if (wtime > 0 || btime > 0) {
        
        int our_time = (BitFish::current_pos.game_info.side_to_move == WHITE) ? wtime + winc  : btime + binc ;
        
        // use 1/50 of remaining time
        if (our_time > 100) {
            time_limit = std::min(our_time - 100, our_time / 40);
            time_limit = std::max(100, time_limit);  
        }
    }

    // Launch search in separate thread
    is_searching = true;
    BitFish::search_info.stop.store(false, std::memory_order_relaxed);
    
    search_thread = std::thread([depth, time_limit]() {
        BitFish::go(depth, time_limit);
        is_searching = false;
    });
}

void UCI::stop() {
    if (is_searching) {
        BitFish::stop();
        cleanup_search_thread();
    }
}

void UCI::d () {
    std::cout << BitFish::current_pos.to_string() << "\n" << std::flush;
}

void UCI::eval () {
    std::cout << BitFish::current_pos.to_string() << "\n";
    std::cout << "Current evaluation: " << BitFish::evaluate(BitFish::current_pos) << std::flush;
}

void UCI::loop () {
    while (true) {
        std::string string;
        std::getline(std::cin, string);

        std::istringstream iss (string);
        std::string command;

        iss >> command;

        if (command == "go") {
            parse_go (string);
        } else if (command == "stop") {
            stop();
        } else if (command == "position") {
            parse_position(string);
        } else if (command == "uci") {
            uci ();
        } else if (command == "isready") {
            isready();
        } else if (command == "ucinewgame") {
            ucinewgame();
        } else if (command == "d") {
            d();
        } else if (command == "eval") {
            eval();
        } else if (command == "quit") {
            // Clean up before exiting
            if (is_searching) {
                stop();
            }
            cleanup_search_thread();
            break;
        } 
        else {
            info_string("invalid command");
        }
    }
}