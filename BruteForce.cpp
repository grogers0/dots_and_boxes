#include "Board.h"

#include "boost/algorithm/combination.hpp"
#include <algorithm>
#include <utility>
#include <climits>
#include <cassert>

static void usage()
{
    printf("USAGE: ./brute_force <width> <height>\n");

    exit(1);
}

void do_brute_force_one_level(const Board &oldboard,
        std::vector<std::pair<Edge, short>> &table,
        std::vector<Edge>::iterator first, std::vector<Edge>::iterator middle,
        std::vector<Edge>::iterator last)
{
    do {
        Board board(oldboard);

        unsigned long idx = 0;
        std::for_each(first, middle, [&] (Edge edge)
                {
                    idx |= (((unsigned long)1) << board.edge_index(edge));
                    board.move(0, edge);
                });
        board.reset_score();

        Edge bestMove;
        int bestScoreDiff = INT_MIN;

        std::for_each(board.edge_begin(), board.edge_end(), [&] (Edge edge)
                {
                    if (!board.is_move_valid(edge))
                        return;
                    int nextIdx = idx | (((unsigned long)1) << board.edge_index(edge));
                    assert(table[nextIdx].second != SHRT_MAX);
                    bool tookSquare = board.move(0, edge);
                    if (tookSquare) {
                        int nextScore = board.get_score(0) +
                            table[nextIdx].second;
                        if (nextScore > bestScoreDiff) {
                            bestScoreDiff = nextScore;
                            bestMove = edge;
                        }
                        
                    } else {
                        int nextScore = -table[nextIdx].second;
                        if (nextScore > bestScoreDiff) {
                            bestScoreDiff = nextScore;
                            bestMove = edge;
                        }
                    }
                    board.unmove(0, edge);
                });

        table[idx].first = bestMove;
        table[idx].second = bestScoreDiff;

        board.print(stdout);
        bestMove.print(stdout);
        printf("index: %lu, expect to win by: %d\n\n", idx, bestScoreDiff);
    } while (boost::next_combination(first, middle, last));
}

void brute_force(const Board &oldboard)
{
    std::vector<Edge> edges;

    std::for_each(oldboard.edge_begin(), oldboard.edge_end(), [&] (Edge edge)
            { if (oldboard.is_move_valid(edge)) edges.push_back(edge); });

    std::vector<std::pair<Edge, short>> table;
    table.resize(((unsigned long)1) << oldboard.num_edges(), std::make_pair(Edge(), SHRT_MAX));
    table.back().second = 0; // initialize the final state

    std::sort(edges.begin(), edges.end());
    for (std::vector<Edge>::iterator middle = --edges.end();
            middle != edges.begin(); --middle) {
        do_brute_force_one_level(oldboard, table, edges.begin(), middle, edges.end());
    }

    do_brute_force_one_level(oldboard, table, edges.begin(), edges.begin(), edges.end());
}

int main(int argc, char **argv)
{
    if (argc < 3)
        usage();

    int width = atoi(argv[1]);
    int height = atoi(argv[2]);

    Board board(width, height);
    brute_force(board);
}
