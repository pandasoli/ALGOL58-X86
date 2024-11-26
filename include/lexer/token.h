#ifndef TOKEN_H
#define TOKEN_H

#include <location.h>

#include <stddef.h>

#define TOKEN_KINDS_LIST(T, KW) \
	T(Unknown)                    \
	T(EOI)                        \
                                \
	/* Symbols */                 \
	T(Plus) T(Dash)               \
	T(Star) T(Slash)              \
	T(Comma) T(Colon)             \
	T(OpenParen) T(CloseParen)    \
	T(Equals)                     \
                                \
	T(LessThan) T(GreaterThan)    \
	T(DiffFrom)                   \
                                \
	/* Literals */                \
	T(Identifier)                 \
	T(Int) T(Float)               \
                                \
	/* Keywords */


typedef enum TokenKind TokenKind;
enum TokenKind {
#define T(kind, ...) kind ## Tk,
#define KW(kind, ...) kind ## Kw,
	TOKEN_KINDS_LIST(T, KW)
#undef T
#undef KW
};

typedef struct Token Token;
struct Token {
	TokenKind kind;
	const char *literal;
	Location location;
};

/* TokenKind kind, const char *literal, long pos, long len */
Token token_create();

#endif
