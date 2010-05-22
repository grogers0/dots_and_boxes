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

bool Board::move(int player, Edge move)
{
    assert(is_move_valid(move));

    int oldscore = score[player];

    edges[edge_index(move)] = true;

    for_each_adjacent_node(move, [&] (Node node)
            { if (this->degree(node) == 4) ++score[player]; });

    return oldscore != score[player];
}

void Board::unmove(int player, Edge move)
{
    assert(!is_move_valid(move));

    edges[edge_index(move)] = false;

    for_each_adjacent_node(move, [&] (Node node)
            { if (this->degree(node) == 3) --score[player]; });
}

bool Board::is_move_valid(Edge move) const
{
    if (move.dir == HORIZ) {
        if (move.x < 0 || move.x >= width || move.y < 0 || move.y > height)
            return false;
    } else {
        if (move.x < 0 || move.x > width || move.y < 0 || move.y >= height)
            return false;
    }

    return !edges[edge_index(move)];
}

bool Board::is_game_over() const
{
    return std::count(edges.begin(), edges.end(), false) == 0;
}

int Board::degree(Node node) const
{
    int sum = 0;
    for_each_adjacent_edge(node, [&] (Edge edge)
            { sum += (int) edges[this->edge_index(edge)]; });
    return sum;
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
    } else if (base == "invalid") {
        decider = &Board::decide_move_invalid;
    } else if (base == "timeout") {
        decider = &Board::decide_move_timeout;
    } else if (base == "crash") {
        exit(1);
    } else if (base == "nocheap") {
        decider = &Board::decide_move_nocheap;
    } else {
        fprintf(stderr, "dots solver run with command: %s, cannot decide which move decider to use\n", base.c_str());
        exit(1);
    }
}
