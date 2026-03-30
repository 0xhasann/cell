#include "path.h"
#include <readline/history.h>

void custom_history(char *args[]) {
  (void)args;

  for (int i = 0; i < history_length; i++) {
    HIST_ENTRY *entry = history_get(history_base + i);
    if (entry != NULL && entry->line != NULL) {
      printf("%d %s\n", history_base + i, entry->line);
    }
  }
}