#include <lexer/token.h>
#include <utils.h>


char *token_strkind(kind)
	TokenKind kind;
{
	switch (kind) {
#define X(kind)       case kind ## Tk: return #kind;
#define KW(kind, ...) case kind ## Kw: return #kind;
	TOKEN_KINDS_LIST(X, KW)
#undef X
#undef KW
		default: return NULL;
	}
}
