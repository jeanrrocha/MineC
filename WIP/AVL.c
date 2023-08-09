#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

typedef struct {
	int x;
	int y;
	int z;
	int id;
} block;

typedef struct node {
	block data;
	char bal;
	struct node *left, *right;
} node;

void destroyChunk (node* T) {
	if (!T)
		return;
	
	destroyChunk (T->left);
	destroyChunk (T->right);
	free (T);
}

node* rotEE( node* T ) { 
	node *aux = T->left;
	T->left = aux->right;
	aux->right = T;
	T->bal = 0;
	aux->bal = 0;
	return aux;
}

node* rotDD( node* T ) {
	node *aux = T->right;
	T->right = aux->left;
	aux->left = T;
	T->bal = 0;
	aux->bal = 0;
	return aux;
}

node* rotED( node* T ) {
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

node* rotDE( node* T ) {
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

int compareBlocks (block first, block second) {
	if (first.x != second.x)
		return first.x < second.x;
	if (first.y != second.y)
		return first.y < second.y;
	return first.z < second.z;
}

node* insert (node *T, block data, int* height) {
	
	if (!T) {
		node *new = (node*)malloc(sizeof(node));
		new->data = data;
		new->bal = 0;
		new->left = new->right = NULL;
		*height = 1;
		return new;
	}
	
	if (compareBlocks (data, T->data)) {
		T->left = insert (T->left, data, height);
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
		T->right = insert (T->right, data, height);
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
	
	*T = insert (*T, data, &height);
}

void print (node *T) {
	if (!T)
		return;
	
	print (T->left);
	printf ("%d %d %d %d\n", T->data.x, T->data.y, T->data.z, T->data.id);
	print (T->right);
}

void printPRE (node *T) {
	if (!T)
		return;
	
	printf ("%d %d %d %d\n", T->data.x, T->data.y, T->data.z, T->data.id);
	print (T->left);
	
	print (T->right);
}

void printPOS (node *T) {
	if (!T)
		return;
	
	print (T->left);
	
	print (T->right);
	printf ("%d %d %d %d\n", T->data.x, T->data.y, T->data.z, T->data.id);
}



int main (){
	node *chunk = NULL;
	
	int height = 0;
	
	//chunk = insert (chunk, (block){1, 1, 1, 0}, &height);
	
	print(chunk);
	
	insertBlock (&chunk, (block){1, 1, 1, 0});
	insertBlock (&chunk, (block){2, 1, 1, 0});
	insertBlock (&chunk, (block){3, 1, 1, 0});
	insertBlock (&chunk, (block){1, 1, 2, 0});
	insertBlock (&chunk, (block){1, 1, 1, 0});
	insertBlock (&chunk, (block){2, 2, 1, 0});
	insertBlock (&chunk, (block){1, 2, 2, 0});
	insertBlock (&chunk, (block){2, 1, 2, 0});
	insertBlock (&chunk, (block){2, 2, 2, 0});

	
	print(chunk);
	printf ("\n");
	printPRE(chunk);
	printf ("\n");
	printPOS(chunk);
	printf ("\n");
	
	return 0;
}
