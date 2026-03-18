#include <stdio.h>
#include <string.h>


int main(int agrc, char *agrv[]) {

  while (1) {
    printf("$ ");

    char command[1024];
    fgets(command, sizeof(command), stdin);

    command[strcspn(command, "\n")] = 0;

    if (strcmp(command, "exit") == 0) {
      break;
    } else if (strncmp(command, "echo ", 5) == 0) {
      printf("%s\n", command + 5);
    } else {
      printf("%s: command not found\n", command);
    }
  }

  return 0;
}