#include "path.h"
#include <dirent.h>
#include <fcntl.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/wait.h>

#include <unistd.h>

int completion_start;

char *file_generator(const char *text, int state) {
  static DIR *dir;
  static struct dirent *entry;

  static char dir_path[1024];
  static char prefix[1024];
  static int has_slash;

  if (state == 0) {
    char *slash = strrchr(text, '/');

    if (slash) {
      has_slash = 1;

      int dir_len = slash - text + 1;
      strncpy(dir_path, text, dir_len);
      dir_path[dir_len] = '\0';

      strcpy(prefix, slash + 1);
    } else {
      has_slash = 0;

      strcpy(dir_path, ".");
      strcpy(prefix, text);
    }

    dir = opendir(dir_path);
  }

  if (!dir)
    return NULL;

  while ((entry = readdir(dir)) != NULL) {
    if (strncmp(entry->d_name, prefix, strlen(prefix)) == 0) {

      char *result;
      if (entry->d_type == DT_DIR) {
        rl_completion_append_character = '\0';

        if (has_slash) {
          result = malloc(strlen(dir_path) + strlen(entry->d_name) + 2);
          sprintf(result, "%s%s/", dir_path, entry->d_name);
        } else {
          result = malloc(strlen(entry->d_name) + 2);
          sprintf(result, "%s/", entry->d_name);
        }

        return result;
      } else {

        rl_completion_append_character = ' ';

        if (has_slash) {
          result = malloc(strlen(dir_path) + strlen(entry->d_name) + 1);
          sprintf(result, "%s%s", dir_path, entry->d_name);
        } else {

          result = strdup(entry->d_name);
        }

        return result;
      }
    }
  }

  closedir(dir);
  return NULL;
}

char *command_generator(const char *text, int state) {
  static int index, len;
  char *commands[] = {"echo", "exit", NULL};

  if (!state) {
    index = 0;
    len = strlen(text);
  }

  while (commands[index]) {
    char *name = commands[index];
    index++;

    if (strncmp(name, text, len) == 0) {
      char *result = malloc(strlen(name) + 2);
      sprintf(result, "%s", name);
      return result;
    }
  }

  return NULL;
}

char **my_completion(const char *text, int start, int end) {
  rl_attempted_completion_over = 1;
  completion_start = start;

  char **matches = NULL;

  if (start == 0) {

    matches = rl_completion_matches(text, command_generator);

    if (matches != NULL) {
      rl_completion_append_character = ' ';
    } else {
      matches = rl_completion_matches(text, file_generator);
    }
  } else {
    matches = rl_completion_matches(text, file_generator);
  }

  if (matches == NULL) {
    write(STDOUT_FILENO, "\a", 1);
  }
  return matches;
}
