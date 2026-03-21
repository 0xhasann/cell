#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

char *find_executable(char *cmd) {
  char *path = getenv("PATH");
  if (!path)
    return NULL;

  char *copy = strdup(path);
  char *dir = strtok(copy, ":");

  while (dir != NULL) {
    static char full[1024];

    snprintf(full, sizeof(full), "%s/%s", dir, cmd);

    if (access(full, X_OK) == 0) {
      free(copy);
      return full;
    }

    dir = strtok(NULL, ":");
  }

  free(copy);
  return NULL;
}