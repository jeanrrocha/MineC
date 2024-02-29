/*
 * Copyright (c) 1990 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. [rescinded 22 July 1999]
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>

#define NODE btree_node
#define TREE BTree
#define PAIR btreePair

typedef struct NODE {
	
	uint64_t rrn;
	uint64_t qttKeys;
	void *keys;
	bool leaf;
	uint64_t *childRRN;
	struct NODE **childNode;
	
} NODE;

typedef struct PAIR {
	void *key;
	NODE *right;
} PAIR;

typedef struct {

	uint64_t rootRRN;
	NODE *rootNode;

	bool ram;
	FILE *file;

	uint64_t qttNodes;

	uint64_t sizeKey;

	uint64_t order;

	int (*comp)(const void *key, const void *elem);
} TREE;

void initialize_btree (TREE *bt, bool ram, FILE *file, uint64_t dataSize, uint64_t order, int (*comp)(const void *key, const void *elem)) {
	bt->rootRRN = -1;
	bt->rootNode = NULL;
	bt->ram = ram;
	bt->file = file;
	bt->qttNodes = 0;
	bt->sizeKey = dataSize;
	bt->order = order;
	bt->comp = comp;
}


int comp_int (const void *key, const void *elem) {
	return*((int*)key) - *((int*)elem);
}

void print_BTree_header (TREE *bt) {
	printf ("\n\n\nrootRRN:   %llu\n", bt->rootRRN);
	//printf ("rootNode:   %x\n", bt->rootNode);
	printf ("ram:   %d\n", bt->ram);
	//printf ("file:   %x\n", bt->file);
	printf ("qttNodes:   %llu\n", bt->qttNodes);
	printf ("sizeKey:   %llu\n", bt->sizeKey);
	printf ("order:   %llu\n", bt->order);
	//printf ("comp:   %x\n\n\n", bt->comp);
}

void btree_insert(void *key, TREE *bt);
int b_search (const void *key, const void *base0, size_t nmemb, size_t size, int (*compar)(const void *, const void *));
int btree_bsearch (void *key, NODE *node, TREE *bt);

int main () {
	TREE test;
	initialize_btree (&test, true, NULL, sizeof(int), 4, comp_int);

	int a = 65;
	btree_insert (&a, &test);
	
	a = 67;
	btree_insert (&a, &test);
	
	a = 69;
	btree_insert (&a, &test);
	
	
	
	printf ("%d\n", *(int*)test.rootNode->keys);
	printf ("%d\n", *(int*)(test.rootNode->keys + 4));
	printf ("%d\n\n\n", *(int*)(test.rootNode->keys + 8));
	
	a = 64;
	btree_insert (&a, &test);
	a = 66;
	btree_insert (&a, &test);
	//a = 68;
	//btree_insert (&a, &test);
	//a = 70;
	//btree_insert (&a, &test);
	
	//printf ("\n%d\n", *(int*)test.rootNode->keys);
	//printf ("%d\n", *(int*)(test.rootNode->keys + 4));
	//printf ("%d\n\n\n", *(int*)(test.rootNode->keys + 8));
	
	//printf ("\n%d\n", *(int*)test.rootNode->childNode[0]->keys);
	//printf ("%d\n", *(int*)(test.rootNode->childNode[0]->keys + 4));
	//printf ("%d\n\n\n", *(int*)(test.rootNode->childNode[0]->keys + 8));
	
	//printf ("\n%d\n", *(int*)test.rootNode->childNode[1]->keys);
	//printf ("%d\n", *(int*)(test.rootNode->childNode[1]->keys + 4));
	//printf ("%d\n\n\n", *(int*)(test.rootNode->childNode[1]->keys + 8));
	
	
	
	
}

NODE *btree_node_malloc (TREE *bt) {
	
	NODE *out = (NODE*)malloc(sizeof(NODE));
	
	out->rrn = bt->rootRRN;
	out->qttKeys = 0;
	out->keys = malloc(bt->order * bt->sizeKey);
	memset (out->keys, 0, bt->order * bt->sizeKey);
	out->leaf = true;
	
	if (bt->ram) {
		out->childNode = (NODE**) malloc (sizeof(NODE*) * bt->order);
		memset (out->childNode, 0, sizeof(void*) * bt->order);
		out->childRRN = NULL;
	} else {
		out->childNode = NULL;
		out->childRRN = (uint64_t*) malloc (sizeof(uint64_t) * bt->order);
	}

	return out;

}

bool btree_insert_aux_ram (void *key, NODE *node, TREE *bt);

void btree_insert(void *key, TREE *bt) {
	
	if (!bt->ram)
		goto file;
	
	if (bt->rootNode == NULL) {
		NODE *new = btree_node_malloc (bt);
		
		new->qttKeys = 1;
		new->leaf = true;
		memcpy (new->keys, key, bt->sizeKey);
		
		bt->qttNodes++;
		bt->rootNode = new;
		
		//printf ("%s\n", (char*)new->keys);
		//printf ("%llu\n", sizeof(new->keys));
		
	} else
		btree_insert_aux_ram (key, bt->rootNode, bt);
	
	
	return;
	
	
	file:
	
	return;
	
}

PAIR btree_divide_ram(PAIR promo, NODE *node, TREE *bt, int pos);

bool btree_insert_aux_ram (void *key, NODE *node, TREE *bt) {

	int pos = btree_bsearch (key, node, bt);
	
	printf ("%d\n", pos);

	if (!node->leaf && !btree_insert_aux_ram (key, node->childNode[pos], bt)) {
		/*
		NODE *target = *(node->childNode+pos*sizeof(NODE*));
		NODE *left = NULL;
		NODE *right = NULL
		
		if (pos > 0)
			left = *(node->childNode+(pos-1)*sizeof(NODE*));
		if (pos < bt->order-1)
			right = *(node->childNode+(pos+1)*sizeof(NODE*));
		*/
		
	}
	
	else {
		if (node->qttKeys == bt->order-1) {
			
			
			PAIR in;
			in.key = malloc (bt->sizeKey);
			memcpy (in.key, key, bt->sizeKey);
			PAIR out = btree_divide_ram (in, node, bt, pos);
			
			NODE *new = btree_node_malloc (bt);
			new->leaf = false;
			new->childNode[0] = bt->rootNode;
			new->childNode[1] = out.right;
			memcpy (new->keys, out.key, bt->sizeKey);
			
			printf ("child: %llu\n", new->childNode[0]);
			printf ("child: %llu\n", new->childNode[1]);
			
			bt->qttNodes++;
			bt->rootNode = new;
			
		} else {
			if (node->qttKeys != pos) 
				memmove (((char *)node->keys + (pos + 1) * bt->sizeKey), ((char *)node->keys + pos * bt->sizeKey), ((node->qttKeys - pos) * bt->sizeKey));
			memcpy (((char *)node->keys + pos * bt->sizeKey), key, bt->sizeKey);
			node->qttKeys++;
		}
	}
	return false;
}

PAIR btree_divide_ram(PAIR promo, NODE *node, TREE *bt, int pos) {

	PAIR out;
	out.key = malloc (bt->sizeKey);
	NODE *new = btree_node_malloc (bt);
	new->leaf = node->leaf;
	
	bt->qttNodes++;
	
	if (pos <= (bt->order-1)/2) {
		memcpy (new->keys, (char*)node->keys + bt->sizeKey * ((bt->order-1)/2), bt->sizeKey * (bt->order/2));
		
		node->qttKeys -= (bt->order/2);
		new->qttKeys = (bt->order/2);
		
		memmove (((char *)node->keys + (pos + 1) * bt->sizeKey), ((char *)node->keys + pos * bt->sizeKey), ((node->qttKeys - pos) * bt->sizeKey));
		memcpy (((char *)node->keys + pos * bt->sizeKey), promo.key, bt->sizeKey);
	} else {
		
	}
	
	memcpy (out.key, ((char *)node->keys + node->qttKeys * bt->sizeKey), bt->sizeKey);
	memset (((char *)node->keys + node->qttKeys * bt->sizeKey), 0, (bt->order - 1 - node->qttKeys) * bt->sizeKey);
	
	out.right = new;

	return out;
	
	
	/*
	if (i <= (btree_order-1)/2) {
		
		
		
		
		memcpy (*new.chaves, *node->chaves + t->tam_chave * ((btree_order-1)/2), t->tam_chave * (btree_order/2));
		
		memmove (*node->chaves + t->tam_chave * (i+1), *node->chaves + t->tam_chave * i, (node->qtd_chaves-1-i)*t->tam_chave);
		memcpy (*node->chaves + t->tam_chave * i, promo.chave_promovida, t->tam_chave);

		memcpy (new.filhos, node->filhos + ((size_t)(btree_order/2. + 0.5) - 1), sizeof(int)*(btree_order-1));
		
		memmove (node->filhos + (i + 2), node->filhos + (i + 1), sizeof(int)*(btree_order-1-i));
		memcpy (node->filhos + (i + 1), &promo.filho_direito, sizeof(int));
		
	} else {
		
		memcpy (*new.chaves, *node->chaves + t->tam_chave * (((btree_order-1)/2) + 1), t->tam_chave * (btree_order/2 - 1));
		memmove (*new.chaves + t->tam_chave * (((size_t)(btree_order/2. + 0.5)) - i + 1), *new.chaves + t->tam_chave * (((size_t)(btree_order/2. + 0.5)) - i), t->tam_chave * (((size_t)(btree_order/2. + 0.5)) - i));
		memcpy (*new.chaves + t->tam_chave * (((size_t)(btree_order/2. + 0.5)) - i), promo.chave_promovida, t->tam_chave);
		
		memcpy (new.filhos, node->filhos + ((size_t)(btree_order/2. + 0.5)), sizeof(int)*(btree_order-2));
		memmove (new.filhos + (((size_t)(btree_order/2. + 0.5)) - i + 2), new.filhos + (((size_t)(btree_order/2. + 0.5)) - i+1), sizeof(int)*((btree_order-2) - (((size_t)(btree_order/2. + 0.5)) - i)));
		memcpy (new.filhos + (((size_t)(btree_order/2. + 0.5)) - i+1), &promo.filho_direito, sizeof(int));
		
	}
	
	//set descendents to NULL
	memset (node->filhos + ((size_t)(btree_order/2. + 0.5)), -1, (btree_order-((size_t)(btree_order/2. + 0.5)))*sizeof(int));
	memset (new.filhos + ((size_t)(btree_order/2. + 0.5)), -1, (btree_order-((size_t)(btree_order/2. + 0.5)))*sizeof(int));

	node->qtd_chaves--;
	memcpy (p.chave_promovida, node->chaves[node->qtd_chaves], t->tam_chave);
	memset (node->chaves[node->qtd_chaves], 0, (btree_order-node->qtd_chaves-1)*t->tam_chave);
	
	
	
	
	p.filho_direito = new.this_rrn;
	
	btree_write (new, t);
	btree_write (*node, t);
	
	if (new.filhos)
		free (new.filhos);
	if (*new.chaves)
		free (*new.chaves);
	if (new.chaves)
		free (new.chaves);
	
    return p;
	*/
}



bool btree_insert_aux_file () {
	return false;
}























int b_search (const void *key, const void *base0, size_t nmemb, size_t size, int (*compar)(const void *, const void *)) {
	const void *base = base0;
	int lim, cmp;
	const void *p;

	for (lim = nmemb; lim != 0; lim >>= 1) {
		p = (const char *)base + (lim >> 1) * size;
		cmp = (*compar)(key, p);
		
		if (cmp == 0)
			return (((uint64_t)p - (uint64_t)base0)/size);
		
		if (cmp > 0) {
			base = (const char *)p + size;
			lim--;
		}
	}
	
	return (((uint64_t)p - (uint64_t)base0)/size);
}

int btree_bsearch (void *key, NODE *node, TREE *bt) {
	int pos = b_search (key, node->keys, node->qttKeys, bt->sizeKey, bt->comp);
	if ((pos == node->qttKeys-1 || pos == 0) && bt->comp (key, (char*)node->keys+pos*bt->sizeKey) > 0)
		pos++;
	return pos;
}