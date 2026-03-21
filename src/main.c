#include "builtin.c"
#include "executable.c"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int agrc, char *agrv[]) {

  while (1) {
    printf("$ ");

    char command[1024];
    fgets(command, sizeof(command), stdin);

    command[strcspn(command, "\n")] = 0;

    // char name[10];
    // char args[1024];
    // const char *inbuilt_command[] = {"echo", "type", "exit"};
    // int size = sizeof(inbuilt_command) / sizeof(inbuilt_command[0]);

    // sscanf(command, "%s %[^\n]", name, args);
    char *argv[1024];
    int i = 0;

    char *token = strtok(command, " ");
    while (token != NULL) {
      argv[i++] = token;
      token = strtok(NULL, " ");
    }
    argv[i] = NULL;

    if (argv[0] == NULL)
      continue;

    if (strcmp(argv[0], "exit") == 0) {
      break;
    } else if (strcmp(argv[0], "echo") == 0) {
      // printf("%s\n", argv);
      for (int j = 1; argv[j] != NULL; j++) {
        printf("%s", argv[j]);
        if (argv[j + 1] != NULL)
          printf(" ");
      }
      printf("\n");
    } else if (strcmp(argv[0], "type") == 0) {
      type_command(argv + 1);
    } else if (strcmp(argv[0], "pwd") == 0) {
      printf("%s\n", pwd());
    } else if (strcmp(argv[0], "cd") == 0) {
      if (argv[1] == NULL) {
        char *home = getenv("HOME");
        chdir(home);
      } else if (chdir(argv[1]) == -1) {
        printf("%s: No such file or directory\n", argv[1]);
      }
    } else {
      // printf("%s: command not found\n", argv[0]);
      char *path = find_executable(argv[0]);

      if (path == NULL) {
        printf("%s: command not found\n", argv[0]);
        continue;
      }

      pid_t pid = fork();

      if (pid == 0) {
        execv(path, argv);

        perror("exec failed");
        exit(1);
      } else {
        wait(NULL);
      }
    }
  }

  return 0;
}