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
#define MAX_COMMANDS 10000

static char *commands[MAX_COMMANDS];
static int command_count = 0;
static char command_buf[MAX_VALUE];

void add_completion(const char *cmd) {
  if (!cmd)
    return;

  for (int i = 0; i < command_count; i++) {
    if (strcmp(commands[i], cmd) == 0) {
      return;
    }
  }

  if (command_count < MAX_COMMANDS - 1) {
    commands[command_count++] = strdup(cmd);
    commands[command_count] = NULL;
  }
}

void load_path_commands() {
  static const char *builtins[] = {"cd", "echo", "exit", "pwd", "type", NULL};
  for (int i = 0; i < command_count; i++) {
    free(commands[i]);
    commands[i] = NULL;
  }
  command_count = 0;

  for (int i = 0; builtins[i] != NULL; i++) {
    add_completion(builtins[i]);
  }

  char *path = getenv("PATH");
  if (!path)
    return;

  char *copy = strdup(path);
  if (!copy)
    return;

  char *dir_path = strtok(copy, ":");
  while (dir_path != NULL) {
    DIR *dir = opendir(dir_path);
    if (dir) {
      struct dirent *entry;
      while ((entry = readdir(dir))) {
        if (entry->d_name[0] == '.')
          continue;

        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path,
                 entry->d_name);
        if (access(full_path, X_OK) == 0) {
          add_completion(entry->d_name);
        }
      }
      closedir(dir);
    }

    dir_path = strtok(NULL, ":");
  }

  free(copy);
}

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
  (void)end;
  if (start != 0)
    return NULL;

  load_path_commands();
  rl_completion_append_character = ' ';
  return rl_completion_matches(text, command_generator);
}
