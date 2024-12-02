#ifndef NODE_H
#define NODE_H

#include <lexer/token.h> /* single outside dependency */

#define NODE_KINDS_LIST(N) \
	N(Number)                \
	N(Identifier)            \
	N(Binary)                \
	N(Unary)                 \
                           \
	/* Statements */         \
	N(WHILE)


typedef enum NodeKind NodeKind;
enum NodeKind {
#define N(kind) kind ## Nk,
	NODE_KINDS_LIST(N)
#undef N
};

typedef struct Node Node;

typedef Token NumberNode;
typedef Token IdentifierNode;

typedef struct BinaryNode BinaryNode;
struct BinaryNode {
	Node *left;
	TokenKind op;
	Node *right;
};

typedef struct UnaryNode UnaryNode;
struct UnaryNode {
	TokenKind op;
	Node *val;
};

typedef struct WHILENode WHILENode;
struct WHILENode {
	Node *cmp;
	Node *body;
};

struct Node {
	NodeKind kind;

	union {
		BinaryNode binary;
		UnaryNode unary;
		NumberNode number;
		IdentifierNode identifier;

		/* Statements */
		WHILENode while_;
	};

	Node *next;
};

/* Node *node */
int free_node();

/* void **dst, *src */
int heap_node();

#endif
