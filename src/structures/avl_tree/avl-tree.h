#pragma once

#include <stdbool.h>
#include <stddef.h>

typedef struct Key {
  void *data;
  size_t size;
} Key;

typedef struct AvlTree {
  struct AvlTree *left;
  struct AvlTree *right;

  struct AvlTree *parent;

  Key *key;
  int (*comp)(const void *left, const void *right);
  int height;
} AvlTree;

AvlTree *create_avl_tree(int (*key_comp)(const void *, const void *));
void destroy_avl_tree(AvlTree *self);

// void small_right_rotation(AvlTree* self);
// void small_left_rotation(AvlTree* self);
// void big_right_rotation(AvlTree* self);
// void big_left_rotation(AvlTree* self);

bool avl_tree_insert(AvlTree *self, void *data, size_t size);
// bool avl_tree_remove(AvlTree *self, Key key);

void avl_tree_split(const AvlTree *tree, AvlTree *left_sub_tree,
                    AvlTree *right_sub_tree);
const Key *avl_tree_find(const AvlTree *self, const void *data, size_t size);

const AvlTree *avl_tree_get_next_iterator(const AvlTree *tree,
                                          const AvlTree *iterator);
