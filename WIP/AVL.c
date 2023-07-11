#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

typedef struct node {
	int element;
	struct node *left, *right;
	short BF;
} node;

node *newNode (int data){
	node *new = (node*)malloc(sizeof(node));
	new->element = data;
	new->left = new->right = NULL;
	new->BF = 0;
}

node *rotateLL (node* src){
	node *aux = src->left;
	src->left = aux->right;
	aux->right = src;
	src->BF = 0;
	aux->BF = 0;
	return aux;
}

node *rotateRR (node* src){
	node *aux = src->right;
	src->right = aux->left;
	aux->left = src;
	src->BF = 0;
	aux->BF = 0;
	return aux;
}

node *rotateLR (node *src){
	node *aux = src->left;
	node *aux2 = aux->right;
	aux->right = aux2->left;
	aux2->left = aux;
	src->left = aux2->right;
	aux2->right = src;
	if (aux2->BF == -1){
		src->BF = 1;
		aux->BF = 0;
		aux2->BF = 0;
	} else if (aux2->BF == 1){
		src->BF = 0;
		aux->BF = -1;
		aux2->BF = 0;
	} else {
		src->BF = 0;
		aux->BF = 0;
	}
	return aux2;
}

node *rotateRL (node *src){
	node *aux = src->right;
	node *aux2 = aux->left;
	aux->left = aux2->right;
	aux2->right = aux;
	src->right = aux2->left;
	aux2->left = src;
	if (aux2->BF == -1){
		src->BF = 0;
		aux->BF = 1;
		aux2->BF = 0;
	} else if (aux2->BF == 1){
		src->BF = -1;
		aux->BF = 0;
		aux2->BF = 0;
	} else {
		src->BF = 0;
		aux->BF = 0;
	}
	return aux2;
}

void preOrder(node *root)
{
    if(root != NULL)
    {
        printf("%d ", root->element);
        preOrder(root->left);
        preOrder(root->right);
    }
}

node *insertNode (node *src, int data, int *heightChange){
	if (!src){
		*heightChange = 1;
		return newNode (data);
	}
	
	if (data < src->element){
		src->left = insertNode(src->left, data, heightChange);
		if (*heightChange == 1){
			if (src->BF == 1)
				*heightChange = 0;
			src->BF--;
		}
	} else {
		src->right = insertNode(src->right, data, heightChange);
		if (*heightChange == 1){
			if (src->BF == -1)
				*heightChange = 0;
			src->BF++;
		}
	}
	
	if (src->BF == -2){
		if (src->left->BF == -1)
			src = rotateLL(src);
		else
			src = rotateLR(src);
		*heightChange = 0;
	} else if (src->BF == 2){
		if (src->right->BF == 1)
			src = rotateRR(src);
		else
			src = rotateRL(src);
		*heightChange = 0;
	}
	return src;
}

node *insert (node *src, int data){
	int height = 0;
	return insertNode(src, data, &height);
}

int main (){
	node *AVL = NULL;
	
	int *height = malloc(sizeof(int));;
	*height = 0;
	
	for (int i = 1; i < 32; i++){
		AVL = insert(AVL, i);
		*height = 0;
	}
	
	preOrder(AVL);
	printf ("\n");
	
	return 0;
}
