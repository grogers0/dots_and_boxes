#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <string>
#include <cstdio>
#include <iterator>

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
    bool operator==(const Edge &edge) const
    { return dir == edge.dir && x == edge.x && y == edge.y; }
    bool operator!=(const Edge &edge) const
    { return dir != edge.dir || x != edge.x || y != edge.y; }

    void print(FILE *fp) const;
};

Edge read_edge(FILE *fp);

struct Node
{
    int x, y;

    Node() : x(-1), y(-1) {}
    Node(int x, int y) : x(x), y(y) {}

    bool operator==(const Node &node) const
    { return x == node.x && y == node.y; }
    bool operator!=(const Node &node) const
    { return x != node.x || y != node.y; }

    void print(FILE *fp) const;
};

class EdgeIterator
{
    public:
        typedef std::ptrdiff_t difference_type;
        typedef Edge value_type;
        typedef const Edge *pointer;
        typedef const Edge &reference;
        typedef std::forward_iterator_tag iterator_category;

        EdgeIterator(const Edge &edge, int width, int height) :
            edge(edge), width(width), height(height) {}

        const Edge &operator*() const { return edge; }
        const Edge *operator->() const { return &edge; }

        EdgeIterator &operator++()
        {
            if (edge.dir == HORIZ) {
                if (edge.x < width) {
                    if (edge.y <= height) {
                        ++edge.y;
                    } else {
                        ++edge.x;
                        edge.y = 0;
                    }
                } else {
                    edge.dir = VERT;
                    edge.x = 0;
                    edge.y = 0;
                }
            } else {
                if (edge.x <= width) {
                    if (edge.y < height) {
                        ++edge.y;
                    } else {
                        ++edge.x;
                        edge.y = 0;
                    }
                } else {
                    edge.dir = HORIZ;
                    edge.x = -1;
                    edge.y = -1;
                }
            }
            return *this;
        }

        EdgeIterator operator++(int)
        {
            EdgeIterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const EdgeIterator &it) const
        { return edge == it.edge; }
        bool operator!=(const EdgeIterator &it) const
        { return edge != it.edge; }

    private:
        Edge edge;
        int width, height;
};

class NodeIterator
{
    public:
        NodeIterator(const Node &node, int width) :
            node(node), width(width) {}

        const Node &operator*() const { return node; }
        const Node *operator->() const { return &node; }

        NodeIterator &operator++()
        {
            if (node.x < width) {
                ++node.x;
            } else {
                node.x = 0;
                ++node.y;
            }
            return *this;
        }

        NodeIterator operator++(int)
        {
            NodeIterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const NodeIterator &it) const
        { return node == it.node; }
        bool operator!=(const NodeIterator &it) const
        { return node != it.node; }

    private:
        Node node;
        int width;
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

        template<class F> void foreach_adjacent_node(const Edge &e, F f) const;
        template<class F> void foreach_adjacent_edge(const Node &n, F f) const;
        template<class F> void foreach_edge(F f) const;
        template<class F> void foreach_node(F f) const;

        void print(FILE *fp, int player = 0) const;

        void set_move_decider(const std::string &);
        Edge decide_move() { return (this->*decider)(); }

        EdgeIterator edge_begin() const
        { return EdgeIterator(Edge(HORIZ, 0, 0), width, height); }
        EdgeIterator edge_end() const
        { return EdgeIterator(Edge(HORIZ, -1, -1), width, height); }
        NodeIterator node_begin() const
        { return NodeIterator(Node(0, 0), width); }
        NodeIterator node_end() const
        { return NodeIterator(Node(0, height), width); }

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

template<class F>
void Board::foreach_adjacent_edge(const Node &node, F f) const
{
    f(Edge(HORIZ, node.x, node.y));
    f(Edge(HORIZ, node.x, node.y + 1));
    f(Edge(VERT, node.x, node.y));
    f(Edge(VERT, node.x + 1, node.y));
}

template<class F>
void Board::foreach_adjacent_node(const Edge &edge, F f) const
{
    if (edge.dir == HORIZ) {
        if (edge.y > 0)
            f(Node(edge.x, edge.y - 1));

        if (edge.y < height)
            f(Node(edge.x, edge.y));
    } else {
        if (edge.x > 0)
            f(Node(edge.x - 1, edge.y));

        if (edge.x < width)
            f(Node(edge.x, edge.y));
    }
}

template<class F>
void Board::foreach_edge(F f) const
{
    Edge edge;

    edge.dir = HORIZ;
    for (edge.x = 0; edge.x < width; ++edge.x) {
        for (edge.y = 0; edge.y <= height; ++edge.y) {
            f(edge);
        }
    }

    edge.dir = HORIZ;
    for (edge.x = 0; edge.x <= width; ++edge.x) {
        for (edge.y = 0; edge.y < height; ++edge.y) {
            f(edge);
        }
    }
}

template<class F>
void Board::foreach_node(F f) const
{
    Node node;
    for (node.x = 0; node.x < width; ++node.x) {
        for (node.y = 0; node.y < height; ++node.y) {
            f(node);
        }
    }
}


#endif
