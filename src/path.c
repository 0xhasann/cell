#include "path.h"
#include <fcntl.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/unistd.h>
#include <unistd.h>
#define MAX_VALUE 1024
#define IS_SPECIAL(c)                                                          \
  ((c) == '"' || (c) == '\\' || (c) == '$' || (c) == '`' || (c) == '\n')
static char command_buf[MAX_VALUE];

char *pwd() { return getcwd(command_buf, MAX_VALUE); }

char *commands[] = {"cd", "ls", "echo", "exit", NULL};

char *command_generator(const char *text, int state) {
  static int index, len;

  if (!state) {
    index = 0;
    len = strlen(text);
  }

  while (commands[index]) {
    char *name = commands[index];
    index++;

    if (strncmp(name, text, len) == 0) {
      return strdup(name);
    }
  }

  return NULL;
}

char **my_completion(const char *text, int start, int end) {
  return rl_completion_matches(text, command_generator);
}

void format_input(char *args[], char cmd[]) {
  char *ptr = cmd;
  char current[1024];
  int k = 0;
  int c = 0;

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

    if (*ptr == '\\') {
      ptr++;
      if (*ptr) {
        current[k++] = *ptr;
        ptr++;
      }
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
    } else if (*ptr == '"') {
      ptr++;
      while (*ptr && *ptr != '"') {

        if (*ptr == '\\') {
          ptr++;
          if (*ptr && IS_SPECIAL(*ptr)) {
            current[k++] = *ptr;
          } else {
            current[k++] = '\\';
            current[k++] = *ptr;
          }
          ptr++;
        } else {
          current[k++] = *ptr;
          ptr++;
        }
      }
      if (*ptr == '"') {
        ptr++;
      }
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
}

int handle_redirection(char *args[], int *redirection_fd) {

  for (int i = 0; args[i] != NULL; i++) {
    if (strcmp(args[i], ">") == 0 || strcmp(args[i], "1>") == 0 ||
        strcmp(args[i], ">>") == 0 || strcmp(args[i], "1>>") == 0) {
      *redirection_fd = STDOUT_FILENO;

    } else if (strcmp(args[i], "2>") == 0 || strcmp(args[i], "2>>") == 0) {
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

      int fd = open(fileName, O_WRONLY | O_CREAT | O_APPEND, 0644);
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

int is_builtin(char *cmd) {
  const char *builtins[] = {"cd", "echo", "exit", "pwd", "type"};
  int size = sizeof(builtins) / sizeof(builtins[0]);
  for (int i = 0; i < size; i++) {
    if (strcmp(cmd, builtins[i]) == 0)
      return 1;
  }
  return 0;
}

void type_command(char *cmd[]) {

  if (is_builtin(cmd[0])) {
    printf("%s is a shell builtin\n", cmd[0]);
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
    snprintf(full_path, sizeof(full_path), "%s/%s", dir, *cmd);
    if (access(full_path, X_OK) == 0) {
      printf("%s is %s\n", *cmd, full_path);
      free(copy_path);
      return;
    }
    dir = strtok(NULL, ":");
  }
  printf("%s not found\n", *cmd);
  free(copy_path);
}
