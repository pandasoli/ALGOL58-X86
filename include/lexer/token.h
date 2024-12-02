#ifndef TOKEN_H
#define TOKEN_H

#include <location.h>

#define TOKEN_KINDS_LIST(T, KW) \
	T(EOI)                        \
                                \
	/* Symbols */                 \
	T(Plus) T(Dash)               \
	T(Star) T(Slash)              \
	T(Comma) T(Colon)             \
	T(At) T(Dot) T(Semi)          \
	T(OpenParen) T(CloseParen)    \
	T(Equals) T(Arrow)            \
                                \
	T(LssEquals) T(GtrEquals)     \
	T(LessThan) T(GreaterThan)    \
	T(DiffFrom)                   \
                                \
	/* Literals */                \
	T(Identifier)                 \
	T(Int) T(Float)               \
	T(Char) T(Bit)                \
                                \
	/* Keywords */                                 \
	KW(ABORT) KW(ABS) KW(AND) KW(BEGIN) KW(BIT)    \
	KW(BITSZE) KW(BLOCK) KW(BY) KW(BYTE)           \
	KW(BYTESIZE) KW(CASE) KW(COMPOOL)              \
	KW(COSTANT) KW(DEF) KW(DEFAULT) KW(DEFINE)     \
	KW(ELSE) KW(END) KW(EQV) KW(EXIT) KW(FALLTHRU) \
	KW(FALSE) KW(FIRST) KW(FOR) KW(GOTO) KW(IF)    \
	KW(INLINE) KW(INSTANCE) KW(ITEM) KW(LABEL)     \
	KW(LAST) KW(LBOUND) KW(LIKE) KW(LOC) KW(MOD)   \
	KW(NEXT) KW(NOT) KW(NULL) KW(NWDSEN) KW(OR)    \
	KW(OVERLAY) KW(PARALLEL) KW(POS) KW(PROC)      \
	KW(PROGRAM) KW(REC) KW(REF) KW(RENT) KW(REP)   \
	KW(RETURN) KW(SGN) KW(SHIFTL) KW(SHIFTR)       \
	KW(START) KW(STATIC) KW(STATUS) KW(STOP)       \
	KW(TABLE) KW(TERM) KW(THEN) KW(TRUE) KW(TYPE)  \
	KW(UBOUND) KW(WHILE) KW(WORDSIZE) KW(XOR)


typedef enum TokenKind TokenKind;
enum TokenKind {
#define T(kind)  kind ## Tk,
#define KW(kind) kind ## Kw,
	TOKEN_KINDS_LIST(T, KW)
#undef T
#undef KW
};

typedef struct Token Token;
struct Token {
	TokenKind kind;
	const char *literal;
	Location location;

	union {
		char bead; /* For bit literals */
	};
};

/* TokenKind kind, const char *literal, long pos, long len */
Token token_create();

#endif
