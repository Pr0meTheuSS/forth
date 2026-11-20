#include "reader.h"
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct Reader {
  FILE *stream;
} Reader;

Reader *create_reader(FILE *in) {
  if (!in) {
    in = stdin;
  }
  Reader *ret = malloc(sizeof(Reader));
  if (!ret) {
    return NULL;
  }
  ret->stream = in;
  return ret;
}

void destroy_reader(Reader *reader) {
  // fclose(reader->stream);
  free(reader);
}

bool reader_get_next_word(Reader *reader, Word *word_out) {
  assert(reader);
  assert(word_out);
  int ret = fscanf(reader->stream, "%63s", word_out->word);
  return ret == 1;
}
