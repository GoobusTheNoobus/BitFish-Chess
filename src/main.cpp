/**
 * main.cpp
 * 
 * Entry point
 */

#include "bitboards.h"
#include "uci.h"

using namespace std::chrono;


int main() {
    Bitboards::init();
    std::cout << "BitFish " << VERSION << " by GoobusTheNoobus\n" << std::flush;

    UCI::loop();

    

    return 0;
}


