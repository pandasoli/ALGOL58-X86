#include <lexer/lexer.h>
#include <parser/parser.h>
#include <utils.h>
#include <hashtable.h>

#include <stdio.h>
#include <string.h>
#include <sys/resource.h>


int err;


main() {
	char source[] =
		"WHILE SOME'THING < 3; % smth %\n"
			"BEGIN\n"
			"SOME'THING = SOME'THING * 100;\n"
			"END\n"
		;

	int pipefd[2];
	pipe(pipefd);

	write(pipefd[1], source, strlen(source));
	close(pipefd[1]);

	Lexer lex;
	lexer_init(pipefd[0], &lex);

	/*Token token;*/

	/*do {*/
	/*	err = lex.lex(&lex, &token);*/
	/*	if (err) break;*/
	/*	printf("%s '%.*s'\n", token_strkind(token.kind), (int) token.location.len, token.literal);*/
	/*}*/
	/*while (err == 0 && token.kind != EOITk);*/

	Parser par;
	parser_init(lex.lex, &lex, &par);

	Node *ast = {0};
	err = par.parse(&par, &ast);

	printf("err %d\n", err);
	if (ast) {
		print_node(ast);
		free_node(ast);
	}
}
