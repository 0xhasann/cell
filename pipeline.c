#include "path.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

void run_builtin(char **cmd) {
  if (strcmp(cmd[0], "type") == 0) {
    if (strcmp(cmd[1], "exit") == 0) {
      printf("exit is a shell builtin\n");
    } else {
      printf("%s: not found\n", cmd[1]);
    }
  } else if (strcmp(cmd[0], "echo") == 0) {
    for (int i = 1; cmd[i] != NULL; i++) {
      printf("%s", cmd[i]);
      if (cmd[i + 1] != NULL) {
        printf(" ");
      }
    }

    printf("\n");
  }
}

/*
void run_pipeline(char **cmd1, char **cmd2) {
    int fd[2];
    pipe(fd);

    pid_t p1 = fork();

    if (p1 == 0) {
        dup2(fd[1], STDOUT_FILENO);
        close(fd[0]);
        close(fd[1]);

        if (is_builtin(cmd1[0])) {
          run_builtin(cmd1);
          exit(0);
        }

        execvp(cmd1[0], cmd1);
        perror("exec1 failed");
        exit(1);
    }

    pid_t p2 = fork();

    if (p2 == 0) {
        dup2(fd[0], STDIN_FILENO);
        close(fd[1]);
        close(fd[0]);

        if (is_builtin(cmd2[0])) {
          run_builtin(cmd2);
          exit(0);
        }

        execvp(cmd2[0], cmd2);
        perror("exec2 failed");
        exit(1);
    }

    close(fd[0]);
    close(fd[1]);

    waitpid(p1, NULL, 0);
    waitpid(p2, NULL, 0);
} */

void run_multi_pipeline(char ***commands, int n) {
  int prev_fd = -1;

  for (int i = 0; i < n; i++) {
    int fd[2];

    if (i < n - 1) {
      pipe(fd);
    }

    pid_t pid = fork();

    if (pid == 0) {
      if (prev_fd != -1) {
        dup2(prev_fd, STDIN_FILENO);
        close(prev_fd);
      }

      if (i < n - 1) {
        dup2(fd[1], STDOUT_FILENO);
        close(fd[0]);
        close(fd[1]);
      }

      if (is_builtin(commands[i][0])) {
        run_builtin(commands[i]);
        exit(0);
      }

      execvp(commands[i][0], commands[i]);
      perror("exec failed");
      exit(1);
    }

    if (prev_fd != -1) {
      close(prev_fd);
    }

    if (i < n - 1) {
      close(fd[1]);
      prev_fd = fd[0];
    }
  }

  for (int i = 0; i < n; i++) {
    wait(NULL);
  }
}