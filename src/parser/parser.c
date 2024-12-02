#include <parser/parser.h>
#include <utils.h>
#include <err.h>

#include <stdio.h>
#include <assert.h>


#define CURRENT self->current
#define NEXT ERR(self->next(self->next_props, &CURRENT))

/* A short for the toppest expr */
#define expr add


static add();
static stmt();


static primary(self, node)
	Parser *self;
	Node **node;
{
	switch (CURRENT.kind) {
		case FloatTk:
		case IntTk: {
			Node n = {.kind = NumberNk, .number = CURRENT, .next = NULL};
			ERR(heap_node(node, &n));
			NEXT;
		} break;
		
		case IdentifierTk: {
			Node n = {.kind = IdentifierNk, .identifier = CURRENT, .next = NULL};
			ERR(heap_node(node, &n));
			NEXT;
		} break;

		case OpenParenTk:
			NEXT;

			ERR(expr(self, node));

			if (CURRENT.kind != CloseParenTk) {
				fprintf(stderr, "Position %ld: Expected closing parenthesis when found %s\n", CURRENT.location.pos, token_strkind(CURRENT.kind));
				return 2;
			}

			NEXT;
			break;

		default:
			fprintf(stderr, "Position %ld: Unexpected %s token found parsing primary\n", CURRENT.location.pos, token_strkind(CURRENT.kind));
			return 2;
	}

	return 0;
}

static unary(self, node)
	Parser *self;
	Node **node;
{
	if (CURRENT.kind == DashTk) {
		Node n = {.kind = UnaryNk, .unary = {.op = CURRENT.kind}, .next = NULL};
		NEXT;

		ERR(primary(self, &n.unary.val));
		ERR(heap_node(node, &n));
	}
	else
		return primary(self, node);

	return 0;
}

static mul(self, node)
	Parser *self;
	Node **node;
{
	ERR(unary(self, node));

	while (CURRENT.kind == StarTk || CURRENT.kind == SlashTk) {
 		Node n = {.kind = BinaryNk, .binary = {.left = *node, .op = CURRENT.kind}, .next = NULL};
		NEXT;

		ERR(unary(self, &n.binary.right));
		ERR(heap_node(node, &n));
	}

	return 0;
}

static add(self, node)
	Parser *self;
	Node **node;
{
	ERR(mul(self, node));

	while (CURRENT.kind == PlusTk || CURRENT.kind == DashTk) {
		Node n = {.kind = BinaryNk, .binary = {.left = *node, .op = CURRENT.kind}, .next = NULL};
		NEXT;

		ERR(mul(self, &n.binary.right));
		ERR(heap_node(node, &n));
	}

	return 0;
}

static cmp(self, node)
	Parser *self;
	Node **node;
{
	ERR(add(self, node));

	/*
	 * TODO: maybe make an aux function for
	 * binary expression functions.
	 */

	while (
		CURRENT.kind == EqualsTk || CURRENT.kind == DiffFromTk ||
		CURRENT.kind == LessThanTk || CURRENT.kind == GreaterThanTk ||
		CURRENT.kind == LssEqualsTk || CURRENT.kind == GtrEqualsTk
	) {
		Node n = {.kind = BinaryNk, .binary = {.left = *node, .op = CURRENT.kind}, .next = NULL};
		NEXT;

		ERR(add(self, &n.binary.right));
		ERR(heap_node(node, &n));
	}

	return 0;
}

static logic(self, node)
	Parser *self;
	Node **node;
{
	ERR(cmp(self, node));

	while (CURRENT.kind == ANDKw || CURRENT.kind == ORKw) {
		Node n = {.kind = BinaryNk, .binary = {.left = *node, .op = CURRENT.kind}, .next = NULL};
		NEXT;

		ERR(add(self, &n.binary.right));
		ERR(heap_node(node, &n));
	}

	return 0;
}

static dir(self, node)
	Parser *self;
	Node **node;
{
	return 0;
}

static stmt(self, node)
	Parser *self;
	Node **node;
{
	/* TODO: Create a label for each one */

	switch (CURRENT.kind) {
		case BEGINKw: {
			NEXT;
			ERR(stmt(self, node));

			if (CURRENT.kind != ENDKw) {
				fprintf(stderr, "Position %ld: Expected end of compound statement\n", CURRENT.location.pos);
				return 1;
			}
			NEXT;
		} break;

		case WHILEKw: {
			Node n = {.kind = WHILENk, .next = NULL};
			NEXT;

			ERR(logic(self, &n.while_.cmp));

			if (CURRENT.kind != SemiTk) {
				fprintf(stderr, "Position %ld: Expected semicolon after expression\n", CURRENT.location.pos);
				return 1;
			}
			NEXT;

			ERR(stmt(self, &n.while_.body));
			ERR(heap_node(node, &n));
		} break;

		default:
			fprintf(stderr, "Position %ld: Expected statement, found %s\n", CURRENT.location.pos, token_strkind(CURRENT.kind));
			return 1;
	}

	return 0;
}

static parse(self, node)
	Parser *self;
	Node **node;
{
	assert(self != NULL);
	assert(node != NULL);

	NEXT;

	Node *last_node = NULL;
	*node = NULL;

	while (CURRENT.kind != EOITk) {
		Node *new;

		ERR(stmt(self, &new));

		/* Linked list append */
		last_node = *node == NULL
			? (*node = new)
			: (last_node->next = new);
	}

	return 0;
}

parser_init(next, next_props, parser)
	NextFn next;
	void *next_props;
	Parser *parser;
{
	assert(next != NULL);
	assert(parser != NULL);

	parser->current = (Token) {};
	parser->next = next;
	parser->next_props = next_props;

	parser->parse = &parse;

	return 0;
}
