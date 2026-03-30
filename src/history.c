#include "path.h"
#include <readline/history.h>
#include <stdlib.h>

void custom_history(char *args[]) {

  int his_start_index = 0;
  if (args[1] != NULL) {
    his_start_index = history_length - atoi(args[1]);
  }

  for (int i = his_start_index; i < history_length; i++) {
    HIST_ENTRY *entry = history_get(history_base + i);
    if (entry != NULL && entry->line != NULL) {
      printf("%d %s\n", history_base + i, entry->line);
    }
  }
}