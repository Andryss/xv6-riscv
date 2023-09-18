#include "kernel/types.h"
#include "user/user.h"

char buf[32];

void parent(int pipefd[2]) {
    fprintf(pipefd[1], "ping");

    sleep(2);

    int n;
    if ((n = read(pipefd[0], buf, sizeof(buf))) < 0) {
        fprintf(2, "pingpong: parent: read error");
        exit(1);
    }

    fprintf(1, "%d: got ", getpid());
    if (write(1, buf, n) != n) {
        fprintf(2, "pingpong: parent: write error\n");
        exit(1);
    }
    fprintf(1, "\n");
}

void child(int pipefd[2]) {
    sleep(1);

    int n;
    if ((n = read(pipefd[0], buf, sizeof(buf))) < 0) {
        fprintf(2, "pingpong: child: read error");
        exit(1);
    }

    fprintf(1, "%d: got ", getpid());
    if (write(1, buf, n) != n) {
        fprintf(2, "pingpong: child: write error\n");
        exit(1);
    }
    fprintf(1, "\n");

    fprintf(pipefd[1], "pong");
}

int main() {
    int pipes[2];

    if (pipe(pipes) == -1) {
        fprintf(2, "pingpong: pipe error\n");
        exit(1);
    }

    int pid = fork();

    if (pid == -1) {
        fprintf(2, "pingpong: fork error\n");
        exit(1);
    }

    if (pid == 0) {
        child(pipes);
    } else {
        parent(pipes);
    }
    close(pipes[0]);
    close(pipes[1]);
    exit(0);
}