#include "Board.h"
#include <cassert>

#include <vector>
#include <algorithm>

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

Edge Board::decide_move_random()
{
    std::vector<Edge> valid_moves;

    std::for_each(edge_begin(), edge_end(), [&] (Edge edge)
            { if (this->is_move_valid(edge)) valid_moves.push_back(edge); });

    int r = 0;

    int fd = open("/dev/urandom", O_RDONLY);
    read(fd, &r, sizeof(r));
    close(fd);

    return valid_moves[r % valid_moves.size()];
}

Edge Board::decide_move_first()
{
    EdgeIterator it = std::find_if(edge_begin(), edge_end(),
            [&] (Edge edge)
            { return this->is_move_valid(edge); });
    return *it;
}

Edge Board::decide_move_invalid()
{
    return Edge(HORIZ, -1, -1);
}

Edge Board::decide_move_timeout()
{
    usleep(1010000);
    return decide_move_first();
}

// picks any move that it can take a square first, then picks a random move
// from the remaining moves that won't give up a square to the enemy. If there
// are none, picks a random square
Edge Board::decide_move_nocheap()
{
    std::vector<Edge> take_square_moves, give_square_moves, other_moves;

    std::for_each(edge_begin(), edge_end(), [&] (Edge edge)
            {
                if (!this->is_move_valid(edge))
                    return;
                bool take_square = false, give_square = false;
                this->for_each_adjacent_node(edge, [&] (const Node &node)
                    {
                        switch (this->degree(node)) {
                            case 3: take_square = true; break;
                            case 2: give_square = true; break;
                        }
                    });
                if (take_square) {
                    take_square_moves.push_back(edge);
                } else if (give_square) {
                    give_square_moves.push_back(edge);
                } else {
                    other_moves.push_back(edge);
                }
            });

    if (!take_square_moves.empty()) {
        return take_square_moves.front();
    } else {
        int r = 0;

        int fd = open("/dev/urandom", O_RDONLY);
        read(fd, &r, sizeof(r));
        close(fd);

        if (!other_moves.empty()) {
            return other_moves[r % other_moves.size()];
        } else {
            assert(!give_square_moves.empty());
            return give_square_moves[r % give_square_moves.size()];
        }
    }
}
