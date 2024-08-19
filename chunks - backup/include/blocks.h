#ifndef BLOCKS_H
#define BLOCKS_H

#include "avl.h"
#include "tree.h"

typedef struct {
	vec3 pos;
	TREE *blocks;
} CHUNK;

typedef struct {
	vec3 currentPos;
	CHUNK chunks[3][3][3];
} CHUNK_MANAGER;

void insert_block (TREE *tree, BLOCK b);
void erase_block (TREE *tree, BLOCK b);


void redstone_lamp_update (BLOCK *, CHUNK_MANAGER *);
void redstone_dust_update (BLOCK *, CHUNK_MANAGER *);
void piston_update (BLOCK *, CHUNK_MANAGER *);
#endif