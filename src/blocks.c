#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "tree.h"

#include "math.h"

void insert_block (TREE *tree, BLOCK b) {
	tree_insert (tree, (void*)&b);
}

void erase_block (TREE *tree, BLOCK b) {
	tree_erase (tree, (void*)&b);
}

void clear_chunk (TREE *tree) {
	tree_clear (tree);
}

BLOCK* get_block (TREE *tree, BLOCK b) {
	return (BLOCK*)tree_search (tree, (void*)&b);
}

BLOCK get_block_clone (TREE *tree, BLOCK b) {
	return *get_block(tree, b);
}

#define CHUNK_SIZE 64

BLOCK *get_block_from_chunk (CHUNK_MANAGER *chunks, BLOCK b) {
	vec3 aux;

	aux.x = b.pos.x>=0?(int)b.pos.x/64:(int)b.pos.x/64-1;
	aux.y = b.pos.y>=0?(int)b.pos.y/64:(int)b.pos.y/64-1;
	aux.z = b.pos.z>=0?(int)b.pos.z/64:(int)b.pos.z/64-1;
	
	if (aux.x < chunks->currentPos.x-1 || aux.x > chunks->currentPos.x+1)
		return NULL;
	if (aux.y < chunks->currentPos.y-1 || aux.y > chunks->currentPos.y+1)
		return NULL;
	if (aux.z < chunks->currentPos.z-1 || aux.z > chunks->currentPos.z+1)
		return NULL;
	
	return get_block(chunks->chunks[(int)(aux.x+1 - chunks->currentPos.x)][(int)(aux.y+1 - chunks->currentPos.y)][(int)(aux.z+1 - chunks->currentPos.z)].blocks, b);
}

void insert_block_from_chunk (CHUNK_MANAGER *chunks, BLOCK b) {
	vec3 aux;

	aux.x = b.pos.x>=0?(int)b.pos.x/64:(int)b.pos.x/64-1;
	aux.y = b.pos.y>=0?(int)b.pos.y/64:(int)b.pos.y/64-1;
	aux.z = b.pos.z>=0?(int)b.pos.z/64:(int)b.pos.z/64-1;
	
	if (aux.x < chunks->currentPos.x-1 || aux.x > chunks->currentPos.x+1)
		return;
	if (aux.y < chunks->currentPos.y-1 || aux.y > chunks->currentPos.y+1)
		return;
	if (aux.z < chunks->currentPos.z-1 || aux.z > chunks->currentPos.z+1)
		return;
	
	return insert_block(chunks->chunks[(int)(aux.x+1 - chunks->currentPos.x)][(int)(aux.y+1 - chunks->currentPos.y)][(int)(aux.z+1 - chunks->currentPos.z)].blocks, b);
}


void erase_block_from_chunk (CHUNK_MANAGER *chunks, BLOCK b) {
	vec3 aux;

	aux.x = b.pos.x>=0?(int)b.pos.x/64:(int)b.pos.x/64-1;
	aux.y = b.pos.y>=0?(int)b.pos.y/64:(int)b.pos.y/64-1;
	aux.z = b.pos.z>=0?(int)b.pos.z/64:(int)b.pos.z/64-1;
	
	if (aux.x < chunks->currentPos.x-1 || aux.x > chunks->currentPos.x+1)
		return;
	if (aux.y < chunks->currentPos.y-1 || aux.y > chunks->currentPos.y+1)
		return;
	if (aux.z < chunks->currentPos.z-1 || aux.z > chunks->currentPos.z+1)
		return;
	
	return erase_block(chunks->chunks[(int)(aux.x+1 - chunks->currentPos.x)][(int)(aux.y+1 - chunks->currentPos.y)][(int)(aux.z+1 - chunks->currentPos.z)].blocks, b);
}



/*
TREE *relativePos (CHUNK_MANAGER *chunks, vec3 pos) {
	vec3 aux;
	aux.x = pos.x>=0?(int)pos.x/64:(int)pos.x/64-1;
	aux.y = pos.y>=0?(int)pos.y/64:(int)pos.y/64-1;
	aux.z = pos.z>=0?(int)pos.z/64:(int)pos.z/64-1;
	
	if (aux.x < chunks->currentPos.x-1 || aux.x > chunks->currentPos.x+1)
		return NULL;
	if (aux.y < chunks->currentPos.y-1 || aux.y > chunks->currentPos.y+1)
		return NULL;
	if (aux.z < chunks->currentPos.z-1 || aux.z > chunks->currentPos.z+1)
		return NULL;
	
	return chunks->chunks[(int)(aux.x+1 - chunks->currentPos.x)][(int)(aux.y+1 - chunks->currentPos.y)][(int)(aux.z+1 - chunks->currentPos.z)].blocks;
}
*/




BLOCK *block_down (TREE* tree, BLOCK b) {
	b.pos.y--;
	return get_block(tree, b);
}

/*
block *getPointerToElement (node *chunks, block b) {
	/*
	while (chunks) {
		if (compareBlocks(b, chunks->data))
			chunks = chunks->left;
		else if (compareBlocks(chunks->data, b))
			chunks = chunks->right;
		else
			return &chunks->data;
	}
	return NULL;
	
}


block *blockDown (node **chunks, block b) {
	return (getPointerToElement (*chunks, (block){b.x, b.y-1, b.z}));
}

block *blockUp (node **chunks, block b) {
	return (getPointerToElement (*chunks, (block){b.x, b.y+1, b.z}));
}

block *blockSouth (node **chunks, block b) {
	return (getPointerToElement (*chunks, (block){b.x, b.y, b.z+1}));
}

block *blockNorth (node **chunks, block b) {
	return (getPointerToElement (*chunks, (block){b.x, b.y, b.z-1}));
}

block *blockEast (node **chunks, block b) {
	return (getPointerToElement (*chunks, (block){b.x+1, b.y, b.z}));
}

block *blockWest (node **chunks, block b) {
	return (getPointerToElement (*chunks, (block){b.x-1, b.y, b.z}));
}
*/

void redstone_lamp_update (BLOCK *b, CHUNK_MANAGER *chunks) {
	printf ("AAA\n");
	
	/*
	b->powered = false;
	b->lit = false;
	
	block *down;
	if (down = blockDown(&chunks->chunks[1][1][1].blocks, *b)) {
		if (down->powered) {
			b->lit = true;
		}
	}
	
	block *up;
	if (up = blockUp(&chunks->chunks[1][1][1].blocks, *b)) {
		if (up->powered) {
			b->lit = true;
		}
	}
	
	block *north;
	if (north = blockNorth(&chunks->chunks[1][1][1].blocks, *b)) {
		if (north->powered) {
			b->lit = true;
		} else if (north->id == 9 && north->south == 1 && north->power > 0) {
			b->powered = true;
		}
	}
	
	block *south;
	if (south = blockSouth(&chunks->chunks[1][1][1].blocks, *b)) {
		if (south->powered) {
			b->lit = true;
		} else if (south->id == 9 && south->north == 1 && south->power > 0) {
			b->powered = true;
		}
	}
	
	block *east;
	if (east = blockEast(&chunks->chunks[1][1][1].blocks, *b)) {
		if (east->powered) {
			b->lit = true;
		} else if (east->id == 9 && east->west == 1 && east->power > 0) {
			b->powered = true;
		}
	}
	
	block *west;
	if (west = blockWest(&chunks->chunks[1][1][1].blocks, *b)) {
		if (west->powered) {
			b->lit = true;
		} else if (west->id == 9 && west->east == 1 && west->power > 0) {
			b->powered = true;
		}
	}
	
	if (b->powered) {
		b->lit = true;
	}
	*/
}


void redstone_dust_update (BLOCK *b, CHUNK_MANAGER *chunks) {
	
	/*
	block *down;
	block *up;
	block *north;
	block *south;
	block *east;
	block *west;
	
	block *up_north;
	block *up_south;
	block *up_east;
	block *up_west;
	
	block *down_north;
	block *down_south;
	block *down_east;
	block *down_west;
	
	down = blockDown(&chunks->chunks[1][1][1].blocks, *b);
	up = blockUp(&chunks->chunks[1][1][1].blocks, *b);
	
	north = blockNorth(&chunks->chunks[1][1][1].blocks, *b);
	south = blockSouth(&chunks->chunks[1][1][1].blocks, *b);
	east = blockEast(&chunks->chunks[1][1][1].blocks, *b);
	west = blockWest(&chunks->chunks[1][1][1].blocks, *b);
	
	up_north = blockNorth(&chunks->chunks[1][1][1].blocks, (block){b->x, b->y+1, b->z});
	up_south = blockSouth(&chunks->chunks[1][1][1].blocks, (block){b->x, b->y+1, b->z});
	up_east = blockEast(&chunks->chunks[1][1][1].blocks, (block){b->x, b->y+1, b->z});
	up_west = blockWest(&chunks->chunks[1][1][1].blocks, (block){b->x, b->y+1, b->z});
	
	down_north = blockNorth(&chunks->chunks[1][1][1].blocks, (block){b->x, b->y-1, b->z});
	down_south = blockSouth(&chunks->chunks[1][1][1].blocks, (block){b->x, b->y-1, b->z});
	down_east = blockEast(&chunks->chunks[1][1][1].blocks, (block){b->x, b->y-1, b->z});
	down_west = blockWest(&chunks->chunks[1][1][1].blocks, (block){b->x, b->y-1, b->z});
	
	b->north = 0;
	b->south = 0;
	b->east = 0;
	b->west = 0;
	
	short power = b->power;
	b->power = 0;
	
	if (north) {
		if (north->id == b->id) {
			b->north = 1;
			if (north->power > b->power)
				b->power = north->power - 1;
		}
		
		if (north->power_source) {
			b->north = 1;
			b->power = north->power;
		}
	}
	
	if (up_north) {
		if (up_north->id == b->id) {
			b->north = 1;
			if (up_north->power > b->power)
				b->power = up_north->power - 1;
		}
		
		if (up_north->power_source) {
			b->north = 1;
			b->power = up_north->power;
		}
		
	}
	
	if (down_north) {
		if (down_north->id == b->id) {
			b->north = 1;
			if (down_north->power > b->power)
				b->power = down_north->power - 1;
		}
		
		if (down_north->power_source) {
			b->north = 1;
			b->power = down_north->power;
		}
		//
	}
	
	if (south) {
		if (south->id == b->id) {
			b->south = 1;
			if (south->power > b->power)
				b->power = south->power - 1;
		}
		
		if (south->power_source) {
			b->south = 1;
			b->power = south->power;
		}
	}
	
	if (up_south) {
		
		if (up_south->id == b->id) {
			b->south = 1;
			if (up_south->power > b->power)
				b->power = up_south->power - 1;
		}
		
		if (up_south->power_source) {
			b->south = 1;
			b->power = up_south->power;
		}
		//
	}
	
	if (down_south) {
		if (down_south->id == b->id) {
			b->south = 1;
			if (down_south->power > b->power)
				b->power = down_south->power - 1;
		}
		
		if (down_south->power_source) {
			b->south = 1;
			b->power = down_south->power;
		}
		//
	}
	
	if (east) {
		if (east->id == b->id) {
			b->east = 1;
			if (east->power > b->power)
				b->power = east->power - 1;
		}
		
		if (east->power_source) {
			b->east = 1;
			b->power = east->power;
		}
	}
	
	if (up_east) {
		
		if (up_east->id == b->id) {
			b->east = 1;
			if (up_east->power > b->power)
				b->power = up_east->power - 1;
		}
		
		if (up_east->power_source) {
			b->east = 1;
			b->power = up_east->power;
		}
		//
	}
	
	if (down_east) {
		
		if (down_east->id == b->id) {
			b->east = 1;
			if (down_east->power > b->power)
				b->power = down_east->power - 1;
		}
		
		if (down_east->power_source) {
			b->east = 1;
			b->power = down_east->power;
		}
		//
	}
	
	if (west) {
		if (west->id == b->id) {
			b->west = 1;
			if (west->power > b->power)
				b->power = west->power - 1;
		}
		
		if (west->power_source) {
			b->west = 1;
			b->power = west->power;
		}
	}
	
	if (up_west) {
		
		if (up_west->id == b->id) {
			b->west = 1;
			if (up_west->power > b->power)
				b->power = up_west->power - 1;
		}
		
		if (up_west->power_source) {
			b->west = 1;
			b->power = up_west->power;
		}
		//
	}
	
	if (down_west) {
		
		if (down_west->id == b->id) {
			b->west = 1;
			if (down_west->power > b->power)
				b->power = down_west->power - 1;
		}
		
		if (down_west->power_source) {
			b->west = 1;
			b->power = down_west->power;
		}
		//
	}
	
	
	if (b->power > 0)
		b->lit = true;
	else
		b->lit = false;
	
	if ((b->north + b->south + b->east + b->west) == 1) {
		if (b->north)
			b->south = 1;
		if (b->south)
			b->north = 1;
		if (b->east)
			b->west = 1;
		if (b->west)
			b->east = 1;
	}
	
	if ((b->north + b->south + b->east + b->west) > 0) {		
		
		if (north) {
			if (north->id == b->id && north->power < b->power) {
				north->update(north, chunks);
			}
		}
		
		if (up_north) {
			if (up_north->id == b->id && up_north->power < b->power) {
				up_north->update(up_north, chunks);
			}
		}
		
		if (down_north) {
			if (down_north->id == b->id && down_north->power < b->power) {
				down_north->update(down_north, chunks);
			}
		}
		
		if (south) {
			if (south->id == b->id && south->power < b->power) {
				south->update(south, chunks);
			}
		}
		
		if (up_south) {
			if (up_south->id == b->id && up_south->power < b->power) {
				up_south->update(up_south, chunks);
			}
		}
		
		if (down_south) {
			if (down_south->id == b->id && down_south->power < b->power) {
				down_south->update(down_south, chunks);
			}
		}
		
		if (east) {
			if (east->id == b->id && east->power < b->power) {
				east->update(east, chunks);
			}
		}
		
		if (up_east) {
			if (up_east->id == b->id && up_east->power < b->power) {
				up_east->update(up_east, chunks);
			}
		}
		
		if (down_east) {
			if (down_east->id == b->id && down_east->power < b->power) {
				down_east->update(down_east, chunks);
			}
		}
		
		if (west) {
			if (west->id == b->id && west->power < b->power) {
				west->update(west, chunks);
			}
		}
		
		if (up_west) {
			if (up_west->id == b->id && up_west->power < b->power) {
				up_west->update(up_west, chunks);
			}
		}
		
		if (down_west) {
			if (down_west->id == b->id && down_west->power < b->power) {
				down_west->update(down_west, chunks);
			}
		}
	}
	*/
}



void piston_update (BLOCK *b, CHUNK_MANAGER *chunks) {
	/*
	b->lit = false;
	b->powered = false;

	block *down;
	if (down = blockDown(&chunks->chunks[1][1][1].blocks, *b)) {
		if (down->powered) {
			b->lit = true;
		}
	}
	
	block *up;
	if (up = blockUp(&chunks->chunks[1][1][1].blocks, *b)) {
		if (up->powered) {
			b->lit = true;
		}
	}
	
	block *north;
	if (north = blockNorth(&chunks->chunks[1][1][1].blocks, *b)) {
		if (north->powered) {
			b->lit = true;
		} else if (north->id == 9 && north->south == 1 && north->power > 0) {
			b->powered = true;
		}
	}
	
	block *south;
	if (south = blockSouth(&chunks->chunks[1][1][1].blocks, *b)) {
		if (south->powered) {
			b->lit = true;
		} else if (south->id == 9 && south->north == 1 && south->power > 0) {
			b->powered = true;
		}
	}
	
	block *east;
	if (east = blockEast(&chunks->chunks[1][1][1].blocks, *b)) {
		if (east->powered) {
			b->lit = true;
		} else if (east->id == 9 && east->west == 1 && east->power > 0) {
			b->powered = true;
		}
	}
	
	block *west;
	if (west = blockWest(&chunks->chunks[1][1][1].blocks, *b)) {
		if (west->powered) {
			b->lit = true;
		} else if (west->id == 9 && west->east == 1 && west->power > 0) {
			b->powered = true;
		}
	}
	
	if (b->powered) {
		b->lit = true;
	}
	
	if (b->lit && !b->tick) {
		
		block *goUp[16];
		int qtt = 0;
		
		for (int i = 0; i < 16; i++)
			goUp[i] = NULL;
		
		for (int i = 0; i < 16; i++) {
			if (up) {
				goUp[i] = up;
				up = blockUp(&chunks->chunks[1][1][1].blocks, *up);
				qtt++;
			} else
				break;
		}
		
		
		
		if (!goUp[15]) {
			printf ("AAAAA\n");
			for (int i = qtt-1; i >= 0; i--) {
				insertBlockToChunk (chunks, (BLOCK){(vec3){goUp[i]->x, goUp[i]->y+1, goUp[i]->z}, goUp[i]->id, goUp[i]->north, goUp[i]->south, goUp[i]->east, goUp[i]->west, goUp[i]->power, goUp[i]->powered, goUp[i]->power_source, goUp[i]->lit, goUp[i]->update, goUp[i]->tick});
				removeBlockToChunk (chunks, (BLOCK){(vec3){goUp[i]->x, goUp[i]->y, goUp[i]->z}, goUp[i]->id, goUp[i]->north, goUp[i]->south, goUp[i]->east, goUp[i]->west, goUp[i]->power, goUp[i]->powered, goUp[i]->power_source, goUp[i]->lit, goUp[i]->update, goUp[i]->tick});
			}
		}
		
		
		
		if (!up) {
			b->tick = true;
			insertBlockToChunk (chunks, (BLOCK){(vec3){b->x, b->y+1, b->z}, b->id+1, 0, 0, 0, 0, 0, 0, 0, NULL});
		}
		
	} else if (!b->lit && b->tick) {
		if (up)
			removeBlockToChunk (chunks, (BLOCK){(vec3){up->x, up->y, up->z}, up->id});
		b->tick = false;
	}
	*/
}