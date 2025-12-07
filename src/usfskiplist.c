#include "usfskiplist.h"

usf_skiplist *usf_newsk(void) {
	/* Allocate new skiplist */
	usf_skiplist *skiplist;
	skiplist = calloc(1, sizeof(usf_skiplist));

	return skiplist;
}

usf_skiplist *usf_skset(usf_skiplist *skiplist, uint64_t i, usf_data data) {
	/* Insert element at index i in the skiplist */
	if (skiplist == NULL) return NULL; /* Nonexistent */

	int32_t level;
	usf_skipnode **skiplinks[USF_SKIPLIST_FRAMESIZE], **skipframe, *node;

	for (skipframe = skiplist->base, level = USF_SKIPLIST_FRAMESIZE - 1; level >= 0; level--) {
		while ((node = skipframe[level])) {
			if (node->index > i) break; /* Overshot */
			if (node->index == i) { /* Found; only modify node */
				node->data = data;
				return skiplist;
			}
			skipframe = node->nextnodes;
		}
		skiplinks[level] = &skipframe[level];
	}

	node = calloc(1, sizeof(usf_skipnode)); /* Skipframe zeroed (NULL) by default */
	node->data = data; node->index = i;
	for (level = 0; level < USF_SKIPLIST_FRAMESIZE; level++) {
		node->nextnodes[level] = *skiplinks[level]; /* Link this with next */
		*skiplinks[level] = node; /* Link prev with this */

		if (rand() & 1) break; /* Probabilistic upkeep */
	}

	skiplist->size++;
	return skiplist;
}

usf_data usf_skget(usf_skiplist *skiplist, uint64_t i) {
	/* Get value at index i in skiplist; returns USFNULL if it doesn't exist */
	if (skiplist == NULL) return USFNULL;

	int32_t level;
	usf_skipnode **skipframe, *node;
	for (skipframe = skiplist->base, level = USF_SKIPLIST_FRAMESIZE - 1; level >= 0; level--) {
		while ((node = skipframe[level])) {
			if (node->index > i) break; /* Overshot */
			if (node->index == i) return node->data; /* Found */
			skipframe = node->nextnodes; /* Continue */
		}
	}

	return USFNULL; /* Did not find */
}

usf_data usf_skdel(usf_skiplist *skiplist, uint64_t i) {
	/* Same as usf_skget but delete and unlink node if found */
	if (skiplist == NULL) return USFNULL;

	int32_t level;
	usf_data data;
	usf_skipnode **skipframe, *node;
	for (skipframe = skiplist->base, level = USF_SKIPLIST_FRAMESIZE - 1; level >= 0; level--) {
		while ((node = skipframe[level])) {
			if (node->index > i) break; /* Overshot */
			if (node->index == i) { /* Unlink */
				skipframe[level] = node->nextnodes[level];
				break;
			}
			skipframe = node->nextnodes; /* Continue */
		}
	}

	if (node && node->index == i) { /* Found */
		data = node->data;
		free(node);
		return data;
	}

	return USFNULL; /* Did not find */
}

void usf_freesk(usf_skiplist *skiplist) {
	/* Frees a skiplist */
	if (skiplist == NULL) return;

	usf_skipnode *node, *prev;
	node = skiplist->base[0];

	while (node) {
		prev = node;
		node = prev->nextnodes[0]; /* Get next then free the one we were on */
		free(prev);
	}

	free(skiplist);
}

void usf_freeskptr(usf_skiplist *skiplist) {
	/* Frees a skiplist and its contents */
	if (skiplist == NULL) return;

	usf_skipnode *node, *prev;
	node = skiplist->base[0];

	while (node) {
		prev = node;
		node = prev->nextnodes[0]; /* Get next then free the one we were on */
		free(prev->data.p); /* Free contents */
		free(prev);
	}

	free(skiplist);
}
