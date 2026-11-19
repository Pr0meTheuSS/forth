#include <stdio.h>

#include "avl-tree.h"
#include "test.h"

int intless(const void *left, const void *right) {
  int *l = (int *)left;
  int *r = (int *)right;
  return (*l - *r);
}

bool init_avl_tree_test() {
  AvlTree *tree = create_avl_tree(intless);

  bool res = tree;
  res = res && (!tree->left);
  res = res && (!tree->right);
  res = res && (!tree->key);
  res = res && (tree->height == 1);

  destroy_avl_tree(tree);
  return res;
}

bool insert_and_find_avl_tree_test() {
  AvlTree *tree = create_avl_tree(intless);

  int key_value = 1;

  bool insert_res = avl_tree_insert(tree, &key_value, sizeof(key_value));
  if (!insert_res) {
    return false;
  }

  const Key *found = avl_tree_find(tree, &key_value, sizeof(key_value));

  destroy_avl_tree(tree);
  return found;
}

bool false_find_avl_tree_test() {
  AvlTree *tree = create_avl_tree(intless);

  int key_value = 1;
  const Key *found = avl_tree_find(tree, &key_value, sizeof(key_value));

  destroy_avl_tree(tree);
  return !found;
}

bool insert_and_find_lot_values_avl_tree_test() {
  AvlTree *tree = create_avl_tree(intless);
  bool res = true;

  for (int i = 0; i < 100; ++i) {
    int key_value = (i + 50) % 100;

    bool insert_res = avl_tree_insert(tree, &key_value, sizeof(key_value));
    if (!insert_res) {
      res = false;
      printf("insert error\n");
      goto cleanup;
    }

    const Key *found = avl_tree_find(tree, &key_value, sizeof(key_value));
    if (!found) {
      res = false;
      printf("find error\n");
      goto cleanup;
    }
  }
cleanup:
  destroy_avl_tree(tree);

  return res;
}

bool iterator_avl_tree_test() {
  AvlTree *tree = create_avl_tree(intless);
  bool res = true;

  int valuesAmount = 10;
  for (int i = 0; i < valuesAmount; ++i) {
    int key_value = (i + valuesAmount / 2) % valuesAmount;

    bool insert_res = avl_tree_insert(tree, &key_value, sizeof(key_value));
    if (!insert_res) {
      res = false;
      printf("insert error\n");
      goto cleanup;
    }
  }
  int treeAmount = 0;
  const AvlTree *iterator = avl_tree_get_next_iterator(tree, NULL);
  while (iterator) {
    int value = *(int *)iterator->key->data;
    if (value != treeAmount) {
      res = false;
      goto cleanup;
    }
    treeAmount++;
    iterator = avl_tree_get_next_iterator(tree, iterator);
  }
  res = res && treeAmount == valuesAmount;
cleanup:
  destroy_avl_tree(tree);

  return res;
}

static Test tests[] = {
    {.name = "init_avl_tree_test", .test = init_avl_tree_test},
    {.name = "insert_and_find_avl_tree_test",
     .test = insert_and_find_avl_tree_test},
    {.name = "false_find_avl_tree_test", .test = false_find_avl_tree_test},
    {.name = "insert_and_find_lot_values_avl_tree_test",
     .test = insert_and_find_lot_values_avl_tree_test},
    {.name = "iterator_avl_tree_test", .test = iterator_avl_tree_test}};

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
