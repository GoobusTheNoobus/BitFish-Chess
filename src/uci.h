/**
 * uci.h
 * 
 * Universal Chess Interface support so it can play on lichess & stuff
 */

#pragma once
#include <iostream>
#include <vector>
#include <string>
#include "move.h"



namespace UCI {
    // logging
    void info_depth (int depth, int eval, uint64_t nodes, uint64_t elapsed, const std::vector<Move>& pv);
    void info_string (const std::string& message);
    
    // commands
    void parse_position(const std::string& command);
    void parse_go(const std::string& command);
    void uci();
    void isready();
    void ucinewgame();

    // as a rip off of stockfish, i must include these stockfish exclusive command
    void d(); 
    void eval();

    void loop();
}