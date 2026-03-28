#include "path.h"
#include <dirent.h>
#include <fcntl.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include <sys/fcntl.h>
#include <sys/unistd.h>
#include <sys/wait.h>
#include <unistd.h>
#define MAX_VALUE 1024
#define IS_SPECIAL(c)                                                          \
  ((c) == '"' || (c) == '\\' || (c) == '$' || (c) == '`' || (c) == '\n')

static char command_buf[MAX_VALUE];

char *pwd() { return getcwd(command_buf, MAX_VALUE); }

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
      int append = (strcmp(args[i], ">>") == 0 || strcmp(args[i], "1>>") == 0 ||
                    strcmp(args[i], "2>>") == 0);
      char *fileName = args[i + 1];
      args[i] = NULL;
      args[i + 1] = NULL;
      int saved_output = dup(*redirection_fd);

      int fd = open(fileName, O_WRONLY | O_CREAT | (append ? O_APPEND : O_TRUNC),
                     0644);
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

void custom_echo(char *args[]) {
  for (int j = 1; args[j] != NULL; j++) {
    printf("%s", args[j]);
    if (args[j + 1] != NULL)
      printf(" ");
  }
  printf("\n");
}

void custom_cd(char *args[]) {
  if (args[1] == NULL || strcmp(args[1], "~") == 0) {
    char *home = getenv("HOME");
    chdir(home);
  } else if (chdir(args[1]) == -1) {
    printf("%s: No such file or directory\n", args[1]);
  }
}

void custom_executable(char *args[]) {
  char *path = find_executable(args[0]);

  if (path == NULL) {
    printf("%s: command not found\n", args[0]);
    return;
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
