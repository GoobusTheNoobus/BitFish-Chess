#pragma once
#include <string_view>
#include <array>

constexpr int BOARD_SIZE = 64;
constexpr int PIECE_NUM  = 12;
constexpr int COLOR_NUM = 2;

constexpr int BISHOP_BLOCKER_COMBINATIONS = 1 << 9;
constexpr int ROOK_BLOCKER_COMBINATIONS = 1 << 12;

const std::string_view STARTING_POS_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

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