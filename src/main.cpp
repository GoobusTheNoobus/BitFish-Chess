#include <iostream>
#include <chrono>
#include <iomanip>

#include "bitboards.h"
#include "position.h"
#include "move.h"
#include "bitfish.h"

using namespace std::chrono;


int main () {
    Bitboards::init();

    Position pos("r3kb1r/pp1n1p1p/1q2p2p/3p4/8/1N6/PPPN1PPP/R2QR1K1 b kq - 2 12");

    auto start = steady_clock::now();
    auto result = BitFish::get_best_move(pos, 5);

    auto end = steady_clock::now();
    auto duration = duration_cast<milliseconds>(end - start).count();

    std::cout << move_to_string (result.first) << " cp " << result.second <<  "\n";
    std::cout << "search took : " << duration << "ms\n";
    std::cout << "nps " << BitFish::nodes / duration * 1000 << "\n";

    std::cout << "Endgame Weight: " << BitFish::eg_weight(pos) << "\n";
    

    return 0;
}


