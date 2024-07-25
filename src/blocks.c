#include <stdio.h>
#include <stdlib.h>
#include "avl.h"
#include "mat.h"
#include "blocks.h"
#include "math.h"

block *getPointerToElement (node *chunks, block b) {
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


void redstone_lamp_update (block *b, CHUNK_MANAGER *chunks) {
	
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
}


void redstone_dust_update (block *b, CHUNK_MANAGER *chunks) {
	
	
	block *down;
	block *up;
	block *north;
	block *south;
	block *east;
	block *west;
	down = blockDown(&chunks->chunks[1][1][1].blocks, *b);
	up = blockUp(&chunks->chunks[1][1][1].blocks, *b);
	north = blockNorth(&chunks->chunks[1][1][1].blocks, *b);
	south = blockSouth(&chunks->chunks[1][1][1].blocks, *b);
	east = blockEast(&chunks->chunks[1][1][1].blocks, *b);
	west = blockWest(&chunks->chunks[1][1][1].blocks, *b);
	
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
		
		if (south) {
			if (south->id == b->id && south->power < b->power) {
				south->update(south, chunks);
			}
		}
		
		if (east) {
			if (east->id == b->id && east->power < b->power) {
				east->update(east, chunks);
			}
		}
		
		if (west) {
			if (west->id == b->id && west->power < b->power) {
				west->update(west, chunks);
			}
		}
	}
}



void piston_update (block *b, CHUNK_MANAGER *chunks) {
	
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
	
}