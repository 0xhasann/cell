#include <stdio.h>
#include <string.h>


int main(int agrc, char *agrv[]) {

  while (1) {
    printf("$\n");

    char command[1024];
    fgets(command, sizeof(command), stdin);

    command[strcspn(command, "\n")] = 0;

    printf("%s: command not found\n", command);
  }

  return 0;
}