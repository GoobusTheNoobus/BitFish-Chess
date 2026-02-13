// ------------------------------------ BITFISH ---------------------------------------

#include <iostream>
#include <chrono>
#include <iomanip>

#include <cstdlib>
#include <ctime>

#include "bitboards.h"
#include "position.h"
#include "move.h"
#include "bitfish.h"

using namespace std::chrono;

void play_self () {
    while (true) {
        Move best_move = BitFish::iterative_deepen(100);

        if (best_move == NO_MOVE) {
            std::cout << "Game Ended! \n";
            break;
        }

        BitFish::current_pos.make_move(best_move);
        std::cout << move_to_string(best_move) << "\n";
        
        std::cout << BitFish::current_pos.to_string();

        

        
    }
}

int main() {
    Bitboards::init();
    std::cout << "BitFish " << VERSION << "\n";

    while (true) {
        std::string str;
        std::getline(std::cin, str);

        BitFish::current_pos.parse_fen(str);

        BitFish::go(16, 50000);
    }

    

    return 0;
}

/*int main() {
    std::srand(std::time(nullptr));

    Bitboards::init();

    std::string username;

    std::cout << "Username: ";
    std::getline(std::cin, username);

    std::array<std::string, 8> challenge_msgs = {
        "ima cook you " + username,
        "ur gonna taste the wrath of bitfish " + username + "!!!!",
        "u dont stand a chance against me " + username + ", might as well resign now",
        "prepare to be demolished " + username + "!!!",
        "bitfish doesnt lose to humans, its not in his rulebook to do so, " + username,
        "hope you brought calculator " + username + " cuz ur about to get mathed",
        "i just calculated mate in 67, " + username + ", its ggs", 
        "ur cheeks bouta be clapped, " + username
    };

    std::cout << challenge_msgs[std::rand() % 7] << "\n";
    std::cout << "im playign white cuz i like going first\n";

    while (true) {
        Move best_move = BitFish::iterative_deepen(20000);
        BitFish::current_pos.make_move(best_move);

        std::cout << "i go " << move_to_string(best_move) << ", ur move\n";
        std::string player_move_s;
        
        

        MoveList moves = MoveGen::generate_moves(BitFish::current_pos);

        Move player_move = NO_MOVE; 

        while (player_move == NO_MOVE) {
            std::getline(std::cin, player_move_s);
            for (int i = 0; i < moves.size; i++) {
                if (move_to_string(moves[i]) == player_move_s) {
                    player_move = moves[i];
                    break;
                }
            }
            if (player_move == NO_MOVE) {
                std::cout << "try again bozo\n"; 
                continue;
            }
        }
        



        BitFish::current_pos.make_move(player_move);
        std::cout << "bet\n";
    }

    return 0;
}*/
