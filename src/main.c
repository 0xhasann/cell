#include "path.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
int main(int agrc, char *agrv[]) {

  while (1) {
    printf("$ ");
    fflush(stdout);

    char command[1024];
    char *args[100];
    char *ptr = command;
    char current[1024];
    int k = 0;
    int c = 0;
    fgets(command, sizeof(command), stdin);

    command[strcspn(command, "\n")] = 0;

    while (*ptr) {
      if (*ptr == ' ') {
        if (k > 0) {
          current[k] = '\0';
          args[c++] = strdup(current);
          k = 0;
        }
        ptr++;
        continue;
      }

      if (*ptr == '\'') {
        ptr++;

        while (*ptr && *ptr != '\'') {
          current[k++] = *ptr++;
        }

        if (*ptr == '\'')
          ptr++;

        continue;
      }
      current[k++] = *ptr;
      ptr++;
    }
    if (k > 0) {
      current[k] = '\0';
      args[c++] = strdup(current);
    }

    args[c] = NULL;
    if (args[0] == NULL)
      continue;
    const char *inbuilt_command[] = {"echo", "type", "exit"};
    int size = sizeof(inbuilt_command) / sizeof(inbuilt_command[0]);

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
      // printf("%s: command not found\n", args[0]);
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
  }

  return 0;
}