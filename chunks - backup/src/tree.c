#include "tree.h"
#include "avl.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

TREE *tree_init (const size_t sz, int (*compar)(const void*, const void*))
{
	TREE *tree = malloc(sizeof(TREE));

	if(!tree)
		return NULL;
	
	tree->root = NULL;
	tree->sz = sz;
	tree->compar = compar;
	tree->n = 0;
	
	return tree;
}

void* _tree_search (const void *key, TREE_NODE *node, TREE tree[restrict static 1]) {
	if (!node)
		return NULL;
	
	if (tree->compar(key, node->data) == 0)
		return node->data;
	
	return tree->compar(key, node->data) ? _tree_search(key, node->left, tree) : _tree_search(key, node->right, tree);
}

void* tree_search (TREE tree[restrict static 1], const void *key) {
	return _tree_search(key, tree->root, tree);
}


void _tree_clear (TREE_NODE *node) {
	if (!node)
		return;
	
	_tree_clear(node->left);
	_tree_clear(node->right);
	
	free (node->data);
	free (node);
}

void tree_clear (TREE tree[restrict static 1]) {
	_tree_clear(tree->root);
}

void tree_insert (TREE tree[restrict static 1], const void *key)
{
	tree->_height = 0;
	tree->root = _tree_insert (key, tree->root, tree);
}

TREE_NODE* _tree_insert (const void *key, TREE_NODE *node, TREE tree[restrict static 1])
{
	if (!node) {
		TREE_NODE* new_node = (TREE_NODE*)malloc(sizeof(TREE_NODE));
		new_node->data = malloc(tree->sz);
		memcpy (new_node->data, key, tree->sz);
		new_node->bal = 0;
		new_node->left = new_node->right = NULL;
		tree->n++;
		
		return new_node;
	}
	
	//printf ("%d\n", tree->compar(key, node->data));
	
	if (tree->compar(key, node->data)) {
		printf ("a\n");
		node->left = _tree_insert (key, node->left, tree);
		if (tree->_height = 1) {
			node->bal--;
			
			if (node->bal == 0) {
				tree->_height = 0;
				return node;
			} else if (node->bal == -2) {
				tree->_height = 0;
				if (node->left->bal == -1)
					return _tree_rotEE(node);
				else
					return _tree_rotED(node);
			}
			
		}
	} else if (tree->compar(node->data, key)) {
		node->right = _tree_insert (key, node->right, tree);
		if (tree->_height == 1) {
			node->bal++;
			if (node->bal == 0) {
				tree->_height = 0;
				return node;
			} else if (node->bal == 2) {
				tree->_height = 0;
				if (node->right->bal == 1)
					return _tree_rotDD(node);
				else
					return _tree_rotDE(node);
			}
		}
	}
	return node;
}








void tree_erase (TREE tree[restrict static 1], const void *key)
{
	if (!tree->root)
		return;
	tree->_height = 0;
	tree->root = _tree_erase (key, tree->root, tree);
}

TREE_NODE *_tree_erase (const void *key, TREE_NODE *node, TREE tree[restrict static 1]) {
	if (!node)
		return NULL;
	
	if (tree->compar(key, node->data)) {
		node->left = _tree_erase (key, node->left, tree);
		
		if (tree->_height == 1) {
			node->bal++;
			if (node->bal != 2) {
				tree->_height = 0;
				return node;
			} else {
				if (node->right->bal == -1)
					return _tree_rotDEremove(node, &tree->_height);
				return _tree_rotDDremove(node, &tree->_height);
			}
		}
		return node;
	}
	else if (tree->compar(node->data, key)) {
		node->right = _tree_erase (key, node->right, tree);

		if (tree->_height == 1) {
			node->bal--;
			if (node->bal != -2) {
				tree->_height = 0;
				return node;
			} else {
				if (node->left->bal == 1)
					return _tree_rotEDremove(node, &tree->_height);
				return _tree_rotEEremove(node, &tree->_height);
			}
		}
		return node;
	} 
	else {
		if (!node->left && !node->right) {
			tree->_height = 1;
			free (node->data);
			free (node);
			return NULL;
		} else if (!node->left || !node->right) {
			tree->_height = 1;
			TREE_NODE* aux = node->left ? node->left : node->right;
			if (node->left)
				node->left = NULL;
			else
				node->right = NULL;
			free (node->data);
			free (node);
			return aux;
		} else {
			TREE_NODE* aux = node->bal == -1 ? _tree_max(node->left) : _tree_min(node->right);
			memcpy (node->data, aux->data, tree->sz);
			memcpy (aux->data, key, tree->sz);
			node = _tree_erase (key, node, tree);
			return node;
		}
	}
	return node;
}

















TREE_NODE* _tree_min (TREE_NODE* node) {
	if (!node)
		return NULL;
	
	while (node->left)
		node = node->left;
	return node;
}

TREE_NODE* _tree_max (TREE_NODE* node) {
	if (!node)
		return NULL;
	
	while (node->right)
		node = node->right;
	return node;
}








TREE_NODE* _tree_rotEE(TREE_NODE* node) { 
	TREE_NODE* aux = node->left;
	node->left = aux->right;
	aux->right = node;
	node->bal = 0;
	aux->bal = 0;
	return aux;
}

TREE_NODE* _tree_rotDD(TREE_NODE* node) {
	TREE_NODE* aux = node->right;
	node->right = aux->left;
	aux->left = node;
	node->bal = 0;
	aux->bal = 0;
	return aux;
}

TREE_NODE* _tree_rotED(TREE_NODE* node) {
	TREE_NODE* aux = node->left;
	TREE_NODE* aux2 = aux->right;
	aux->right = aux2->left;
	aux2->left = aux;
	node->left = aux2->right;
	aux2->right = node;
	if (aux2->bal == -1){
		node->bal = 1;
		aux->bal = 0;
		aux2->bal = 0;
	} else if (aux2->bal == 1){
		node->bal = 0;
		aux->bal = -1;
		aux2->bal = 0;
	} else {
		node->bal = 0;
		aux->bal = 0;
	}
	return aux2;
}

TREE_NODE* _tree_rotDE(TREE_NODE* node) {
	TREE_NODE* aux = node->right;
	TREE_NODE* aux2 = aux->left;
	aux->left = aux2->right;
	aux2->right = aux;
	node->right = aux2->left;
	aux2->left = node;
	if (aux2->bal == -1){
		node->bal = 0;
		aux->bal = 1;
		aux2->bal = 0;
	} else if (aux2->bal == 1){
		node->bal = -1;
		aux->bal = 0;
		aux2->bal = 0;
	} else {
		node->bal = 0;
		aux->bal = 0;
	}
	return aux2;
}




TREE_NODE *_tree_rotEEremove (TREE_NODE *node, int *height) {
	TREE_NODE* aux = node->left;
	node->left = aux->right;
	aux->right = node;
	
	if (aux->bal == 0) {
		node->bal = -1;
		aux->bal = 1;
		*height = 0;
	} else {
		node->bal = 0;
		aux->bal = 0;
		*height = 1;
	}
	return aux;
}

TREE_NODE *_tree_rotDDremove (TREE_NODE *node, int *height) {
	TREE_NODE* aux = node->right;
	node->right = aux->left;
	aux->left = node;
	
	if (aux->bal == 0) {
		node->bal = 1;
		aux->bal = -1;
		*height = 0;
	} else {
		node->bal = 0;
		aux->bal = 0;
		*height = 1;
	}
	return aux;
}

TREE_NODE* _tree_rotEDremove (TREE_NODE* node, int *height) {
	TREE_NODE* aux = node->left;
	TREE_NODE* aux2 = aux->right;
	aux->right = aux2->left;
	aux2->left = aux;
	node->left = aux2->right;
	aux2->right = node;
	*height = 1;
	if (aux2->bal == -1){
		node->bal = 1;
		aux->bal = 0;
		aux2->bal = 0;
	} else if (aux2->bal == 1){
		node->bal = 0;
		aux->bal = -1;
		aux2->bal = 0;
	} else {
		node->bal = 0;
		aux->bal = 0;
	}
	return aux2;
}

TREE_NODE* _tree_rotDEremove (TREE_NODE* node, int *height) {
	TREE_NODE* aux = node->right;
	TREE_NODE* aux2 = aux->left;
	aux->left = aux2->right;
	aux2->right = aux;
	node->right = aux2->left;
	aux2->left = node;
	*height = 1;
	if (aux2->bal == -1){
		node->bal = 0;
		aux->bal = 1;
		aux2->bal = 0;
	} else if (aux2->bal == 1){
		node->bal = -1;
		aux->bal = 0;
		aux2->bal = 0;
	} else {
		node->bal = 0;
		aux->bal = 0;
	}
	return aux2;
}