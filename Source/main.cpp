#include <iostream>

#include "bitboards.h"
#include "position.h"
#include "move.h"
#include "movegen.h"

int main () {
    Bitboards::init();

    Position pos("rnbqkbnr/pppp1ppp/8/4p3/4P3/8/PPPP1PPP/RNBQKBNR w KQkq e6 0 2");
    Encode::print_move_list(MoveGen::generate_moves(PSEUDO_LEGAL, pos));

    return 0;
}