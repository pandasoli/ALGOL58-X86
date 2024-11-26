#include <lexer/lexer.h>
#include <err.h>
#include <str.h>

#include <stdio.h>
#include <string.h>
#include <assert.h>

#define IS_ID(a) ('a' <= a && a <= 'z') || ('A' <= a && a <= 'Z') || a == '_'
#define IS_DEC(a) ('0' <= a && a <= '9')

#define CURRENT self->current
#define NEXT ERR(self->next(self))
#define APPEND ERR(buf.append(&buf, CURRENT))


static KeywordInfo keyword_infos[] = {
#define T(kind)
#define KW(kind, image) {kind ## Kw, image},
	TOKEN_KINDS_LIST(T, KW)
#undef T
#undef KW
	{0, NULL}
};


static lex(self, token)
	Lexer *self;
	Token *token;
{
	assert(self != NULL);
	assert(token != NULL);

	int err;

	if (self->pos == 0) {
		NEXT;
		self->pos = 0;
	}

	while (
		CURRENT == ' ' ||
		CURRENT == '\t' ||
		CURRENT == '\n'
	) NEXT;

	TokenKind kind = UnknownTk;
	long pos = self->pos;
	char *literal = NULL;

	switch (CURRENT) {
		case   0: NEXT; kind = EOITk; break;

		/* Symbols */
		case '+': NEXT; kind = PlusTk; break;
		case '-': NEXT; kind = DashTk; break;
		case '*': NEXT; kind = StarTk; break;
		case '/': NEXT; kind = SlashTk; break;
		case ',': NEXT; kind = CommaTk; break;
		case ':': NEXT; kind = ColonTk; break;
		case '(': NEXT; kind = OpenParenTk; break;
		case ')': NEXT; kind = CloseParenTk; break;
		case '=': NEXT; kind = EqualsTk; break;

		case '<': {
			kind = LessThanTk;
			NEXT;

			if (CURRENT == '>') {
				kind = DiffFromTk;
				NEXT;
			}
		} break;
		case '>': NEXT; kind = GreaterThanTk; break;

		default: {
			StrBuilder buf;

			ERR(strbuilder_create(NULL, 0, &buf));

			/* Numbers support:
			 * int		123
			 * float	123.456
			 */
			if (IS_DEC(CURRENT)) {
				kind = IntTk;

				/* Get the integer part */
				while (IS_DEC(CURRENT)) {
					APPEND;
					NEXT;
				}

				/* Identify first dot within int */
				if (CURRENT == '.') {
					kind = FloatTk;
					APPEND;
					NEXT;

					while (IS_DEC(CURRENT)) {
						APPEND;
						NEXT;
					}
				}
			}

			/* [a-zA-Z_][a-zA-Z_0-1]* */
			else if (IS_ID(CURRENT)) {
				kind = IdentifierTk;

				while (IS_ID(CURRENT) || IS_DEC(CURRENT)) {
					APPEND;
					NEXT;
				}

				/* Keywords */
				int i;

				for (i = 0; keyword_infos[i].kind != 0 && kind == IdentifierTk; i++)
					if (strcmp(keyword_infos[i].image, buf.data) == 0)
						kind = keyword_infos[i].kind;

				/* If it's a keyword, no need to store it's literal */
				if (kind != IdentifierTk)
					buf.free(&buf);
			}

			/* Just skip Unknown */
			else {
				APPEND;
				NEXT;
			}

			/* Convert buffer to string view */
			if (buf.cap) {
				StrView view;

				ERR(buf.extract_view(&buf, &view));
				literal = view.data;
			}

			buf.free(&buf);
		}
	}	

	*token = token_create(kind, literal, pos, self->pos - pos);
	return 0;
}

static next(self)
	Lexer *self;
{
	assert(self != NULL);

	int ret = read(self->fd, &self->current, 1);

	if (ret == -1) {
		printf("read(%d, %p) returned %d on lex.next", self->fd, &self->current, ret);
		return 1;
	}
	else if (ret == 0)
		self->current = '\0';

	self->pos++;

	return 0;
}

lexer_init(fd, lexer)
	int fd;
	Lexer *lexer;
{
	assert(fd > 0);
	assert(lexer != NULL);

	lexer->fd = fd;
	lexer->current = '\0';
	lexer->pos = 0;

	lexer->lex = &lex;
	lexer->next = &next;

	return 0;
}
