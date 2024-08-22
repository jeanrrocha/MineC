#ifndef BLOCKS_H
#define BLOCKS_H

#include "tree.h"
#include "types.h"
#include "tree.h"

void insert_block (TREE *tree, BLOCK b);
void erase_block (TREE *tree, BLOCK b);
void clear_chunk (TREE *tree);
BLOCK* get_block (TREE *tree, BLOCK b);
BLOCK get_block_clone (TREE *tree, BLOCK b);
BLOCK *get_block_from_chunk (CHUNK_MANAGER *chunks, BLOCK b);

void redstone_lamp_update (BLOCK *, CHUNK_MANAGER *);
void redstone_dust_update (BLOCK *, CHUNK_MANAGER *);
void piston_update (BLOCK *, CHUNK_MANAGER *);

BLOCK *get_block_from_chunk (CHUNK_MANAGER *chunks, BLOCK b);
void insert_block_from_chunk (CHUNK_MANAGER *chunks, BLOCK b);
void erase_block_from_chunk (CHUNK_MANAGER *chunks, BLOCK b);

BLOCK *block_down (TREE* tree, BLOCK b);

#endif