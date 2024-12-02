#include <lexer/lexer.h>
#include <location.h>


Token token_create(kind, literal, pos, len)
    TokenKind kind;
    const char *literal;
    long pos, len;
{
    return (Token) {
			.kind = kind,
			.literal = literal,
			.location = location_create(pos, len)
		};
}
