#include "Board.h"

#include <string>
#include <cstdlib>

int main(int argc, char **argv)
{
    std::string solver = argv[0];

    while (true) {
        Board board = read_board(stdin);
        board.set_move_decider(solver);
        Move m = board.decide_move();
        m.print(stdout);
        fflush(stdout);
    }
}
