#include <lexer/lexer.h>
#include <utils.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>

#define MEM_ENV_VAR "MAXMEM_KB"


void setmemlimit() {
	if(getenv(MEM_ENV_VAR) == NULL) return;

	struct rlimit memlimit;

	long bytes = atol(getenv(MEM_ENV_VAR)) * 1024;
	memlimit.rlim_cur = bytes;
	memlimit.rlim_max = bytes;

	if (setrlimit(RLIMIT_AS, &memlimit) == -1) {
		perror("setrlimit failed");
		exit(1);
	}
}

main() {
	setmemlimit();

	char source[] = "(1 + 2) * 3.0";

	int pipefd[2];
	pipe(pipefd);

	write(pipefd[1], source, strlen(source));
	close(pipefd[1]);

	Lexer lex;
	lexer_init(pipefd[0], &lex);

	Token token;
	int err;

	do {
		if ((err = lex.lex(&lex, &token)))
			break;

		printf("%s %.*s\n", token_strkind(token.kind), (int) token.location.len, token.literal);
		free((char *) token.literal);
	}
	while (token.kind != EOITk);
}
