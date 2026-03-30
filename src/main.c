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
  // using_history();

  while (1) {

    char command[1024];
    char *args[100];
    int redirection_fd = -1;

    char *input = readline("$ ");
    if (!input)
      break;
    add_history(input);

    memset(args, 0, sizeof(args));
    strncpy(command, input, sizeof(command) - 1);
    command[sizeof(command) - 1] = '\0';

    if (strchr(command, '|')) {
      char *left = strtok(command, "|");
      char *right = strtok(NULL, "|");

      while (*left == ' ')
        left++;
      while (*right == ' ')
        right++;

      char *cmd1[100];
      char *cmd2[100];

      format_input(cmd1, left);
      format_input(cmd2, right);

      run_pipeline(cmd1, cmd2);
      continue;
    }

    memset(args, 0, sizeof(args));
    format_input(args, command);
    if (args[0] == NULL)
      continue;

    int saved_output = handle_redirection(args, &redirection_fd);

    if (strcmp(args[0], "exit") == 0) {
      free(input);
      break;
    } else if (strcmp(args[0], "echo") == 0) {
      custom_echo(args);
    } else if (strcmp(args[0], "type") == 0) {
      type_command(args + 1);
    } else if (strcmp(args[0], "pwd") == 0) {
      printf("%s\n", pwd());
    } else if (strcmp(args[0], "cd") == 0) {
      custom_cd(args);
    } else {
      custom_executable(args);
    }

    if (saved_output != -1) {
      dup2(saved_output, redirection_fd);
      close(saved_output);
    }

    int exiting = (strcmp(args[0], "exit") == 0);
    free(input);
    if (exiting)
      break;
  }

  return 0;
}