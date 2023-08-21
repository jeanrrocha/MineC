#include <stdio.h>
#include <stdlib.h>
#include "avl.h"

node* rotEE(node* T) { 
	node *aux = T->left;
	T->left = aux->right;
	aux->right = T;
	T->bal = 0;
	aux->bal = 0;
	return aux;
}

node* rotDD(node* T) {
	node *aux = T->right;
	T->right = aux->left;
	aux->left = T;
	T->bal = 0;
	aux->bal = 0;
	return aux;
}

node* rotED(node* T) {
	node *aux = T->left;
	node *aux2 = aux->right;
	aux->right = aux2->left;
	aux2->left = aux;
	T->left = aux2->right;
	aux2->right = T;
	if (aux2->bal == -1){
		T->bal = 1;
		aux->bal = 0;
		aux2->bal = 0;
	} else if (aux2->bal == 1){
		T->bal = 0;
		aux->bal = -1;
		aux2->bal = 0;
	} else {
		T->bal = 0;
		aux->bal = 0;
	}
	return aux2;
}

node* rotDE(node* T) {
	node *aux = T->right;
	node *aux2 = aux->left;
	aux->left = aux2->right;
	aux2->right = aux;
	T->right = aux2->left;
	aux2->left = T;
	if (aux2->bal == -1){
		T->bal = 0;
		aux->bal = 1;
		aux2->bal = 0;
	} else if (aux2->bal == 1){
		T->bal = -1;
		aux->bal = 0;
		aux2->bal = 0;
	} else {
		T->bal = 0;
		aux->bal = 0;
	}
	return aux2;
}

node *rotEEremove (node *T, int *height) {
	node *aux = T->left;
	T->left = aux->right;
	aux->right = T;
	
	if (aux->bal == 0) {
		T->bal = -1;
		aux->bal = 1;
		*height = 0;
	} else {
		T->bal = 0;
		aux->bal = 0;
		*height = 1;
	}
	return aux;
}

node *rotDDremove (node *T, int *height) {
	node *aux = T->right;
	T->right = aux->left;
	aux->left = T;
	
	if (aux->bal == 0) {
		T->bal = 1;
		aux->bal = -1;
		*height = 0;
	} else {
		T->bal = 0;
		aux->bal = 0;
		*height = 1;
	}
	return aux;
}

node* rotEDremove (node *T, int *height) {
	node *aux = T->left;
	node *aux2 = aux->right;
	aux->right = aux2->left;
	aux2->left = aux;
	T->left = aux2->right;
	aux2->right = T;
	*height = 1;
	if (aux2->bal == -1){
		T->bal = 1;
		aux->bal = 0;
		aux2->bal = 0;
	} else if (aux2->bal == 1){
		T->bal = 0;
		aux->bal = -1;
		aux2->bal = 0;
	} else {
		T->bal = 0;
		aux->bal = 0;
	}
	return aux2;
}

node* rotDEremove (node *T, int *height) {
	node *aux = T->right;
	node *aux2 = aux->left;
	aux->left = aux2->right;
	aux2->right = aux;
	T->right = aux2->left;
	aux2->left = T;
	*height = 1;
	if (aux2->bal == -1){
		T->bal = 0;
		aux->bal = 1;
		aux2->bal = 0;
	} else if (aux2->bal == 1){
		T->bal = -1;
		aux->bal = 0;
		aux2->bal = 0;
	} else {
		T->bal = 0;
		aux->bal = 0;
	}
	return aux2;
}

block max (node *T) {
	if (!T)
		return (block){-1, -1, -1, -1};
	
	while (T->right)
		T = T->right;
	return T->data;
}

block min (node *T) {
	if (!T)
		return (block){-1, -1, -1, -1};
	
	while (T->left)
		T = T->left;
	return T->data;
}

int compareBlocks (block first, block second) {
	if (first.x != second.x)
		return first.x < second.x;
	if (first.y != second.y)
		return first.y < second.y;
	return first.z < second.z;
}

node* insertNode (node *T, block data, int *height) {
	
	if (!T) {
		node* newNode = (node*)malloc(sizeof(node));
		newNode->data = data;
		newNode->bal = 0;
		newNode->left = newNode->right = NULL;
		*height = 1;
		return newNode;
	}
	
	if (compareBlocks (data, T->data)) {
		T->left = insertNode (T->left, data, height);
		if (*height == 1) {
			T->bal--;
			if (T->bal == 0) {
				*height = 0;
				return T;
			} else if (T->bal == -2) {
				*height = 0;
				if (T->left->bal == -1)
					return rotEE(T);
				else
					return rotED(T);
			}
		}
	} else if (compareBlocks (T->data, data)) {
		T->right = insertNode (T->right, data, height);
		if (*height == 1) {
			T->bal++;
			if (T->bal == 0) {
				*height = 0;
				return T;
			} else if (T->bal == 2) {
				*height = 0;
				if (T->right->bal == 1)
					return rotDD(T);
				else
					return rotDE(T);
			}
		}
	}
	return T;
}

void insertBlock (node **T, block data) {
	
	int height = 0;
	
	*T = insertNode (*T, data, &height);
}

node *removeNode (node *T, block data, int *height) {
	if (!T)
		return NULL;
	
	if (compareBlocks (data, T->data)) {
		T->left = removeNode (T->left, data, height);
		
		if (*height == 1) {
			T->bal++;
			if (T->bal != 2) {
				*height = 0;
				return T;
			} else {
				if (T->right->bal == -1)
					return rotDEremove(T, height);
				return rotDDremove(T, height);
			}
		}
		return T;
	} else if (compareBlocks (T->data, data)) {
		T->right = removeNode (T->right, data, height);

		if (*height == 1) {
			T->bal--;
			if (T->bal != -2) {
				*height = 0;
				return T;
			} else {
				if (T->left->bal == 1)
					return rotEDremove(T, height);
				return rotEEremove(T, height);
			}
		}
		return T;
	} else {
		if (!T->left && !T->right) {
			*height = 1;
			free (T);
			return NULL;
		} else if (!T->left || !T->right) {
			*height = 1;
			node *aux = T->left ? T->left : T->right;
			free (T);
			return aux;
		} else {
			block aux = T->bal == -1 ? max(T->left) : min(T->right);
			T = removeNode (T, aux, height);
			T->data = aux;
			return T;
		}
	}
	return T;
}

void removeBlock (node **T, block data) {
	if (!*T)
		return;
	
	int height = 0;
	
	*T = removeNode (*T, data, &height);
}

void clearChunk (node *T) {
	if (!T)
		return;
	
	destroyChunk (T->left);
	destroyChunk (T->right);
	
	removeBlock (&T, T->data);
}

void destroyChunk (node *T) {
	if (!T)
		return;
	
	destroyChunk (T->left);
	destroyChunk (T->right);
	free (T);
}
