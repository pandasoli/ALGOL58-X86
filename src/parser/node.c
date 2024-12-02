#include <parser/node.h>
#include <utils.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


/* It probably doesn't make much sense to use a label
   to make recursion faster in this context as frees
   within switch need to get back to inside of it
   and the last free will probably exit the this
   function before calling free_node */

free_node(node)
	Node *node;
{
	assert(node != NULL);

	switch (node->kind) {
		case NumberNk: free((char *) node->number.literal); break;
		case IdentifierNk: free(((char *) node->identifier.literal)); break;
		case BinaryNk: free_node(node->binary.left); free_node(node->binary.right); break;
		case UnaryNk: free_node(node->unary.val); break;
		default: assert(0);
	}

	Node *next = node->next;
	free(node);
	if (next) return free_node(next);
	return 0;
}

heap_node(dst, src)
	void **dst;
	Node *src;
{
	*dst = malloc(sizeof *src);
	if (*dst == NULL) {
		fprintf(stderr, "Couldn't malloc(%ld) on %s\n", sizeof *src, __func__);
		return 1;
	}

	memcpy(*dst, src, sizeof *src);

	return 0;
}
