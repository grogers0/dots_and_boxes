#include "Board.h"
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <algorithm>

Board::Board(int width, int height) :
    width(width), height(height),
    edges(width * (height + 1) + height * (width + 1), false),
    decider(NULL)
{
    score[0] = score[1] = 0;
}

bool Board::move(int player, const Move &move)
{
    assert(is_move_valid(move));

    int oldscore = score[player];

    if (move.dir == HORIZ) {
        edges[horiz_edge_index(move.x, move.y, width, height)] = true;

        if (move.y > 0 && degree(move.x, move.y - 1) == 4)
            ++score[player];

        if (move.y < height && degree(move.x, move.y) == 4)
            ++score[player];
    } else {
        edges[vert_edge_index(move.x, move.y, width, height)] = true;

        if (move.x > 0 && degree(move.x - 1, move.y) == 4)
            ++score[player];

        if (move.x < width && degree(move.x, move.y) == 4)
            ++score[player];
    }

    return oldscore != score[player];
}

void Board::unmove(int player, const Move &move)
{
    assert(!is_move_valid(move));

    if (move.dir == HORIZ) {
        edges[horiz_edge_index(move.x, move.y, width, height)] = false;

        if (move.y > 0 && degree(move.x, move.y) == 3)
            --score[player];

        if (move.y < height && degree(move.x, move.y + 1) == 3)
            --score[player];
    } else {
        edges[vert_edge_index(move.x, move.y, width, height)] = false;

        if (move.x > 0 && degree(move.x, move.y) == 3)
            --score[player];

        if (move.x < width && degree(move.x + 1, move.y) == 3)
            --score[player];
    }
}

bool Board::is_move_valid(const Move &move) const
{
    if (move.dir == HORIZ) {
        if (move.x < 0 || move.x >= width || move.y < 0 || move.y > height)
            return false;

        return !edges[horiz_edge_index(move.x, move.y, width, height)];
    } else {
        if (move.x < 0 || move.x > width || move.y < 0 || move.y >= height)
            return false;

        return !edges[vert_edge_index(move.x, move.y, width, height)];
    }
}

bool Board::is_game_over() const
{
    return std::count(edges.begin(), edges.end(), false) == 0;
}

int Board::degree(int x, int y) const
{
    return (int)edges[horiz_edge_index(x, y, width, height)] +
        (int)edges[horiz_edge_index(x, y + 1, width, height)] +
        (int)edges[vert_edge_index(x, y, width, height)] +
        (int)edges[vert_edge_index(x + 1, y, width, height)];
}


std::string basename_str(const std::string &str)
{
    char *cpy = strdup(str.c_str());
    char *base = basename(cpy);
    std::string ret(base);
    free(cpy);
    return ret;
}

void Board::set_move_decider(const std::string &solver)
{
    std::string base = basename_str(solver);
    if (base == "random") {
        decider = &Board::decide_move_random;
    } else if (base == "first") {
        decider = &Board::decide_move_first;
    } else {
        fprintf(stderr, "dots solver run with command: %s, cannot decide which move decider to use\n", base.c_str());
        exit(1);
    }
}
