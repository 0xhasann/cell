#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>

#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int handle_redirection(char *args[], int *redirection_fd) {

  for (int i = 0; args[i] != NULL; i++) {
    if (strcmp(args[i], ">") == 0 || strcmp(args[i], "1>") == 0) {
      *redirection_fd = STDOUT_FILENO;

    } else if (strcmp(args[i], "2>") == 0) {
      *redirection_fd = STDERR_FILENO;
    }
    if (*redirection_fd != -1) {
      if (args[i + 1] == NULL) {
        printf("Expacted file name after >");
        return -1;
      }
      char *fileName = args[i + 1];
      args[i] = NULL;
      args[i + 1] = NULL;
      int saved_output = dup(*redirection_fd);

      int fd = open(fileName, O_WRONLY | O_CREAT | O_TRUNC, 0644);
      if (fd < 0) {
        perror("open");
        return -1;
      }

      dup2(fd, *redirection_fd);
      close(fd);

      return saved_output;
    }
  }

  return -1;
}

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