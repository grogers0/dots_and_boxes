#include "Board.h"
#include <cassert>

#include <vector>

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

Edge Board::decide_move_random()
{
    std::vector<Edge> valid_moves;
    for (Direction dir = DIR_MIN; dir < DIR_MAX; dir = (Direction)(dir + 1)) {
        for (int x = 0; x <= width; ++x) {
            for (int y = 0; y <= height; ++y) {
                Edge move(dir, x, y);
                if (is_move_valid(move))
                    valid_moves.push_back(move);
            }
        }
    }

    int r = 0;

    int fd = open("/dev/urandom", O_RDONLY);
    read(fd, &r, sizeof(r));
    close(fd);

    return valid_moves[r % valid_moves.size()];
}

Edge Board::decide_move_first()
{
    for (Direction dir = DIR_MIN; dir < DIR_MAX; dir = (Direction)(dir + 1)) {
        for (int x = 0; x <= width; ++x) {
            for (int y = 0; y <= height; ++y) {
                Edge move(dir, x, y);
                if (is_move_valid(move))
                    return move;
            }
        }
    }
    assert(false); // unreached
}

Edge Board::decide_move_invalid()
{
    return Edge(HORIZ, -1, -1);
}

Edge Board::decide_move_timeout()
{
    usleep(3000000);
    return decide_move_first();
}

// picks any move that it can take a square first, then picks a random move
// from the remaining moves that won't give up a square to the enemy. If there
// are none, picks a random square
Edge Board::decide_move_nocheap()
{
    std::vector<Edge> take_square_moves, give_square_moves, other_moves;

    for (Direction dir = DIR_MIN; dir < DIR_MAX; dir = (Direction)(dir + 1)) {
        for (int x = 0; x <= width; ++x) {
            for (int y = 0; y <= height; ++y) {
                Edge move(dir, x, y);
                if (!is_move_valid(move))
                    continue;

#warning fixme
            }
        }
    }
}
