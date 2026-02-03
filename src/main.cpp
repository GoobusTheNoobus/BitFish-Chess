#include <iostream>
#include <chrono>
#include <iomanip>

#include "bitboards.h"
#include "position.h"
#include "move.h"
#include "bitfish.h"



int main () {
    Bitboards::init();

    Position pos("rnbqkbnr/pppp1ppp/8/4p3/4P3/8/PPPP1PPP/RNBQKBNR w KQkq e6 0 2");

    BitFish::Engine e;


    std::cout << move_to_string (e.searcher.get_best_move(pos, 6).first);

    
    

    return 0;
}


