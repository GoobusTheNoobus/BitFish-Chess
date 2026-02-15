/**
 * constants.h
 * 
 * Contains all the global constants 
 */

#pragma once
#include <string_view>
#include <array>

#include "type.h"

using namespace std::chrono;

constexpr int BOARD_SIZE = 64;
constexpr int PIECE_NUM  = 12;
constexpr int COLOR_NUM = 2;

constexpr int BISHOP_BLOCKER_COMBINATIONS = 1 << 9;
constexpr int ROOK_BLOCKER_COMBINATIONS = 1 << 12;

constexpr uint32_t NO_MOVE = -1;

const std::string STARTING_POS_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

constexpr std::array<int, PIECE_NUM + 1> material = {
    100, 300, 320, 500, 900, 0, -100, -300, -320, -500, -900, 0, 0
};

constexpr std::array<int, 4> promo_flag_bonus = {
    200, 220, 400, 800
};

const std::array<char, PIECE_NUM + 1> PIECE_TO_CHAR = {
    'P',
    'N',
    'B',
    'R',
    'Q',
    'K',
    'p',
    'n',
    'b',
    'r',
    'q',
    'k',
    '.'
};

constexpr int MOVE_CASTLING_FLAG = 0b0001;
constexpr int MOVE_ENPASSANT_FLAG = 0b0010;
constexpr int MOVE_DOUBLE_PUSH_FLAG = 0b0011;
constexpr int MOVE_NPROMO_FLAG = 0b0100;
constexpr int MOVE_BPROMO_FLAG = 0b0101;
constexpr int MOVE_RPROMO_FLAG = 0b0110;
constexpr int MOVE_QPROMO_FLAG = 0b0111;

constexpr uint8_t WKS_RIGHT = 0b0001;
constexpr uint8_t WQS_RIGHT = 0b0010;
constexpr uint8_t BKS_RIGHT = 0b0100;
constexpr uint8_t BQS_RIGHT = 0b1000;

constexpr Bitboard WKS_CASTLE_BETWEEN_SQU = (1ULL << F1) | (1ULL << G1);
constexpr Bitboard WQS_CASTLE_BETWEEN_SQU = (1ULL << D1) | (1ULL << C1) | (1ULL << B1);

constexpr Bitboard BKS_CASTLE_BETWEEN_SQU = (1ULL << F8) | (1ULL << G8);
constexpr Bitboard BQS_CASTLE_BETWEEN_SQU = (1ULL << D8) | (1ULL << C8) | (1ULL << B8);

constexpr int MATE_EVAL = 30000;
constexpr int INF = 30001;
constexpr int MAX_CP = 10000;

const std::string VERSION = "v0.5.1";

constexpr int KINGSIDE_CASTLING_BONUS = 8;
constexpr int QUEENSIDE_CASTLING_BONUS = 6;

constexpr int KNIGHT_MOB_BONUS = 4;
constexpr int BISHOP_MOB_BONUS = 3;
constexpr int ROOK_MOB_BONUS = 2;
constexpr int QUEEN_MOB_BONUS = 2;

constexpr int MAX_DEPTH = 16;
constexpr int MAX_QDEPTH = 20;

constexpr int BISHOP_PAIR_BONUS = 30;

constexpr int KING_SQUARE_CONTROLLED_BONUS = 5;

namespace EvalTables {
    constexpr std::array<int, BOARD_SIZE> pawn_table_mg = {
        0,  0,  0,  0,  0,  0,  0,  0,
        5, 10, 10,-30,-30, 10, 10,  5,
        5,  0,-10,  0,  0,-10,  0,  5,
        0,  0, 10, 30, 30, 10,  0,  0,
        5,  5, 10, 30, 30, 10,  5,  5,
        10, 10, 20, 30, 30, 20, 10, 10,
        50, 50, 55, 55, 55, 55, 50, 50,
        0,  0,  0,  0,  0,  0,  0,  0
    };

    constexpr std::array<int, BOARD_SIZE> pawn_table_eg = {
        0,  0,  0,   0,   0,  0,  0, 0,
        0,  0,  0,   0,   0,  0,  0, 0,
        10, 15, 20,  25,  25, 20, 15, 10,
        20, 30, 35,  40,  40, 35, 30, 20,
        30, 45, 55,  60,  60, 55, 45, 30,
        50, 60, 75,  90,  90, 75, 60, 50,
        75, 80, 90, 120, 120, 90, 80, 75,
        0, 0,   0,   0,   0,  0,  0, 0
    };

    constexpr std::array<int, BOARD_SIZE> knight_table = {
        -30,-30,-10,  0,  0,-10,-30,-30,
        -20,-10,  0,  5,  5,  0,-10,-20,
        -10,  0, 15, 10, 10, 20,  0,-10,
        -10,  5, 10, 25, 25, 10,  5,-10,
        -10,  5, 10, 25, 25, 10,  5,-10,
        -10,  0, 15, 10, 10, 15,  0,-10,
        -20,-10,  0,  5,  5,  0,-10,-20,
        -30,-20,-10,  0,  0,-10,-20,-30
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
}
    