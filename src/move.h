/**
 * move.h
 * 
 * Contains all move macros
 * Continas MoveList class
 */
#pragma once
#include "type.h"
#include "constants.h"

#include <array>
#include <vector>
#include <bitset>
#include <algorithm>

// Pack everything into a 32 bit integer
#define MOVE(from, to, moved, captured, flag) \
    (((from) & 0x3F) | (((to) & 0x3F) << 6) | \
     (((moved) & 0xF) << 12) | (((captured) & 0xF) << 16) | \
     (((flag) & 0xF) << 20))

// Macros for packing different types of moves
#define NORMAL_MOVE(from, to, moved, captured) \
    MOVE(from, to, moved, captured, 0)

#define DOUBLE_PUSH_MOVE(from, to, moved) \
    MOVE(from, to, moved, NO_PIECE, MOVE_DOUBLE_PUSH_FLAG)

#define EN_PASSANT(from, to, moved, captured) \
    MOVE(from, to, moved, captured, MOVE_ENPASSANT_FLAG)

#define CASTLING_MOVE(from, to, moved) \
    MOVE(from, to, moved, NO_PIECE, MOVE_CASTLING_FLAG)

#define PROMO_MOVE(move, promo_flag) \
    (move | (promo_flag << 20))

// Lookup Macros
#define FROM(move) \
    (move & 0x3F)

#define TO(move) \
    ((move >> 6) & 0x3F)

#define MOVED(move) \
    ((move >> 12) & 0xF)

#define CAPTURED(move) \
    ((move >> 16) & 0xF)

#define FLAG(move) \
    ((move >> 20) & 0xF)

// We also pack all essential gamestate information into a 32 bit integer for speed
// We don't need side to move since we just need to switch it to the opposite color when 
// doing and undoing a move
#define PACK_GI(rule_50, ep, castling) \
    (castling & 0b1111) | ((ep & 127) << 4) | (rule_50 << 11)

#define CASTLING(gi) \
    (gi & 0b1111)

#define RULE_50(gi) \
    (gi >> 11)

#define EP(gi) \
    ((gi >> 4) & 127)

    
// Turns a move into long algebraic notation for UCI protocols
inline std::string move_to_string (Move move) {
    const std::array<char, 4> promo_chars = {
        'n',
        'b',
        'r',
        'q'
    };
    if (FLAG(move) >= MOVE_NPROMO_FLAG) 
    return square_to_str(Square(FROM(move))) + square_to_str(Square(TO(move))) + promo_chars[FLAG(move) - MOVE_NPROMO_FLAG];
    else 
    return square_to_str(Square(FROM(move))) + square_to_str(Square(TO(move)));
}

// A MoveList container for the list with a size variable
struct MoveList {

    std::array<Move, 256> list;
    int size = 0;

    // Functions to support foreach loops
    inline Move* begin() {
        return list.data();
    }

    inline Move* end() {
        return list.data() + size;
    }

    inline const Move* begin() const {
        return list.data();
    }

    inline const Move* end() const {
        return list.data() + size;
    }

    // Names are similar to a vector, since originally it was a vector
    inline void push_back (Move move) {
        list[size++] = move;
    }

    inline Move pop () {
        size --;
        return list[size];
    }

    inline Move peek () {
        return list[size - 1];
    }

    inline void clear () {
        size = 0;
    }

    inline Move operator[](int index) const {
        return list[index];
    }
    
    inline Move& operator[](int index) {
        return list[index];
    }

    // Move Ordering function that prioritises captures and promotions before quiet moves
    // By searching likely-to-be-good moves, more beta cutoffs will happen
    inline void sort (Move pv, Move killer1 = NO_MOVE, Move killer2 = NO_MOVE, Move tt_move = NO_MOVE) {

        // Score each move 
        auto score = [pv, killer1, killer2, tt_move](Move m) -> int {
            if (m == pv) return 10000000;
            if (m == tt_move) return 9999999;
            
            if (CAPTURED(m) != NO_PIECE) {
                int victim = std::abs(material[CAPTURED(m)]);
                int attacker = std::abs(material[MOVED(m)]);
                
                
                return 1000000 + (10000 * victim) + (1000 - attacker);
            }
            
            if (FLAG(m) >= MOVE_NPROMO_FLAG) {
                
                return 900000 + promo_flag_bonus[FLAG(m) - MOVE_NPROMO_FLAG];
            }
            
            if (m == killer1) return 800000;
            if (m == killer2) return 700000;

            if (FLAG(m) == MOVE_DOUBLE_PUSH_FLAG) return 1000;
           
            return 0;
        };


        std::sort (list.begin(), list.begin() + size, [pv, killer1, killer2, score](Move a, Move b){
            return score (a) > score(b);
        });

        
    }

    // Prints the entire move list as well as the binary
    inline void print () {
        for (int i = 0; i < size; i++) {
            std::cout << std::bitset<32>(list[i]) << ": " << move_to_string(list[i]) << ",\n";
        }
    }

};





