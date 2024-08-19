#ifndef TREE_H
#define TREE_H

#include "avl.h"
#include <stdlib.h>
//typedef TREE_NODE node;

typedef struct NODE
{
	void *data;
	char bal;
	struct NODE *left, *right;
} TREE_NODE;

typedef struct TREE
{
	TREE_NODE* root;
	size_t sz;
	int _height;
	size_t n;
	int (*compar)(const void *a, const void *b);
} TREE;

TREE *tree_init (const size_t sz, int (*compar)(const void*, const void*));

void tree_insert (TREE tree[restrict static 1], const void *key);
void tree_erase (TREE tree[restrict static 1], const void *key);

TREE_NODE* _tree_insert (const void *key, TREE_NODE *node, TREE tree[restrict static 1]);
TREE_NODE *_tree_erase (const void *key, TREE_NODE *node, TREE tree[restrict static 1]);

void tree_clear (TREE tree[restrict static 1]);

TREE_NODE* _tree_rotEE(TREE_NODE*);
TREE_NODE* _tree_rotDD(TREE_NODE*);
TREE_NODE* _tree_rotED(TREE_NODE*);
TREE_NODE* _tree_rotDE(TREE_NODE*);
TREE_NODE* _tree_rotEEremove (TREE_NODE*, int*);
TREE_NODE* _tree_rotDDremove (TREE_NODE*, int*);
TREE_NODE* _tree_rotEDremove (TREE_NODE*, int*);
TREE_NODE* _tree_rotDEremove (TREE_NODE*, int*);

TREE_NODE* _tree_min (TREE_NODE*);
TREE_NODE* _tree_max (TREE_NODE*);

#endif