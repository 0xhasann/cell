#include "path.h"
#include <readline/history.h>
#include <readline/readline.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

  rl_bind_key('\t', rl_complete);
  rl_attempted_completion_function = my_completion;
  using_history();

  while (1) {
    // printf("$ ");
    // fflush(stdout);

    char *input = readline("$ ");
    if (!input)
      break;
    add_history(input);

    char command[1024];
    char *args[100];
    memset(args, 0, sizeof(args));

    // fgets(command, sizeof(command), stdin);
    strncpy(command, input, sizeof(command) - 1);

    // command[strcspn(command, "\n")] = 0;
    command[sizeof(command) - 1] = '\0';

    format_input(args, command);
    if (args[0] == NULL)
      continue;

    int redirection_fd = -1;

    int saved_output = handle_redirection(args, &redirection_fd);

    if (strcmp(args[0], "exit") == 0) {
      break;
    } else if (strcmp(args[0], "echo") == 0) {

      for (int j = 1; args[j] != NULL; j++) {
        printf("%s", args[j]);
        if (args[j + 1] != NULL)
          printf(" ");
      }
      printf("\n");
    } else if (strcmp(args[0], "type") == 0) {
      if (args[1] != NULL) {
        type_command(args + 1);
      }

    } else if (strcmp(args[0], "pwd") == 0) {
      printf("%s\n", pwd());
    } else if (strcmp(args[0], "cd") == 0) {
      if (args[1] == NULL || strcmp(args[1], "~") == 0) {
        char *home = getenv("HOME");
        chdir(home);
      } else if (chdir(args[1]) == -1) {
        printf("%s: No such file or directory\n", args[1]);
      }
    } else {

      char *path = find_executable(args[0]);

      if (path == NULL) {
        printf("%s: command not found\n", args[0]);
        continue;
      }
      pid_t pid = fork();

      if (pid == 0) {
        execv(path, args);

        perror("exec failed");
        exit(1);
      } else {
        wait(NULL);
      }
    }

    if (saved_output != -1) {
      dup2(saved_output, redirection_fd);
      close(saved_output);
    }

    free(input);
  }

  return 0;
}