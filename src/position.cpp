#include "position.h"
#include "type.h"
#include "bitboards.h"
#include "move.h"

#include <string>
#include <sstream>
#include <cassert>
#include <cctype>
#include <stdexcept>



// lookups
Bitboard Position::get_bitboard(Piece piece) const {
    assert (piece != NO_PIECE && "Trying to get bitboard of NO_PIECE");
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
    string << "Castling rights: \n";
    string << ((game_info.castling & WKS_RIGHT) != 0 ? "White Kingside\n" : "") << ((game_info.castling & WQS_RIGHT) != 0 ? "White Queenside\n" : "") << ((game_info.castling & BKS_RIGHT) != 0 ? "Black Kingside\n" : "") << ((game_info.castling & BQS_RIGHT) != 0 ? "Black Queenside\n" : "") << "\n\n";   
    string << "En Passant Square: " << (game_info.ep_square == NO_SQUARE ? "-" : square_to_str(game_info.ep_square)) << "\n";
    
    return string.str();
}

// editing function
void Position::update_occupancies() {
    board.color_bitboards[WHITE] = get_bitboard(W_PAWN) | get_bitboard(W_KNIGHT) | get_bitboard(W_BISHOP) | get_bitboard(W_ROOK) | get_bitboard(W_QUEEN) | get_bitboard(W_KING);
    board.color_bitboards[BLACK] = get_bitboard(B_PAWN) | get_bitboard(B_KNIGHT) | get_bitboard(B_BISHOP) | get_bitboard(B_ROOK) | get_bitboard(B_QUEEN) | get_bitboard(B_KING);

    board.occupancy = board.color_bitboards[WHITE] | board.color_bitboards[BLACK];
}

void Position::set_square(Square square, Piece piece) {

    if (piece == NO_PIECE) clear_square(square);

    // Mailbox
    board.mailbox[square] = piece;

    // Bitboard
    board.piece_bitboards[piece] |= 1ULL << square;

    update_occupancies();
}

void Position::clear_square (Square square) {

    if (piece_at(square) == NO_PIECE) return;
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

            Square sq = Square(rank << 3 | file);
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
        game_info.ep_square = Square((rank_char - '1') << 3 | (file_char - 'a'));
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


bool Position::is_in_check (Color color) const {
    int king_pos = __builtin_ctzll (get_bitboard(make_piece(KING, color)));
    return is_square_attacked(Square(king_pos), opposite(color));
}



bool Position::can_castle_ks () const {
    if (is_in_check(game_info.side_to_move)) return false;

    if (game_info.side_to_move == WHITE) {
        // rights
        if ((game_info.castling & WKS_RIGHT) == 0)
            return false;

        // between squares
        if ((WKS_CASTLE_BETWEEN_SQU & board.occupancy) != 0ULL)
            return false;
        
        // king and rook
        if (!(piece_at(E1) == W_KING && piece_at(H1) == W_ROOK))
            return false;
        
        if (is_square_attacked(G1, BLACK) || is_square_attacked(F1, BLACK)) 
            return false;

        return true;
    } else {
        // rights
        if ((game_info.castling & BKS_RIGHT) == 0)
            return false;

        // between squares
        if ((BKS_CASTLE_BETWEEN_SQU & board.occupancy) != 0ULL)
            return false;
        
        // king and rook
        if (!(piece_at(E8) == B_KING && piece_at(H8) == B_ROOK))
            return false;
        
        if (is_square_attacked(G8, WHITE) || is_square_attacked(F8, WHITE)) 
            return false;

        return true;
    }

    
    
}

bool Position::can_castle_qs () const {
    if (is_in_check(game_info.side_to_move)) return false;

    if (game_info.side_to_move == WHITE) {
        // rights
        if ((game_info.castling & WQS_RIGHT) == 0)
            return false;

        // between squares
        if ((WQS_CASTLE_BETWEEN_SQU & board.occupancy) != 0ULL)
            return false;
        
        // king and rook
        if (!(piece_at(E1) == W_KING && piece_at(A1) == W_ROOK))
            return false;
        
        if (is_square_attacked(D1, BLACK) || is_square_attacked(C1, BLACK)) 
            return false;

        return true;
    } else {
        // rights
        if ((game_info.castling & BQS_RIGHT) == 0)
            return false;

        // between squares
        if ((BQS_CASTLE_BETWEEN_SQU & board.occupancy) != 0ULL)
            return false;
        
        // king and rook
        if (!(piece_at(E8) == B_KING && piece_at(A8) == B_ROOK))
            return false;
        
        if (is_square_attacked(D8, WHITE) || is_square_attacked(C8, WHITE)) 
            return false;

        return true;
    }

    
    
}

// assumes that move is legal: if not it does some funky stuff
void Position::make_move(Move move) {
    const int flag = FLAG(move);
    const Square to = Square(TO(move));
    const Square from = Square(FROM(move));
    
    const Piece moved_piece = Piece(MOVED(move));
    const Piece captured = Piece(CAPTURED(move));
    const Color us = game_info.side_to_move;
    const Color them = opposite(us);
    
    // store in stacks
    
    move_stack.push_back(move);
    undo_stack.push_back(PACK_GI(game_info.rule_50_clock, game_info.ep_square, game_info.castling));
    
    clear_square(from);
    
    
    switch (flag) {
        case MOVE_CASTLING_FLAG: {
            
            const Square rook_from = (to == G1 || to == G8) ? 
                Square(to + 1) :  // H file rook
                Square(to - 2);   // A file rook
            const Square rook_to = (to == G1 || to == G8) ? 
                Square(to - 1) :  // F file
                Square(to + 1);   // D file
            
            // move rookie
            Piece rook = piece_at(rook_from);
            assert(rook != NO_PIECE);
            clear_square(rook_from);
            set_square(rook_to, rook);
            
            // move chicken
            set_square(to, moved_piece);

            // this is the way!!!
            
            // update castling rights
            if (us == WHITE) {
                game_info.castling &= ~(WKS_RIGHT | WQS_RIGHT);
            } else {
                game_info.castling &= ~(BKS_RIGHT | BQS_RIGHT);
            }
            break;
        }
        
        case MOVE_ENPASSANT_FLAG: {
            const Square ep_capture_sq = Square(to + (us == WHITE ? -8 : 8));
            clear_square(ep_capture_sq);
            set_square(to, moved_piece);
            break;
        }
        
        case MOVE_DOUBLE_PUSH_FLAG: {
            set_square(to, moved_piece);
            game_info.ep_square = Square(to + (us == WHITE ? -8 : 8));
            break;
        }
        
        case MOVE_NPROMO_FLAG:
        case MOVE_BPROMO_FLAG:
        case MOVE_RPROMO_FLAG:
        case MOVE_QPROMO_FLAG: {
            // promo map
            static const PieceType promo_pieces[] = {
                KNIGHT, BISHOP, ROOK, QUEEN
            };
            const PieceType promo_type = promo_pieces[flag - MOVE_NPROMO_FLAG];
            const Piece promoted_piece = make_piece(promo_type, us);
            
            if (captured != NO_PIECE) {
                clear_square(to);
            }
            set_square(to, promoted_piece);
            break;
        }
        
        // normie move
        default: {  
            if (captured != NO_PIECE) {
                clear_square(to);
            }
            set_square(to, moved_piece);
            game_info.ep_square = NO_SQUARE;
            break;
        }
    }
    
    
    static const uint8_t castling_mask[64] = {
    
        WQS_RIGHT, 0, 0, 0, 0, 0, 0, WKS_RIGHT,    
        0, 0, 0, 0, 0, 0, 0, 0,                    
        0, 0, 0, 0, 0, 0, 0, 0,                    
        0, 0, 0, 0, 0, 0, 0, 0,                    
        0, 0, 0, 0, 0, 0, 0, 0,                    
        0, 0, 0, 0, 0, 0, 0, 0,                    
        0, 0, 0, 0, 0, 0, 0, 0,                    
        BQS_RIGHT, 0, 0, 0, 0, 0, 0, BKS_RIGHT     
    };
    
    if (castling_mask[from] && type_of(moved_piece) == ROOK) {
        game_info.castling &= ~castling_mask[from];
    }
    if (castling_mask[to] && captured != NO_PIECE && type_of(captured) == ROOK) {
        game_info.castling &= ~castling_mask[to];
    }
    
    // king moved
    if (type_of(moved_piece) == KING) {
        const uint8_t king_mask = us == WHITE ? (WKS_RIGHT | WQS_RIGHT) : (BKS_RIGHT | BQS_RIGHT);
        game_info.castling &= ~king_mask;
    }
    
    // update 50 rules
    const bool is_pawn_move = type_of(moved_piece) == PAWN;
    const bool is_capture = captured != NO_PIECE;
    
    if (is_pawn_move || is_capture) {
        game_info.rule_50_clock = 0;
    } else {
        game_info.rule_50_clock++;
    }
    
    // reset en croissant 
    if (flag != MOVE_DOUBLE_PUSH_FLAG) {
        game_info.ep_square = NO_SQUARE;
    }
    
    
    game_info.side_to_move = them;
}

void Position::undo_move () {
    
    game_info.side_to_move = opposite(game_info.side_to_move);
    Move move = move_stack.pop();

    

    uint32_t prev_gi = undo_stack.pop();

    
    game_info.castling = CASTLING(prev_gi);
    game_info.ep_square = Square(EP(prev_gi));
    game_info.rule_50_clock = RULE_50(prev_gi);

    const int flag = FLAG(move);
    const Square to = Square(TO(move));
    const Square from = Square(FROM(move));
    
    const Piece moved_piece = Piece(MOVED(move));  
    const Piece captured_piece = Piece(CAPTURED(move));
    const Color us = game_info.side_to_move;  
    const Color them = opposite(us);


    
    // move types
    switch (flag) {
        case MOVE_CASTLING_FLAG: {
            
            const Square rook_from = (to == G1 || to == G8) ? 
                Square(to + 1) :  
                Square(to - 2);  
            const Square rook_to = (to == G1 || to == G8) ? 
                Square(to - 1) :  
                Square(to + 1);   
            
            // move chicken
            clear_square(to);
            set_square(from, moved_piece);
            
            // move rookie
            Piece rook = piece_at(rook_to);

            assert(rook != NO_PIECE);
            clear_square(rook_to);
            set_square(rook_from, rook);
            break;

            // THIS IS DA WAYYYYYY 
        }
        
        case MOVE_ENPASSANT_FLAG: {
            // move pawn
            clear_square(to);
            set_square(from, moved_piece);
            
            // restore captured
            const Square ep_capture_sq = Square(to + (us == WHITE ? -8 : 8));
            const Piece captured_pawn = make_piece(PAWN, them);
            set_square(ep_capture_sq, captured_pawn);

            
            break;
        }
        
        case MOVE_DOUBLE_PUSH_FLAG: {
            // basically just normal move
            clear_square(to);
            set_square(from, moved_piece);
            break;
        }
        
        case MOVE_NPROMO_FLAG:
        case MOVE_BPROMO_FLAG:
        case MOVE_RPROMO_FLAG:
        case MOVE_QPROMO_FLAG: {
            // remove promo
            clear_square(to);
            
            // retore original pawn
            const Piece pawn = Piece(MOVED(move));
            set_square(from, pawn);
            
            if (captured_piece != NO_PIECE) {
                set_square(to, Piece(CAPTURED(move)));
            };
            break;
        }
        
        
        default: {
            
            clear_square(to);

            
            set_square(from, moved_piece);
            
            // captured
            

            if (captured_piece != NO_PIECE) {
                
                set_square(to, Piece(captured_piece));
            }
            
            break;
        }
    }
    

}




