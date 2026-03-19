#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/unistd.h>
#include <unistd.h>

int is_builtin(char *cmd) {
  const char *builtins[] = {"cd", "echo", "exit", "pwd", "type"};
  int size = sizeof(builtins) / sizeof(builtins[0]);
  for (int i = 0; i < size; i++) {
    if(strcmp(cmd, builtins[i]) == 0) return 1;
  }
  return 0;
}

void type_command(char *cmd) {


  if (is_builtin(cmd)) {
    printf("%s is a shell builtin\n", cmd);
    return;
  }

  char *path = getenv("PATH");
  if (path == NULL) {
    printf("PATH not found\n");
    return;
  }

  char *copy_path = strdup(path);

  char *dir = strtok(copy_path, ":");
  while (dir != NULL) {
    char full_path[1024];
    snprintf(full_path, sizeof(full_path), "%s/%s", dir, cmd);
    if (access(full_path, X_OK) == 0) {
      printf("%s is %s\n", cmd, full_path);
      free(copy_path);
      return;
    }
    dir = strtok(NULL, ":");
  
  }
  printf("%s not found\n", cmd);
  free(copy_path);

}
