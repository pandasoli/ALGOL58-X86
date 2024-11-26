#ifndef LEXER_H
#define LEXER_H

#include <lexer/token.h>


typedef struct Lexer Lexer;
struct Lexer {
	int fd;
	char current;
	long pos;

	int (*lex)(Lexer *self, Token *token);
	int (*next)(Lexer *self);
};

typedef struct {
	TokenKind kind;
	const char *image;
} KeywordInfo;

/* int fd, Lexer *lexer */
int lexer_init();

#endif
