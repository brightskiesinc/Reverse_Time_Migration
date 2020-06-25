#include "../include/string_list.h"
#include <stdlib.h>
#include <string.h>

void init_slist(string_list *list, int size) {
  list->size = 0;
  list->strings_list = (char **)malloc(sizeof(char *) * size);
  list->max_size = size;
}

void add_slist(string_list *list, char *word) {
  if (list->max_size == 0) {
    return;
  }
  if (list->size == list->max_size) {
    list->max_size *= MULT_RATE;
    list->strings_list =
        (char **)realloc(list->strings_list, sizeof(char *) * list->max_size);
  }
  list->strings_list[list->size] =
      (char *)malloc(sizeof(char) * (strlen(word) + 1));
  strncpy(list->strings_list[list->size], word, strlen(word) + 1);
  list->size++;
}

char slist_contains(string_list *list, char *word) {
  int i = 0;
  for (i = 0; i < list->size; i++) {
    if (strcmp(word, list->strings_list[i]) == 0) {
      return 1;
    }
  }
  return 0;
}

void destroy_slist(string_list *list) {
  int i = 0;
  for (i = 0; i < list->size; i++) {
    free(list->strings_list[i]);
  }
  free(list->strings_list);
  list->size = 0;
  list->max_size = 0;
}
