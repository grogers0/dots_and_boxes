#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <string>
#include <cstdio>

enum Direction
{
    HORIZ,
    VERT,
    DIR_MIN = HORIZ,
    DIR_MAX = VERT + 1
};

struct Edge
{
    Direction dir;
    int x, y;

    Edge() : dir(HORIZ), x(-1), y(-1) {}
    Edge(Direction dir, int x, int y) :
        dir(dir), x(x), y(y) {}

    void print(FILE *fp) const;
};

Edge read_edge(FILE *fp);

struct Node
{
    int x, y;

    Node() : x(-1), y(-1) {}
    Node(int x, int y) : x(x), y(y) {}

    void print(FILE *fp) const;
};

class Board
{
    public:
        Board(int width, int height);

        int get_width() const { return width; }
        int get_height() const { return height; }

        int edge_index(const Edge &edge) const;

        bool move(int player, const Edge &move);
        void unmove(int player, const Edge &move);
        bool is_move_valid(const Edge &move) const;
        bool is_game_over() const;
        int get_score(int player) const { return score[player]; }

        int degree(const Node &node) const;

        void print(FILE *fp, int player = 0) const;

        void set_move_decider(const std::string &);
        Edge decide_move() { return (this->*decider)(); }

    private:
        Board() {}
        void print_horiz_edges(int y, FILE *fp) const;
        void print_vert_edges(int y, FILE *fp) const;
        void read_horiz_edges(int y, FILE *fp);
        void read_vert_edges(int y, FILE *fp);

        int width, height, score[2];
        std::vector<bool> edges; // true if filled

        Edge (Board::*decider)();

        Edge decide_move_random();
        Edge decide_move_first();
        Edge decide_move_invalid();
        Edge decide_move_timeout();
        Edge decide_move_nocheap();

        friend Board read_board(FILE *fp);
};

Board read_board(FILE *fp);

inline int Board::edge_index(const Edge &edge) const
{
    if (edge.dir == HORIZ)
        return edge.y * width + edge.x;
    else
        return width * (height + 1) + edge.y * (width + 1) + edge.x;
}


#endif
