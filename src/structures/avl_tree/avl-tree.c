#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "avl-tree.h"

AvlTree *create_avl_tree(int (*key_comp)(const void *, const void *)) {
  AvlTree *new_tree = (AvlTree *)malloc(sizeof(AvlTree));
  if (!new_tree) {
    return NULL;
  }

  new_tree->left = NULL;
  new_tree->right = NULL;
  new_tree->parent = NULL;
  new_tree->key = NULL;
  new_tree->height = 1;
  new_tree->comp = key_comp;
  return new_tree;
}

void destroy_avl_tree(AvlTree *self) {
  if (!self) {
    return;
  }
  if (self->key) {
    free(self->key->data);
    free((void *)self->key);
  }
  destroy_avl_tree(self->left);
  destroy_avl_tree(self->right);
  free(self);
}

const Key *avl_tree_find(const AvlTree *self, const void *key, size_t size) {
  if (!self) {
    return NULL;
  }

  if (!self->key) {
    return NULL;
  }
  const Key *root_key = self->key;

  int cmp_res = self->comp(key, root_key->data);
  if (cmp_res == 0) {
    return root_key;
  }

  if (cmp_res < 0) {
    return avl_tree_find(self->left, key, size);
  } else {
    return avl_tree_find(self->right, key, size);
  }
}

// void small_right_rotation(AvlTree* self);
// void small_left_rotation(AvlTree* self);
// void big_right_rotation(AvlTree* self);
// void big_left_rotation(AvlTree* self);

bool avl_tree_insert(AvlTree *self, void *key, size_t size) {
  assert(key);
  if (self == NULL) {
    return false;
  }

  if (self->key == NULL) {
    self->key = malloc(sizeof(*self->key));
    self->key->data = malloc(size);
    return memcpy(self->key->data, key, size);
  }

  int cmp_res = self->comp(key, self->key->data);
  if (cmp_res == 0) {
    return false;
  }

  if (cmp_res < 0) {
    self->left = self->left ? self->left : create_avl_tree(self->comp);
    self->left->parent = self;
    return avl_tree_insert(self->left, key, size);
  } else {
    self->right = self->right ? self->right : create_avl_tree(self->comp);
    self->right->parent = self;
    return avl_tree_insert(self->right, key, size);
  }
}

static const AvlTree *subtree_leftmost(const AvlTree *node) {
  while (node->left) {
    node = node->left;
  }
  return node;
}

const AvlTree *avl_tree_get_next_iterator(const AvlTree *tree,
                                          const AvlTree *iterator) {
  if (!tree || !tree->key) {
    return NULL;
  }

  // Первый вызов — самый левый (минимальный) узел.
  if (!iterator) {
    return subtree_leftmost(tree);
  }

  // Если есть правое поддерево — преемник это его левый край.
  if (iterator->right) {
    return subtree_leftmost(iterator->right);
  }

  // Иначе поднимаемся вверх, пока не выйдем из левого ребра.
  const AvlTree *node = iterator;
  const AvlTree *parent = node->parent;
  while (parent && parent->right == node) {
    node = parent;
    parent = parent->parent;
  }

  return parent;
}

// bool avl_tree_remove(AvlTree *self, Key key) {

// }
