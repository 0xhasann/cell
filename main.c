#include "path.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

int main(int agrc, char *agrv[]) {

  while (1) {
    printf("$ ");

    char command[1024];
    fgets(command, sizeof(command), stdin);

    command[strcspn(command, "\n")] = 0;

    char first[5];
    char args[100];
    const char *inbuilt_command[] = {"echo", "type", "exit"};
    int size = sizeof(inbuilt_command) / sizeof(inbuilt_command[0]);

    sscanf(command, "%s %[^\n]", first, args);

    if (strcmp(first, "exit") == 0) {
      break;
    } else if (strcmp(first, "echo") == 0) {
      printf("%s\n", args);
    } else if (strcmp(first, "type") == 0) {
      type_command(args);
    } else if (strstr(first, "pwd") == first) {
      printf("%s\n", pwd());
    } else {
      printf("%s: command not found\n", command);
    }
  }

  return 0;
}