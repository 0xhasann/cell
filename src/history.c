#include "path.h"
#include <readline/history.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
  }

  for (int i = his_start_index; i < history_length; i++) {
    HIST_ENTRY *entry = history_get(history_base + i);
    if (entry != NULL && entry->line != NULL) {
      printf("%d %s\n", history_base + i, entry->line);
    }
  }
}