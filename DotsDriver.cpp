#include "Board.h"

#include <cstdlib>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <cerrno>

#include <string>
#include <stdexcept>

#include <unistd.h>
#include <pthread.h>
#include <signal.h>

static void usage()
{
    printf("USAGE: ./dots <width> <height> <player1> <player2>\n");

    exit(1);
}

static std::string command[2];
static int width, height;

static void parseArgs(int argc, char **argv)
{
    if (argc < 5)
        usage();

    width = atoi(argv[1]);
    height = atoi(argv[2]);
    command[0] = argv[3];
    command[1] = argv[4];
}

void move_fd(int oldfd, int newfd)
{
    assert(dup2(oldfd, newfd) != -1);
    assert(close(oldfd) != -1);
}

void spawn_child(const char *cmd, FILE *&child_stdin,
        FILE *&child_stdout, FILE *&child_stderr, pid_t &child_pid)
{
    int fd_stdin[2];
    int fd_stdout[2];
    int fd_stderr[2];

    assert(pipe(fd_stdin) != -1);
    assert(pipe(fd_stdout) != -1);
    assert(pipe(fd_stderr) != -1);

    pid_t pid = fork();
    assert(pid != -1);

    if (pid > 0) {
        // parent
        close(fd_stdin[0]);
        close(fd_stdout[1]);
        close(fd_stderr[1]);

        child_pid = pid;
        assert(child_stdin = fdopen(fd_stdin[1], "w"));
        assert(child_stdout = fdopen(fd_stdout[0], "r"));
        assert(child_stderr = fdopen(fd_stderr[0], "r"));
    } else {
        // child
        close(fd_stdin[1]);
        close(fd_stdout[0]);
        close(fd_stderr[0]);

        move_fd(fd_stdin[0], STDIN_FILENO);
        move_fd(fd_stdout[1], STDOUT_FILENO);
        move_fd(fd_stderr[1], STDERR_FILENO);

        if (execlp(cmd, cmd, (char*)0) == -1) {
            printf("Failed to execute \"%s\": %s\n",
                    cmd, strerror(errno));
            exit(1);
        }
    }
}

void *echo(void *args)
{
    FILE *fd = (FILE *)args;
    char buf[200];
    while (fgets(buf, sizeof(buf), fd)) {
        fputs(buf, stderr);
    }
    return NULL;
}

void print_win_info(const Board &board, int winning_player = -1)
{
    int score[2] = {board.get_score(0), board.get_score(1)};

    board.print(stdout);

    if (winning_player == -1) {
        if (score[0] > score[1]) {
            winning_player = 0;
        } else if (score[0] < score[1]) {
            winning_player = 1;
        } else {
            printf("tie game! final score => player 1: %d, player 2: %d\n",
                    score[0], score[1]);
            return;
        }
    }

    printf("player %d wins! final score => player 1: %d, player 2: %d\n",
            winning_player + 1, score[0], score[1]);
}

int main(int argc, char **argv)
{
    signal(SIGCHLD, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);

    parseArgs(argc, argv);

    FILE *player_stdin[2], *player_stdout[2], *player_stderr[2];
    pid_t player_pid[2];

    for (int player = 0; player <= 1; ++player) {
        spawn_child(command[player].c_str(), player_stdin[player],
                player_stdout[player], player_stderr[player],
                player_pid[player]);

        pthread_t thread;
        pthread_create(&thread, NULL, &echo, player_stderr[player]);
        pthread_detach(thread);
    }

    Board board(width, height);

    int player = 0;
    bool same_player = true;
    while (!board.is_game_over()) {
        if (!same_player)
            player = !player;

        board.print(stdout);
        printf("player %d's turn...\n", player + 1);

        board.print(player_stdin[player], player);

        Move move;
        try {
            move = read_move(player_stdout[player]);
        } catch (const std::exception &ex) {
            printf("player %d is disqualified (%s)\n", player + 1, ex.what());
            print_win_info(board, !player);
            exit(0);
        }

        if (!board.is_move_valid(move)) {
            printf("player %d is disqualified for making an invalid move: ", player + 1);
            move.print(stdout);
            print_win_info(board, !player);
            exit(0);
        }

        same_player = board.move(player, move);
    }

    print_win_info(board);
}
