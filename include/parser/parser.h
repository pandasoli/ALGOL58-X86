#ifndef PARSER_H
#define PARSER_H

#include <lexer/token.h> /* single outside dependency */
#include <parser/node.h>

typedef int (*NextFn)(void *props, Token *token);

typedef struct Parser Parser;
struct Parser {
	Token current;
	NextFn next;
	void *next_props;

	int (*parse)(Parser *self, Node **node);
};

/* NextFn next, void *next_props, Parser *parser */
int parser_init();

#endif
