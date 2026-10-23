#pragma once

typedef struct Key {
 void* data;
 size_t size;
 int(*comp)(const Key* left, const Key* right);
} Key;

typedef struct AvlTreeNode {
 struct AvlTreeNode* left;
 struct AvlTreeNode* right;
 Key key;
 int height;
} AvlTreeNode;

typedef struct AvlTree {
 AvlTreeNode root;
} AvlTree;

