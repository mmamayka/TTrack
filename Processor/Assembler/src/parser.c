#include <ttrack/dbg.h>
#include <ttrack/binbuf.h>
#include <libcommon/labeldic.h>
#include <string.h>
#include <stdio.h>

#include "parser.h"
#include "tokenizer.h"
#include "cmdparsers.h"

static int const format_if_label(char* str) 
{$_
	ASSERT(str != NULL);

	size_t l = strlen(str);
	if(str[l - 1] == ':') {
		str[l - 1] = '\0';
		RETURN(1);
	}
	RETURN(0);
}

static void parse_print_error(cmd_parser_err_t err)
{$_
	switch(err.type) {
	case CMD_PARSER_ERRT_OK:
		break;

	case CMD_PARSER_ERRT_NARGS:
		fprintf(stderr, "[ERROR] Invalid number of arguments at line %zu\n",
				tokenizer_nline());
		break;

	case CMD_PARSER_ERRT_ARGFMT:
		fprintf(stderr, "[ERROR] Invalid argument format at line %zu (%s)\n",
				tokenizer_nline(), err.str);
		break;

	case CMD_PARSER_ERRT_LABELDIC:
		fprintf(stderr, "[ERROR] LABELDIC TODO\n");
		break;

	case CMD_PARSER_ERRT_BINBUF:
		fprintf(stderr, "[ERROR] Binbary buffer terminated with error %s\n",
				binbuf_errstr(binbuf_error()));
		break;
	case CMD_PARSER_ERRT_INVLABEL:
		fprintf(stderr, "[ERROR] Invalid label name at line %zu\n",
				tokenizer_nline());
		break;
	}
$$
}

static int const parse() 
{$_
	tokenizer_err_t terrc;
	char* tokens[PARSER_MAX_TOKENS];
	size_t ntokens;
	while((terrc = tokenizer_nextline(PARSER_MAX_TOKENS, tokens, &ntokens)) 
				== TOKENIZER_ERR_OK)
	{
		char* const* passtokens = tokens;

		if(ntokens == 0) { continue; }

		if(format_if_label(passtokens[0])) {
			labeldic_setaddr(passtokens[0], binbuf_pos());
			++passtokens;
			--ntokens;
		}

		if(ntokens == 0) { continue; }

		cmd_parser_t* cmd_parser = find_cmd_parser(passtokens[0]);
		if(cmd_parser == NULL) {
			fprintf(stderr, "[ERROR] Invalid command \'%s\' at line %zu\n", 
					passtokens[0], tokenizer_nline());
			RETURN(0);
		}
		cmd_parser_err_t cmderr = cmd_parser->parser(passtokens + 1, ntokens - 1);
		if(cmderr.type != CMD_PARSER_ERRT_OK) {
			parse_print_error(cmderr);
			RETURN(0);
		}
	}
	RETURN(1);
}

int const parser_pass(char const* fname, int pedantic)
{$_
	ASSERT(fname != NULL);

	FILE* ifile = fopen(fname, "r");
	if(ifile == NULL) {
		fprintf(stderr, "[ERROR] Input file \'%s\' not found\n", fname);
		RETURN(0);
	}
	tokenizer_err_t tokerr = tokenizer_init(ifile);
	fclose(ifile);

	if(tokerr != TOKENIZER_ERR_OK) {
		fprintf(stderr, "Fatal error occured while reading file \'%s\': %s\n",
				fname, tokenizer_errstr(tokerr));
		tokenizer_free();
		RETURN(0);
	}

	CMD_PARSER_PEDANTIC_LABELS = pedantic;

	if(!parse()) {
		tokenizer_free();
		RETURN(0);
	}

	tokenizer_free();
	RETURN(1);
}

