#include <lexer/lexer.h>
#include <err.h>
#include <str.h>
#include <utils.h>
#include <hashtable.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define IS_NAME(a) ('A' <= a && a <= 'Z') || a == '$'
#define IS_DIGIT(a) ('0' <= a && a <= '9')

#define CURRENT self->current
#define NEXT if ((err = self->next(self))) return err
#define APPEND if ((err = buf.append(&buf, CURRENT))) return err


static HashTable literals;

static KeywordInfo keyword_infos[] = {
#define T(kind)
#define KW(kind) {kind ## Kw, #kind},
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

	if (self->pos == 0) {
		NEXT;
		self->pos = 0;
		ERR(hashtable_init(&literals));
	}

	while (
		CURRENT == ' ' ||
		CURRENT == '\t' ||
		CURRENT == '\n'
	) NEXT;

	if (CURRENT == '%' || CURRENT == '"') {
		char pair = CURRENT;
		NEXT;

		while (CURRENT != pair) NEXT;
		NEXT;
	}

	while (
		CURRENT == ' ' ||
		CURRENT == '\t' ||
		CURRENT == '\n'
	) NEXT;

	TokenKind kind = EOITk;
	long pos = self->pos;
	long len;
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
		case '@': NEXT; kind = AtTk; break;
		case '.': NEXT; kind = DotTk; break;
		case ';': NEXT; kind = SemiTk; break;
		case '(': NEXT; kind = OpenParenTk; break;
		case ')': NEXT; kind = CloseParenTk; break;
		case '=': NEXT; kind = EqualsTk; break;
		case '!': NEXT; kind = ArrowTk; break;

		case '<': {
			kind = LessThanTk;
			NEXT;

			if (CURRENT == '>') {
				kind = DiffFromTk;
				NEXT;
			}
			else if (CURRENT == '=') {
				kind = LssEqualsTk;
				NEXT;
			}
		} break;

		case '>': {
			kind = GreaterThanTk;
			NEXT;

			if (CURRENT == '=') {
				kind = GtrEqualsTk;
				NEXT;
			}
		} break;

		case '\'': {
			kind = CharTk;
			NEXT;

			StrBuilder buf;
			ERR(strbuilder_create(NULL, 0, &buf));

			/* Done this way to support '' in the
			 * beginning and ending of literals
			 */
			while (1) {
				char last = CURRENT;
				NEXT;

				if (last == '\'') {
					if (CURRENT == '\'') {
						APPEND;
						NEXT;
					}
					else break;
				}
				else {
					buf.append(&buf, last);
				}
			}

			/* Convert buffer to string view */
			if (buf.cap) {
				StrView view;

				ERR(buf.extract_view(&buf, &view));
				literal = view.data;
			}
		 } break;

		default: {
			StrBuilder buf;

			ERR(strbuilder_create(NULL, 0, &buf));

			/* int		123
			 * float	decimal number
			 * bit    bead-size B ' bead ... '
			 */
			if (IS_DIGIT(CURRENT)) {
				kind = IntTk;

				/* Get the integer part */
				while (IS_DIGIT(CURRENT)) {
					APPEND;
					NEXT;
				}

				/* Process floating point */
				if (CURRENT == '.') {
					kind = FloatTk;
					APPEND;
					NEXT;

					while (IS_DIGIT(CURRENT)) {
						APPEND;
						NEXT;
					}
				}
	
				/* Process bit literals */
				else if (CURRENT == 'B') {
					kind = BitTk;
					NEXT;

					/* Check bead validity */
					if (buf.size > 1 || (buf.data[0] - '0') > 5) {
						fprintf(stderr, "Position %ld: Invalid bit literals' bead, max bead is 5\n", pos);
						return 1;
					}

					token->bead = buf.data[0];
					buf.free(&buf);

					/* Check literals beginning */
					if (CURRENT != '\'') {
						fprintf(stderr, "Position %ld: Expected single quote for bit literals\n", self->pos);
						return 1;
					}

					NEXT;

					/* Take character literals part */
					while (CURRENT != '\'' && (IS_DIGIT(CURRENT) || ('A' <= CURRENT && CURRENT <= 'V'))) {
						APPEND;
						NEXT;
					}

					if (CURRENT == '\'') { NEXT; }
					else {
						fprintf(stderr, "Position %ld: Unxpected character inside bit literals\n", self->pos);
						return 1;
					}
				}
			}

			/* [A-Z$][A-Z0-1$']* */
			else if (IS_NAME(CURRENT)) {
				kind = IdentifierTk;

				while (IS_NAME(CURRENT) || IS_DIGIT(CURRENT) || CURRENT == '\'') {
					APPEND;
					NEXT;
				}

				/* Keywords */
				int i;

				for (i = 0; keyword_infos[i].kind != 0 && kind == IdentifierTk; ++i)
					if (strcmp(keyword_infos[i].image, buf.data) == 0)
						kind = keyword_infos[i].kind;

				/* If it's a keyword, no need to store it's literal */
				if (kind != IdentifierTk)
					buf.free(&buf);
			}

			/* Just skip Unknown */
			else {
				fprintf(stderr, "Position %ld: Unknown character '%c'\n", pos, CURRENT);
				return 1;
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

	len = self->pos - pos;

	if (literal) {
		int (*cmp)(const void *, const void *) = (int (*)(const void *, const void *)) strcmp;
		char *nliteral;

		ERR(hashtable_append(literals, literal, literal, len, cmp, &nliteral));

		if (nliteral != literal) {
			free(literal);
			literal = nliteral;
		}
	}

	token->kind = kind;
	token->literal = literal;
	token->location = (Location) { .pos = pos, .end = self->pos, .len = len };
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
