#include "path.h"
#include <readline/history.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int last_appended = 0;

void sync_history_append_index() { last_appended = history_length; }

void custom_append_history() {

  char *file = getenv("HISTFILE");

  if (!file) {
    return;
  }

  FILE *fp = fopen(file, "a");

  if (!fp) {
    perror("fopen");
    return;
  }

  for (int i = last_appended; i < history_length; i++) {
    HIST_ENTRY *entry = history_get(history_base + i);
    if (entry && entry->line) {
      fprintf(fp, "%s\n", entry->line);
    }
  }

  fclose(fp);

  last_appended = history_length;
}

void custom_history(char *args[], char *input) {

  int his_start_index = 0;
  if (args[1] != NULL) {
    his_start_index = history_length - atoi(args[1]);
  }

  if (args[1] && strcmp(args[1], "-r") == 0) {

    if (args[2]) {
      if (read_history(args[2]) != 0) {
        perror("read_history");
      }
    } else {
      printf("history: missing file\n");
    }
    return;
  } else if (args[1] && strcmp(args[1], "-w") == 0) {
    if (args[2]) {
      if (write_history(args[2]) != 0) {
        perror("write_history");
      }
    } else {
      printf("history: missing file\n");
    }
    return;
  } else if (args[1] && strcmp(args[1], "-a") == 0) {
    if (args[2]) {

      FILE *fp = fopen(args[2], "a");
      if (!fp) {
        perror("fopen");
        return;
      }

      for (int i = last_appended; i < history_length; i++) {
        HIST_ENTRY *entry = history_get(history_base + i);
        if (entry && entry->line) {
          fprintf(fp, "%s\n", entry->line);
        }
      }

      fclose(fp);

      last_appended = history_length;
    }
    return;
  }

  for (int i = his_start_index; i < history_length; i++) {
    HIST_ENTRY *entry = history_get(history_base + i);
    if (entry != NULL && entry->line != NULL) {
      printf("%d %s\n", history_base + i, entry->line);
    }
  }
}