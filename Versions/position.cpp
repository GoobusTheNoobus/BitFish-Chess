#include "position.h"
#include "type.h"

Bitboard Position::get_bitboard(Piece piece) const {
    return this->board.piece_bitboards[piece];
}

void Position::update_occupancies() {
    board.color_bitboards[WHITE] = get_bitboard(W_PAWN) | get_bitboard(W_KNIGHT) | get_bitboard(W_BISHOP) | get_bitboard(W_ROOK) | get_bitboard(W_QUEEN) | get_bitboard(W_KING);
    board.color_bitboards[BLACK] = get_bitboard(B_PAWN) | get_bitboard(B_KNIGHT) | get_bitboard(B_BISHOP) | get_bitboard(B_ROOK) | get_bitboard(B_QUEEN) | get_bitboard(B_KING);

    board.occupancy = board.color_bitboards[WHITE] | board.color_bitboards[BLACK];
}

void Position::clear_board () {
    board.mailbox.fill(NO_PIECE);

    board.piece_bitboards.fill(0ULL);
    board.color_bitboards.fill(0ULL);
    board.occupancy = 0ULL;

    
}

void Position::set_start_pos () {
    
    parse_fen(STARTING_POS_FEN);
    
}

void Position::parse_fen(const std::string& fen) {
    clear_board();

    // TO-DO PARSE FEN


    update_occupancies();
}