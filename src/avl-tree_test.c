#include <stdio.h>

#include "avl-tree.h"

bool init_avl_tree_test() {
  AvlTree *tree = create_avl_tree();

  bool res = tree;
  res = res && (!tree->left);
  res = res && (!tree->right);
  res = res && (!tree->key);
  res = res && (tree->height == 1);

  destroy_avl_tree(tree);
  return res;
}

int intless(const Key *left, const Key *right) {
  int *l = (int *)left->data;
  int *r = (int *)right->data;
  return (*l - *r);
}

bool insert_and_find_avl_tree_test() {
  AvlTree *tree = create_avl_tree();

  Key key;

  int key_value = 1;
  key.data = &key_value;
  key.size = sizeof(int);
  key.comp = intless;

  bool insert_res = avl_tree_insert(tree, key);
  if (!insert_res) {
    return false;
  }

  const Key *found = avl_tree_find(tree, key);

  destroy_avl_tree(tree);

  return found;
}

bool false_find_avl_tree_test() {
  AvlTree *tree = create_avl_tree();

  Key key;

  int key_value = 1;
  key.data = &key_value;
  key.size = sizeof(int);
  key.comp = intless;

  const Key *found = avl_tree_find(tree, key);

  destroy_avl_tree(tree);

  return !found;
}

bool insert_and_find_lot_values_avl_tree_test() {
  AvlTree *tree = create_avl_tree();
  bool res = true;

  for (int i = 0; i < 100; ++i) {
    Key key;
    int key_value = i;

    key.data = &key_value;
    key.size = sizeof(key_value);
    key.comp = intless;

    bool insert_res = avl_tree_insert(tree, key);
    if (!insert_res) {
      res = false;
      printf("insert error");
      goto cleanup;
    }

    const Key *found = avl_tree_find(tree, key);
    if (!found) {
      res = false;
      printf("find error");
      goto cleanup;
    }
  }
cleanup:
  destroy_avl_tree(tree);

  return res;
}

typedef struct Test {
  const char *name;
  bool (*test)();
} Test;

Test tests[] = {
    {.name = "init_avl_tree_test", .test = init_avl_tree_test},
    {.name = "insert_and_find_avl_tree_test",
     .test = insert_and_find_avl_tree_test},
    {.name = "false_find_avl_tree_test", .test = false_find_avl_tree_test},
    {.name = "insert_and_find_lot_values_avl_tree_test",
     .test = insert_and_find_lot_values_avl_tree_test}};

bool avl_tree_run_all_tests(void) {
  for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); ++i) {
    if (!tests[i].test()) {
      printf("FAIL: %s\n", tests[i].name);
      return false;
    } else {
      printf("PASS: %s\n", tests[i].name);
    }
  }
  return true;
}
