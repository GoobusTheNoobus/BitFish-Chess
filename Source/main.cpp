#include <iostream>

#include "bitboards.h"
#include "position.h"

int main () {
    Bitboards::init();

    Position pos("r1bqkbnr/pppp1Qpp/2n5/4p3/4P3/8/PPPP1PPP/RNB1KBNR b KQkq - 0 3");
    std::cout << std::boolalpha << pos.is_square_attacked(E8, WHITE);

    return 0;
}