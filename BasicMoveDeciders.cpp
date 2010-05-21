#include "Board.h"
#include <cassert>

#include <deque>

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

Move Board::decide_move_random()
{
    std::deque<Move> valid_moves;
    for (Direction dir = DIR_MIN; dir < DIR_MAX; dir = (Direction)(dir + 1)) {
        for (int x = 0; x <= width; ++x) {
            for (int y = 0; y <= height; ++y) {
                Move move(dir, x, y);
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

Move Board::decide_move_first()
{
    for (Direction dir = DIR_MIN; dir < DIR_MAX; dir = (Direction)(dir + 1)) {
        for (int x = 0; x <= width; ++x) {
            for (int y = 0; y <= height; ++y) {
                Move move(dir, x, y);
                if (is_move_valid(move))
                    return move;
            }
        }
    }
    assert(false); // unreached
}

Move Board::decide_move_invalid()
{
    return Move(HORIZ, -1, -1);
}

Move Board::decide_move_timeout()
{
    usleep(3000000);
    return decide_move_first();
}
