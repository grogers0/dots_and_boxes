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

bool Board::move(int player, const Edge &move)
{
    assert(is_move_valid(move));

    int oldscore = score[player];

    edges[edge_index(move)] = true;

    if (move.dir == HORIZ) {
        if (move.y > 0 && degree(Node(move.x, move.y - 1)) == 4)
            ++score[player];

        if (move.y < height && degree(Node(move.x, move.y)) == 4)
            ++score[player];
    } else {
        if (move.x > 0 && degree(Node(move.x - 1, move.y)) == 4)
            ++score[player];

        if (move.x < width && degree(Node(move.x, move.y)) == 4)
            ++score[player];
    }

    return oldscore != score[player];
}

void Board::unmove(int player, const Edge &move)
{
    assert(!is_move_valid(move));

    edges[edge_index(move)] = false;

    if (move.dir == HORIZ) {
        if (move.y > 0 && degree(Node(move.x, move.y - 1)) == 3)
            --score[player];

        if (move.y < height && degree(Node(move.x, move.y)) == 3)
            --score[player];
    } else {
        if (move.x > 0 && degree(Node(move.x - 1, move.y)) == 3)
            --score[player];

        if (move.x < width && degree(Node(move.x, move.y)) == 3)
            --score[player];
    }
}

bool Board::is_move_valid(const Edge &move) const
{
    if (move.dir == HORIZ) {
        if (move.x < 0 || move.x >= width || move.y < 0 || move.y > height)
            return false;

        return !edges[edge_index(move)];
    } else {
        if (move.x < 0 || move.x > width || move.y < 0 || move.y >= height)
            return false;

        return !edges[edge_index(move)];
    }
}

bool Board::is_game_over() const
{
    return std::count(edges.begin(), edges.end(), false) == 0;
}

int Board::degree(const Node &node) const
{
    return (int)edges[edge_index(Edge(HORIZ, node.x, node.y))] +
        (int)edges[edge_index(Edge(HORIZ, node.x, node.y + 1))] +
        (int)edges[edge_index(Edge(VERT, node.x, node.y))] +
        (int)edges[edge_index(Edge(VERT, node.x + 1, node.y))];
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
