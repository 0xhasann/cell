#include "path.h"
#include <readline/history.h>
#include <stdlib.h>
#include <string.h>

void custom_history(char *args[], char *input) {

  int his_start_index = 0;
  if (args[1] != NULL) {
    his_start_index = history_length - atoi(args[1]);
  }

  if (args[1] && strcmp(args[1], "-r") == 0) {

    if (args[2]) {
      args[2][strcspn(args[2], "\n")] = '\0';
      char *cmd_copy = strdup(input);

      clear_history();
      add_history(cmd_copy);

      if (read_history(args[2]) != 0) {
        perror("read_history");
      }

      free(cmd_copy);
      //   clear_history();
      //   if (read_history(args[2]) != 0) {
      //     perror("read_history");
      //   }
      // } else {
      //   printf("history: missing file\n");
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