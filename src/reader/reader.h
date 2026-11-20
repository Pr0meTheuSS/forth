#pragma once
#include <stdbool.h>
#include <stdio.h>

typedef struct Reader Reader;
typedef struct Word {
  char word[64];
} Word;

Reader *create_reader(FILE *in);
void destroy_reader(Reader *reader);
bool reader_get_next_word(Reader *reader, Word *word_out);
