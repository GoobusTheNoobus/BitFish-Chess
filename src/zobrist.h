#pragma once

#include <random>
#include <array>

#include "type.h"
#include "constants.h"

struct Zobrist {
    std::array<std::array<Key, BOARD_SIZE>, PIECE_NUM> pieces;
    std::array<Key, 16> castling;
    std::array<Key, BOARD_SIZE> en_passant;
    
    Key white_to_move;
    
    Zobrist () {
        std::mt19937_64 rng(6767);

        for (int piece = 0; piece < PIECE_NUM; ++piece) {
            for (int square = 0; square < BOARD_SIZE; ++square) {
                pieces[piece][square] = rng();
            }
        }

        white_to_move = rng();

        for (int i = 0; i < 16; i++) {
            castling[i] = rng();
        }

        for (int i = 0; i < BOARD_SIZE; i++) {
            en_passant[i] = rng();
        }
    }

};

inline Zobrist zobrist;