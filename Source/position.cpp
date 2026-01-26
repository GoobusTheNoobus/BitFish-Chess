#include "position.h"
#include "type.h"
#include "bitboards.h"

#include <string>
#include <sstream>
#include <cassert>
#include <cctype>
#include <stdexcept>


// lookups
Bitboard Position::get_bitboard(Piece piece) const {
    return board.piece_bitboards[piece];
}

Piece Position::piece_at(Square square) const {
    return board.mailbox[square];
}

// position representation
std::string Position::to_string() const {
    std::ostringstream string;

    
        string << "\n  +-----------------+\n";
        for (int rank = 7; rank >= 0; rank--) {
            string << rank + 1 << " | ";
            for (int file = 0; file < 8; file++) {
                int square = rank * 8 + file;
                string << PIECE_TO_CHAR[piece_at(Square(rank << 3 | file))] << " ";
            }
            string << "|\n";
        }
        string << "  +-----------------+\n";
        string << "    a b c d e f g h\n\n";


    string << "\nSide to move: " << (game_info.side_to_move == WHITE ? "White": "Black");

    return string.str();
}

// editing function
void Position::update_occupancies() {
    board.color_bitboards[WHITE] = get_bitboard(W_PAWN) | get_bitboard(W_KNIGHT) | get_bitboard(W_BISHOP) | get_bitboard(W_ROOK) | get_bitboard(W_QUEEN) | get_bitboard(W_KING);
    board.color_bitboards[BLACK] = get_bitboard(B_PAWN) | get_bitboard(B_KNIGHT) | get_bitboard(B_BISHOP) | get_bitboard(B_ROOK) | get_bitboard(B_QUEEN) | get_bitboard(B_KING);

    board.occupancy = board.color_bitboards[WHITE] | board.color_bitboards[BLACK];
}

void Position::set_square(Square square, Piece piece) {
    // Mailbox
    board.mailbox[square] = piece;

    // Bitboard
    board.piece_bitboards[piece] |= 1ULL << square;

    update_occupancies();
}

void Position::clear_square (Square square) {

    board.piece_bitboards[board.mailbox[square]] &= ~(1ULL << square);

    board.mailbox[square] = NO_PIECE;

    
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

void Position::parse_fen(std::string_view fen) {
    clear_board(); // start from empty board

    int rank = 7;
    int file = 0;

    size_t i = 0;
    while (i < fen.size() && fen[i] != ' ') {
        char c = fen[i];

        if (c == '/') {         // move to next rank
            assert(file == 8);  // sanity check
            file = 0;
            --rank;
        }
        else if (std::isdigit(c)) {  // empty squares
            file += c - '0';
        }
        else {  // a piece
            assert(file < 8 && rank >= 0);

            Piece p;
            switch (c) {
                case 'P': p = W_PAWN; break;
                case 'N': p = W_KNIGHT; break;
                case 'B': p = W_BISHOP; break;
                case 'R': p = W_ROOK; break;
                case 'Q': p = W_QUEEN; break;
                case 'K': p = W_KING; break;

                case 'p': p = B_PAWN; break;
                case 'n': p = B_KNIGHT; break;
                case 'b': p = B_BISHOP; break;
                case 'r': p = B_ROOK; break;
                case 'q': p = B_QUEEN; break;
                case 'k': p = B_KING; break;

                default: throw std::invalid_argument("Invalid Piece in Board field");
            }

            Square sq = parse_square(rank, file);
            set_square(sq, p);
            ++file;
        }
        ++i;
    }

    assert(rank == 0 && file == 8); // sanity check all squares processed

    // --- Parse remaining fields ---
    fen.remove_prefix(i + 1); 

    
    game_info.side_to_move = (fen[0] == 'w') ? WHITE : BLACK;
    fen.remove_prefix(2); 

    // castling rights
    game_info.castling = 0;
    size_t j = 0;
    while (j < fen.size() && fen[j] != ' ') {
        switch(fen[j]) {
            case 'K': game_info.castling |= 1; break;
            case 'Q': game_info.castling |= 2; break;
            case 'k': game_info.castling |= 4; break;
            case 'q': game_info.castling |= 8; break;
        }
        ++j;
    }
    fen.remove_prefix(j + 1);

    // en-croissant square
    if (fen[0] == '-') {
        game_info.ep_square = NO_SQUARE;
        fen.remove_prefix(2);
    } else {
        char file_char = fen[0];
        char rank_char = fen[1];
        game_info.ep_square = parse_square((rank_char - '1'), (file_char - 'a'));
        fen.remove_prefix(3);
    }

    // 50-move clock
    size_t space = fen.find(' ');
    game_info.rule_50_clock = std::stoi(std::string(fen.substr(0, space)));
    fen.remove_prefix(space + 1);

    
    update_occupancies();
}

bool Position::is_square_attacked (Square square, Color color) const {
    Bitboard pawns = color == WHITE ? get_bitboard(W_PAWN): get_bitboard(B_PAWN);
    Bitboard knights = color == WHITE ? get_bitboard(W_KNIGHT): get_bitboard(B_KNIGHT);
    Bitboard bishops = color == WHITE ? get_bitboard(W_BISHOP): get_bitboard(B_BISHOP);
    Bitboard rooks = color == WHITE ? get_bitboard(W_ROOK): get_bitboard(B_ROOK);
    Bitboard queens = color == WHITE ? get_bitboard(W_QUEEN): get_bitboard(B_QUEEN);
    Bitboard kings = color == WHITE ? get_bitboard(W_KING): get_bitboard(B_KING);

    // pawns first
    if ((Bitboards::get_pawn_attacks(square, opposite(color)) & pawns) != 0ULL) 
        return true;
        // if color == WHITE, the black pawn attack of that square is the square(s) that if a white pawn stands on would attack that square. 
    
    // knights
    if ((Bitboards::get_knight_attacks(square) & knights) != 0ULL)
        return true;

    Bitboard rook_attacks = Bitboards::get_rook_attacks(square, board.occupancy);
    Bitboard bishop_attacks = Bitboards::get_bishop_attacks(square, board.occupancy);

    // rooks & queens
    if (((rook_attacks & rooks) != 0ULL) || ((rook_attacks & queens) != 0ULL)) 
        return true;

    // bishops & queens
    if (((bishop_attacks & bishops) != 0ULL) || ((bishop_attacks & queens) != 0ULL))
        return true;

    // kings
    if ((Bitboards::get_king_attacks(square) & kings) != 0ULL)
        return true;

    
    return false;

    
}

bool Position::is_in_check () const {
    int king_pos = __builtin_ctzll (game_info.side_to_move == WHITE ? get_bitboard(W_KING): get_bitboard(B_KING));
    return is_square_attacked(Square(king_pos), opposite(game_info.side_to_move));
}
