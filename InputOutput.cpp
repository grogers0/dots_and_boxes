#include "Board.h"

#include <cstdio>
#include <cstdlib>

void Move::print(FILE *fp) const
{
    fprintf(fp, "<%c,%d,%d>\n", dir == HORIZ ? 'h' : 'v', x, y);
    fflush(fp);
}

Move read_move(FILE *fp)
{
    Move move;

    char tmp;
    fscanf(fp, "<%c,%d,%d>\n", &tmp, &move.x, &move.y);

    if (tmp == 'h')
        move.dir = HORIZ;
    else
        move.dir = VERT;

    return move;
}

void Board::print_horiz_edges(int y, FILE *fp) const
{
    fprintf(fp, "+");
    for (int x = 0; x < width; ++x) {
        if (edges[horiz_edge_index(x, y, width, height)])
            fprintf(fp, "-+");
        else
            fprintf(fp, " +");
    }
    fprintf(fp, "\n");
}

void Board::print_vert_edges(int y, FILE *fp) const
{
    for (int x = 0; x < width; ++x) {
        if (edges[vert_edge_index(x, y, width, height)])
            fprintf(fp, "|");
        else
            fprintf(fp, " ");

        if (degree(x, y) == 4)
            fprintf(fp, "#");
        else
            fprintf(fp, " ");
    }

    if (edges[vert_edge_index(width, y, width, height)])
        fprintf(fp, "|\n");
    else
        fprintf(fp, " \n");
}

void Board::print(FILE *fp, int player) const
{
    fprintf(fp, "%d %d %d %d\n", width, height, score[player], score[!player]);
    for (int y = 0; y < height; ++y) {
        print_horiz_edges(y, fp);
        print_vert_edges(y, fp);
    }
    print_horiz_edges(height, fp);
    fflush(fp);
}

void Board::read_horiz_edges(int y, FILE *fp)
{
    int c, x = 0;

    while ((c = fgetc(fp)) != EOF) {
        switch (c) {
            case '\r':
                break;
            case '\n':
                if (x != width) {
                    fprintf(stderr, "x (%d) != width (%d) at newline while reading board\n",
                            x, width);
                    exit(1);
                }
                return;
            case '+':
                break;
            case '-':
                if (x >= width) {
                    fprintf(stderr, "x (%d) >= width (%d) at dash while reading board\n",
                            x, width);
                    exit(1);
                }
                edges[horiz_edge_index(x, y, width, height)] = true;
                ++x;
                break;
            case ' ':
                if (x >= width) {
                    fprintf(stderr, "x (%d) >= width (%d) at space while reading board\n",
                            x, width);
                    exit(1);
                }
                edges[horiz_edge_index(x, y, width, height)] = false;
                ++x;
                break;
            default:
                fprintf(stderr, "unexpected character %d while reading board\n", c);
                exit(1);
        }
    }

    if (c == EOF)
        exit(0);
}

void Board::read_vert_edges(int y, FILE *fp)
{
    int c, x = 0;
    bool reading_cell = false;

    while ((c = fgetc(fp)) != EOF) {
        switch (c) {
            case '\r':
                break;
            case '\n':
                if (x != width + 1) {
                    fprintf(stderr, "x (%d) != width + 1 (%d) at '\\n', vert edges, y = %d\n",
                            x, width + 1, y);
                    exit(1);
                }
                return;
            case '|':
                if (x > width) {
                    fprintf(stderr, "x (%d) > width (%d) at '|', vert edges, y = %d\n",
                            x, width, y);
                    exit(1);
                }
                edges[vert_edge_index(x, y, width, height)] = true;
                ++x;
                break;
            case ' ':
                if (x > width) {
                    fprintf(stderr, "x (%d) > width (%d) at ' ', vert edges, y = %d\n",
                            x, width, y);
                    exit(1);
                }

                if (!reading_cell) {
                    edges[vert_edge_index(x, y, width, height)] = false;
                    ++x;
                }
                break;
            case '#':
                break;
            default:
                fprintf(stderr, "unexpected character %d reading board, vert edges, y = %d\n",
                        c, y);
                exit(1);
        }

        reading_cell = !reading_cell;
    }

    if (c == EOF)
        exit(0);
}

Board read_board(FILE *fp)
{
    Board board;

    fscanf(fp, "%d %d %d %d\n", &board.height, &board.width,
            &board.score[0], &board.score[1]);
    if (feof(fp))
        exit(0);

    board.edges.assign(board.width * (board.height + 1) + board.height * (board.width + 1), false);

    for (int y = 0; y < board.height; ++y) {
        board.read_horiz_edges(y, fp);
        board.read_vert_edges(y, fp);
    }
    board.read_horiz_edges(board.height, fp);

    if (feof(fp))
        exit(0);

    return board;
}


