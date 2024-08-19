#ifndef AVL_H
#define AVL_H

#include "mat.h"

typedef struct {
	double x;
	double y;
	double z;
} vector;

typedef vector point3D;

typedef struct {
	vec3 pos;
	int id;
	short north;
	short south;
	short east;
	short west;
	short power;
	bool powered;
	bool power_source;
	bool lit;
	void (*update)(struct BLOCK *, struct CHUNK_MANAGER *);
	bool tick;
} BLOCK;

typedef struct node {
	BLOCK data;
	char bal;
	struct node *left, *right;
} node;

node* rotEE(node*);
node* rotDD(node*);
node* rotED(node*);
node* rotDE(node*);
node *rotEEremove (node*, int*);
node *rotDDremove (node*, int*);
node* rotEDremove (node*, int*);
node* rotDEremove (node*, int*);

BLOCK min (node*);
BLOCK max (node*);

int compareBlocks (BLOCK, BLOCK);
node* insertNode (node*, BLOCK, int*);
void insertBlock (node**, BLOCK);
node *removeNode (node*, BLOCK, int*);
void removeBlock (node**, BLOCK);

void clearChunk (node **);
void destroyChunk (node*);

#endif