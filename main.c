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

      //   bool flag = false;
      //   for (int i = 0; i < size; i++) {
      //     if (strcmp(args, inbuilt_command[i]) == 0) {
      //       flag = true;
      //       break;
      //     }
      //   }

      //   if (flag) {
      //     printf("%s is shell builtin\n", args);
      //   } else {
      //     printf("%s: not found\n", args);
      //   }
      type_command(args);

    } else {
      printf("%s: command not found\n", command);
    }
  }

  return 0;
}