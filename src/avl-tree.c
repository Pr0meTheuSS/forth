#include <stdlib.h>
#include <string.h>

#include "avl-tree.h"

AvlTree *create_avl_tree() {
  AvlTree *new_tree = (AvlTree *)malloc(sizeof(AvlTree));
  if (new_tree == NULL) {
    return new_tree;
  }
  new_tree->left = NULL;
  new_tree->right = NULL;
  new_tree->key = NULL;
  new_tree->height = 1;

  return new_tree;
}

void destroy_avl_tree(AvlTree *self) {
  if (!self) {
    return;
  }
  if (self->key) {
    free(self->key->data);
    free(self->key);
  }
  destroy_avl_tree(self->left);
  destroy_avl_tree(self->right);
  free(self);
}

const Key *avl_tree_find(const AvlTree *self, const Key key) {
  if (!self) {
    return NULL;
  }
  if (!self->key) {
    return NULL;
  }
  const Key *root_key = self->key;
  const Key *search_key = &key;

  int cmp_res = self->key->comp(root_key, search_key);
  if (cmp_res == 0) {
    return root_key;
  }

  if (cmp_res < 0) {
    return avl_tree_find(self->left, key);
  } else {
    return avl_tree_find(self->right, key);
  }
}

// void small_right_rotation(AvlTree* self);
// void small_left_rotation(AvlTree* self);
// void big_right_rotation(AvlTree* self);
// void big_left_rotation(AvlTree* self);

Key *create_copy_key(Key key) {
  Key *copy_key = (Key *)malloc(sizeof(Key));
  if (!copy_key) {
    return NULL;
  }

  copy_key->data = (void *)malloc(key.size);
  if (!copy_key->data) {
    return copy_key;
  }
  copy_key->size = key.size;

  memcpy(copy_key->data, key.data, copy_key->size);
  copy_key->comp = key.comp;

  return copy_key;
}

bool avl_tree_insert(AvlTree *self, Key key) {
  if (self == NULL) {
    return false;
  }
  if (self->key == NULL) {
    self->key = create_copy_key(key);
    return (self->key);
  }

  int cmp_res = key.comp(&key, self->key);
  if (cmp_res == 0) {
    return false;
  }
  if (cmp_res < 0) {
    return avl_tree_insert(self->left, key);
  } else {
    return avl_tree_insert(self->right, key);
  }
}

// bool avl_tree_remove(AvlTree *self, Key key) {

// }
