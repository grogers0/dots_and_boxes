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

struct Move
{
    Direction dir;
    int x, y;

    Move() : dir(HORIZ), x(-1), y(-1) {}
    Move(Direction dir, int x, int y) :
        dir(dir), x(x), y(y) {}

    void print(FILE *fp) const;
};

Move read_move(FILE *fp);

class Board
{
    public:
        Board(int width, int height);

        int get_width() const { return width; }
        int get_height() const { return height; }

        bool move(int player, const Move &move);
        void unmove(int player, const Move &move);
        bool is_move_valid(const Move &move) const;
        bool is_game_over() const;
        int get_score(int player) const { return score[player]; }

        int degree(int x, int y) const;

        void print(FILE *fp, int player = 0) const;

        void set_move_decider(const std::string &);
        Move decide_move() { return (this->*decider)(); }

    private:
        Board() {}
        void print_horiz_edges(int y, FILE *fp) const;
        void print_vert_edges(int y, FILE *fp) const;
        void read_horiz_edges(int y, FILE *fp);
        void read_vert_edges(int y, FILE *fp);

        int width, height, score[2];
        std::vector<bool> edges; // true if filled

        Move (Board::*decider)();

        Move decide_move_random();
        Move decide_move_first();

        friend Board read_board(FILE *fp);
};

Board read_board(FILE *fp);

inline int horiz_edge_index(int x, int y, int width, int height)
{
    return y * width + x;
}

inline int vert_edge_index(int x, int y, int width, int height)
{
    return width * (height + 1) + y * (width + 1) + x;
}

#endif
