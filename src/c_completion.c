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
static int last_was_tab = 0;

static const char *const builtin_commands[] = {"echo", "exit", NULL};

static void add_matching_builtins(char **matches, int *count, const char *text,
                                  int len) {
  for (int b = 0; builtin_commands[b]; b++) {
    const char *name = builtin_commands[b];
    if (strncmp(name, text, len) != 0)
      continue;
    int dup = 0;
    for (int i = 0; i < *count; i++) {
      if (strcmp(matches[i], name) == 0) {
        dup = 1;
        break;
      }
    }
    if (!dup && *count < 1024)
      matches[(*count)++] = strdup(name);
  }
}

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

    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
      continue;
    }
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
          result = malloc(strlen(dir_path) + strlen(entry->d_name) + 2);
          sprintf(result, "%s%s", dir_path, entry->d_name);
        } else {

          // result = strdup(entry->d_name);
          result = malloc(strlen(entry->d_name) + 1);
          sprintf(result, "%s", entry->d_name);
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

  if (!state) {
    index = 0;
    len = strlen(text);
  }

  while (builtin_commands[index]) {
    char *name = (char *)builtin_commands[index];
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

    // matches = rl_completion_matches(text, command_generator);
    matches = command_completion(text);

    // if (matches != NULL) {
    //   rl_completion_append_character = ' ';
    // }
  } else {
    matches = rl_completion_matches(text, file_generator);
  }

  // if (matches == NULL) {
  //   write(STDOUT_FILENO, "\a", 1);
  // }
  return matches;
}

static int all_matches_same_name(char **matches, int count) {
  for (int i = 1; i < count; i++) {
    if (strcmp(matches[0], matches[i]) != 0)
      return 0;
  }
  return 1;
}

static int compare_match_names(const void *a, const void *b) {
  const char *const *sa = a;
  const char *const *sb = b;
  return strcmp(*sa, *sb);
}

char **command_completion(const char *text) {
  char *matches[1024];
  int count = 0;
  int len = strlen(text);

  char *path_env = getenv("PATH");
  if (path_env) {
    char *paths = strdup(path_env);
    char *dir = strtok(paths, ":");

    while (dir) {
      DIR *d = opendir(dir);

      if (d) {
        struct dirent *entry;

        while ((entry = readdir(d)) != NULL) {
          if (strncmp(entry->d_name, text, len) == 0) {

            char full[1024];
            snprintf(full, sizeof(full), "%s/%s", dir, entry->d_name);

            if (access(full, X_OK) == 0) {
              matches[count++] = strdup(entry->d_name);
            }
          }
        }

        closedir(d);
      }

      dir = strtok(NULL, ":");
    }

    free(paths);
  }

  add_matching_builtins(matches, &count, text, len);

  if (count == 0)
    return NULL;

  char *common = strdup(matches[0]);

  for (int i = 1; i < count; i++) {
    int j = 0;
    while (common[j] && matches[i][j] && common[j] == matches[i][j]) {
      j++;
    }
    common[j] = '\0';
  }

  if (strcmp(common, text) != 0) {
    char **result = malloc(2 * sizeof(char *));
    result[0] = common;
    result[1] = NULL;

    if (count == 1 || all_matches_same_name(matches, count)) {
      rl_completion_append_character = ' ';
    } else {
      rl_completion_append_character = '\0';
    }

    last_was_tab = 0;

    for (int i = 0; i < count; i++)
      free(matches[i]);

    return result;
  }

  if (!last_was_tab) {
    write(STDOUT_FILENO, "\a", 1);
    last_was_tab = 1;

    for (int i = 0; i < count; i++)
      free(matches[i]);
    free(common);

    return NULL;
  }

  last_was_tab = 0;

  qsort(matches, count, sizeof(char *), compare_match_names);

  rl_on_new_line();
  printf("\n");

  for (int i = 0; i < count; i++) {
    printf("%s  ", matches[i]);
  }
  printf("\n");

  rl_on_new_line();
  rl_redisplay();

  for (int i = 0; i < count; i++)
    free(matches[i]);
  free(common);

  return NULL;
}