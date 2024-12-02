#include <parser/node.h>
#include <debug.h>
#include <utils.h>

#include <stdio.h>
#include <assert.h>


print_node(node)
	Node *node;
{
	assert(node != NULL);

	switch (node->kind) {
		case NumberNk: printf("%.*s", (int) node->number.location.len, node->number.literal); break;
		case IdentifierNk: printf("%.*s", (int) node->identifier.location.len, node->identifier.literal); break;

		case BinaryNk:
			putchar('(');
			print_node(node->binary.left);
			printf(" %s ", token_strkind(node->binary.op));
			print_node(node->binary.right);
			putchar(')');
			break;

		case UnaryNk:
			printf("(%s ", token_strkind(node->unary.op));
			print_node(node->unary.val);
			putchar(')');
			break;

		default: assert(0);
	}

	return 0;
}
