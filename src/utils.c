#include <lexer/token.h>
#include <parser/parser.h>
#include <utils.h>


char *token_strkind(kind)
	TokenKind kind;
{
	switch (kind) {
#define X(kind)  case kind ## Tk: return #kind;
#define KW(kind) case kind ## Kw: return #kind;
	TOKEN_KINDS_LIST(X, KW)
#undef X
#undef KW
		default: return NULL;
	}
}

char *node_strkind(kind)
	NodeKind kind;
{
	switch (kind) {
#define X(kind) case kind ## Nk: return #kind;
	NODE_KINDS_LIST(X)
#undef X
		default: return NULL;
	}
}
