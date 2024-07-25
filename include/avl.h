#ifndef AVL_H
#define AVL_H

#include "mat.h"

typedef struct {
	double x;
	double y;
	double z;
} vector;

typedef vector point3D;

typedef struct block {
	int x;
	int y;
	int z;
	int id;
	short north;
	short south;
	short east;
	short west;
	short power;
	bool powered;
	bool power_source;
	bool lit;
	void (*update)(struct block *, struct CHUNK_MANAGER *);
	bool tick;
} block;

typedef struct node {
	block data;
	char bal;
	struct node *left, *right;
} node;


typedef struct {
	vec3 pos;
	node *blocks;
} CHUNK;

typedef struct {
	vec3 currentPos;
	CHUNK chunks[3][3][3];
} CHUNK_MANAGER;

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
	void (*update)(struct block *, struct CHUNK_MANAGER *);
	bool tick;
} BLOCK;

node* rotEE(node*);
node* rotDD(node*);
node* rotED(node*);
node* rotDE(node*);
node *rotEEremove (node*, int*);
node *rotDDremove (node*, int*);
node* rotEDremove (node*, int*);
node* rotDEremove (node*, int*);

block min (node*);
block max (node*);

int compareBlocks (block, block);
node* insertNode (node*, block, int*);
void insertBlock (node**, block);
node *removeNode (node*, block, int*);
void removeBlock (node**, block);

void clearChunk (node **);
void destroyChunk (node*);

#endif