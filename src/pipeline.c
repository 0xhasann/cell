#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

void parse_command(char *input, char **args) {
  int i = 0;

  char *token = strtok(input, " \t\n");

  while (token != NULL) {
    args[i++] = token;
    token = strtok(NULL, " \t\n");
  }

  args[i] = NULL;
}

void run_pipeline(char **cmd1, char **cmd2) {
    int fd[2];
    pipe(fd);

    pid_t p1 = fork();

    if (p1 == 0) {
        dup2(fd[1], STDOUT_FILENO);
        close(fd[0]);
        close(fd[1]);

        execvp(cmd1[0], cmd1);
        perror("exec1 failed");
        exit(1);
    }

    pid_t p2 = fork();

    if (p2 == 0) {
        dup2(fd[0], STDIN_FILENO);
        close(fd[1]);
        close(fd[0]);

        execvp(cmd2[0], cmd2);
        perror("exec2 failed");
        exit(1);
    }

    close(fd[0]);
    close(fd[1]);

    waitpid(p1, NULL, 0);
    waitpid(p2, NULL, 0);
}
