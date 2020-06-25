#ifndef STRING_LIST_H

#define STRING_LIST_H

#define MULT_RATE 2

typedef struct {
  char **strings_list;
  int size;
  int max_size;
} string_list;

void init_slist(string_list *list, int size);

void add_slist(string_list *list, char *word);

char slist_contains(string_list *list, char *word);

void destroy_slist(string_list *list);

#endif
