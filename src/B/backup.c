/*
#define DLL_HEADER_SOURCE
#include "btree/btree_internal.h"
#undef DLL_HEADER_SOURCE

#undef DEBUG_X
#define DEBUG_X(fName, ret, ...) fName,
DEBUG_DEFINE_VTABLE(btree)

#include "log.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

TREE *btree_init (usize dataSize, u32 order, int (*compar)(const void*, const void*))
{
	LOGF_TRACE("BTree = { .rootNode = %p, .order = %u, .qttNodes = %u, .dataSize = %zu }", NULL, order, 0, dataSize);
	TREE *bt = malloc (sizeof (TREE));
	
	bt->rootNode = NULL;
	bt->order = order;
	bt->qttNodes = 0;
	bt->dataSize = dataSize;
	bt->compar = compar;
	
	
	bt->auxData = malloc (dataSize);
	bt->auxNode = NULL;
	
	return bt;
}

/*
void btree_print (TREE *bt, bool index)
{
	NODE *node = bt->rootNode;
	while (node->index)
		node = node->childNode[0];
	while (node) {
		printf ("qttKeys: %d\n", node->qttKeys);
		for (int i = 0; i < node->qttKeys; i++)
			printf ("%d ", *(int*)((char*)node->keys+i*bt->dataSize));
		printf ("\n");
		node = node->next;
	}
}
*/

void btree_insert(void *key, TREE *bt)
{
	
	if (bt->rootNode == NULL) {
		
		NODE *new = _malloc_node (bt, false);
		
		new->qttKeys = 1;
		memcpy (new->keys, key, bt->dataSize);
		
		bt->qttNodes++;
		bt->rootNode = new;
		
		return;
	}
	
	if (!_btree_insert (key, bt->rootNode, bt)) {
		NODE *new = _malloc_node (bt, true);
		bt->qttNodes++;
		
		new->childNode[0] = bt->rootNode;
		new->childNode[1] = bt->auxNode;
		memcpy (new->keys, bt->auxData, bt->dataSize);
		new->qttKeys = 1;
		
		
		bt->rootNode = new;
	}
}

bool _btree_insert (void *key, NODE *node, TREE *bt)
{
	uint16_t pos = _btree_bsearch (key, node, bt);
	
	if (node->index) {
		if (!_btree_insert (key, node->childNode[pos], bt)) {
			if (node->qttKeys == bt->order-1) {
				_btree_split (node, bt, pos);
				return false;
			} else {
				if (node->qttKeys != pos) {
					memmove (((char*)node->keys + (pos + 1) * bt->dataSize), ((char *)node->keys + pos * bt->dataSize), ((node->qttKeys - pos) * bt->dataSize));
					memmove (&node->childNode[pos+2], &node->childNode[pos+1], (node->qttKeys - pos + 1) * sizeof (NODE *));
				}
				memcpy (((char *)node->keys + pos * bt->dataSize), bt->auxData, bt->dataSize);
				node->childNode[pos+1] = bt->auxNode;
				node->qttKeys++;
			}
		}
	} else {
		if (node->qttKeys == bt->order-1) {
			_load_btree_aux (bt, key, NULL);
			_btree_split (node, bt, pos);
			return false;
		} else {
			if (node->qttKeys != pos) 
				memmove (((char *)node->keys + (pos + 1) * bt->dataSize), ((char *)node->keys + pos * bt->dataSize), ((node->qttKeys - pos) * bt->dataSize));
			memcpy (((char *)node->keys + pos * bt->dataSize), key, bt->dataSize);
			node->qttKeys++;
		}
	}
	
	return true;
}

void _btree_split (NODE *node, TREE *bt, uint16_t pos)
{
	NODE *new = _malloc_node (bt, node->index);
	bt->qttNodes++;

	node->qttKeys = bt->order/2;
	new->qttKeys = bt->order - node->qttKeys;
	
	if (pos < bt->order/2) {
		memcpy (new->keys, (char*)node->keys + (node->qttKeys-1) * bt->dataSize, (bt->order-node->qttKeys) * bt->dataSize);

		memmove ((char*)node->keys + (pos + 1) * bt->dataSize, (char*)node->keys + pos * bt->dataSize, (node->qttKeys - pos) * bt->dataSize);
		memcpy ((char*)node->keys + pos * bt->dataSize, bt->auxData, bt->dataSize);
		
		if (node->index) {
			memcpy (new->childNode, node->childNode + node->qttKeys, (bt->order - node->qttKeys) * sizeof (NODE*));
			memmove (node->childNode + pos + 2, node->childNode + pos + 1, (node->qttKeys - pos - 1) * sizeof (NODE*));
			node->childNode[pos+1] = bt->auxNode;
		}
		
	} else {
		memcpy (new->keys, (char*)node->keys + (node->qttKeys) * bt->dataSize, (bt->order-node->qttKeys-1) * bt->dataSize);
		
		pos -= node->qttKeys;

		memmove ((char*)new->keys + (pos + 1) * bt->dataSize, (char*)new->keys + pos * bt->dataSize, (new->qttKeys - pos) * bt->dataSize);
		memcpy ((char*)new->keys + pos * bt->dataSize, bt->auxData, bt->dataSize);

		if (node->index) {
			memcpy (new->childNode, node->childNode + new->qttKeys + !(bt->order%2), (bt->order - new->qttKeys - !(bt->order%2)) * sizeof (NODE*));
			memmove (new->childNode + pos + 1, new->childNode + pos, (new->qttKeys - pos) * sizeof (NODE*));
			new->childNode[pos] = bt->auxNode;
		}
		
	}

	
	memset (((char *)node->keys + node->qttKeys * bt->dataSize), 0, (bt->order - 1 - node->qttKeys) * bt->dataSize);
	_load_btree_aux (bt, new->keys, new);
	
	if (!node->index) {
		new->next = node->next;
		node->next = new;
	} else {
		new->qttKeys--;
		memmove (new->keys, (char*)new->keys + bt->dataSize, new->qttKeys * bt->dataSize);
		memset ((char*)new->keys + new->qttKeys * bt->dataSize, 0, new->qttKeys * bt->dataSize);

		memset (node->childNode + (node->qttKeys+1), 0, (bt->order - node->qttKeys - 1) * sizeof (NODE*));
		memset (new->childNode + (new->qttKeys+1), 0, (bt->order - new->qttKeys - 1) * sizeof (NODE*));
	}
}

















NODE *_malloc_node (TREE *bt, bool index)
{
	
	NODE *out = malloc(sizeof(NODE));
	out->qttKeys = 0;
	out->keys = malloc((bt->order-1) * bt->dataSize);
	memset (out->keys, 0, (bt->order-1) * bt->dataSize);
	
	if (index) {
		out->childNode = malloc (sizeof(NODE*) * bt->order);
		memset (out->childNode, 0, sizeof(NODE*) * bt->order);
	} else {
		out->childNode = NULL;
	}
	
	out->next = NULL;
	out->index = index;
	
	return out;
}

void _load_btree_aux (TREE *bt, void *key, NODE *node)
{
	if (!key && !node) {
		memset (bt->auxData, 0, bt->dataSize);
		bt->auxNode = NULL;
		return;
	}
	
	memset (bt->auxData, 0, bt->dataSize);
	memcpy (bt->auxData, key, bt->dataSize);
	bt->auxNode = node;
}

u64 _btree_bsearch (void *key, NODE *node, TREE *bt)
{
	
	u64 pos = 0;
	for (; pos < node ->qttKeys && bt->compar (key, (char*)node->keys + pos*bt->dataSize) > 0; pos++);
	
	//uint64_t pos = b_search (key, node->keys, node->qttKeys, bt->dataSize, bt->compar);
	//if ((pos == node->qttKeys-1 || pos == 0) && bt->compar (key, (char*)node->keys+pos*bt->dataSize) > 0)
	//	pos++;
	return pos;
}
*/