// ------------------------------------ BITFISH ---------------------------------------

#pragma once
#include <string_view>
#include <array>

#include "type.h"

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

const std::string VERSION = "v0.4.0";



