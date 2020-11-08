#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <errno.h>

#include <ttrack/log.h>
#include <ttrack/dbg.h>
#include <ttrack/text.h>

#ifdef __GNUC__
#	pragma GCC diagnostic ignored "-Wunused-function"
#endif

#define STACK_DATA_T double
#define STACK_DATA_PRINTF_SEQ "%lf"
#include <ttrack/stack.h>
#undef STACK_DATA_PRINTF_SEQ
#undef STACK_DATA_T

#define PARSER_MAX_TOKENS 4

typedef unsigned long opcode_t;
opcode_t const CMD_IN_OPCODE = 		1;
opcode_t const CMD_OUT_OPCODE = 	2;
opcode_t const CMD_PUSH_OPCODE = 	3;
opcode_t const CMD_POP_OPCODE = 	3;
opcode_t const CMD_ADD_OPCODE = 	4;
opcode_t const CMD_SUB_OPCODE = 	5;
opcode_t const CMD_MUL_OPCODE = 	6;
opcode_t const CMD_DIV_OPCODE = 	7;
opcode_t const CMD_SIN_OPCODE = 	8;
opcode_t const CMD_COS_OPCODE = 	9;
opcode_t const CMD_HALT_OPCODE = 	10;

#define stream_assert(stream) 			\
	do {								\
		assert(stream != NULL);			\
		assert(ferror(stream) == 0);	\
	} while(0)						

typedef enum {
	PARSER_ERR_OK = 0,
	PARSER_ERR_INVARG,

	PARSER_NERRORS
} parser_errc_t;

typedef struct {
	parser_errc_t errc;
	char const* message;
} parser_err_t;

parser_err_t parser_err_init(parser_errc_t const errc, char const* const message) {
	assert(errc >=0 && errc < PARSER_NERRORS);
	
	parser_err_t err = { errc, message };
	return err;
}

typedef parser_err_t const (*argparser_t) (string_t const* const args, FILE* const stream);

#define MAKE_DEFAULT_PARSER(name, opcode)										\
	parser_err_t const name(string_t const* const args, FILE* const stream) {	\
		(void)args;																\
		stream_assert(stream);													\
																				\
		fwrite(&opcode, sizeof(opcode_t), 1, stream);							\
		return parser_err_init(PARSER_ERR_OK, NULL);							\
	}


MAKE_DEFAULT_PARSER(parser_in, CMD_IN_OPCODE);
MAKE_DEFAULT_PARSER(parser_out, CMD_OUT_OPCODE);
MAKE_DEFAULT_PARSER(parser_add, CMD_ADD_OPCODE);
MAKE_DEFAULT_PARSER(parser_sub, CMD_SUB_OPCODE);
MAKE_DEFAULT_PARSER(parser_mul, CMD_MUL_OPCODE);
MAKE_DEFAULT_PARSER(parser_div, CMD_DIV_OPCODE);
MAKE_DEFAULT_PARSER(parser_sin, CMD_SIN_OPCODE);
MAKE_DEFAULT_PARSER(parser_cos, CMD_COS_OPCODE);
MAKE_DEFAULT_PARSER(parser_halt, CMD_HALT_OPCODE);
MAKE_DEFAULT_PARSER(parser_pop, CMD_POP_OPCODE);

parser_err_t const parser_push(string_t const* const args, FILE* const stream) {
	assert(args != NULL);
	stream_assert(stream);

	double v = strtod(args[0].first, NULL);
	if(errno == ERANGE) {
		return parser_err_init(PARSER_ERR_INVARG, "invalid double format");
	}

	fwrite(&CMD_PUSH_OPCODE, sizeof(opcode_t), 1, stream);
	fwrite(&v, sizeof(double), 1, stream);
	return parser_err_init(PARSER_ERR_OK, NULL);
}

typedef struct {
	char const* name;
	size_t nargs;
	argparser_t parser;
} cmdref_t;

cmdref_t const COMMANDS[] = {
	{ "in",   0, parser_in },
	{ "out",  0, parser_out },
	{ "push", 1, parser_push },
	{ "pop",  0, parser_pop },
	{ "add",  0, parser_add },
	{ "sub",  0, parser_sub },
	{ "mul",  0, parser_mul },
	{ "div",  0, parser_div },
	{ "sin",  0, parser_sin },
	{ "cos",  0, parser_cos },
	{ "halt", 0, parser_halt }
};
size_t const NCOMMANDS = sizeof(COMMANDS) / sizeof(COMMANDS[0]);

void remove_comments(string_t* const lines, size_t const nlines) {
	assert(lines != NULL);
	assert(nlines != 0);

	for(string_t* line = lines; line < lines + nlines; ++line) {
		char* pos = memchr(line->first, ';', line->last - line->first);
		if(pos != NULL) {
			*pos = '\0';
			line->last = pos;
		}
	}
}

void chomp_lines(string_t* const lines, size_t const nlines) {
	assert(lines != NULL);

	for(string_t* line = lines; line < lines + nlines; ++line) {
		string_chomp(line);
	}

}

string_t* const split_args(string_t const* const line, size_t* const pnargs) {
	assert(line != NULL);
	assert(pnargs != NULL);

	return get_text_lines(line->first, (size_t)(line->last - line->first), ' ', pnargs);
}

cmdref_t const * const find_command(string_t const* const cmdname) {
	string_assert(cmdname);

	for(cmdref_t const* cmd = COMMANDS; cmd < COMMANDS + NCOMMANDS; ++cmd) {
		if(string_ceq(cmdname, cmd->name))
			return cmd;
	}
	return NULL;
}

int main(int argc, char* argv[]) {
	if(argc != 3) {
		ERR("Invalid arguments. Expected source file name and output file name");
	}

	RF_err_t errc = RF_OK;
	size_t source_size = 0;
	char* source_text = read_text2(argv[1], &source_size, &errc);
	if(source_text == NULL) {
		ERR(RF_errstr(errc));
	}

	size_t nlines = 0;
	string_t* lines = get_text_lines(source_text, source_size, '\n', &nlines);
	if(lines == NULL) {
		free(source_text);
		ERR("Out of memory");
	}

	remove_comments(lines, nlines);
	chomp_lines(lines, nlines);

	FILE* ofile = fopen(argv[2], "wb");
	if(ofile == NULL) {
		free(source_text);
		free(lines);
		ERR("Invalid output file");
	}

	size_t nline = 1;
	for(string_t const* line = lines; line < lines + nlines; ++line, ++nline) {
		if(string_length(line) == 0) {
			continue;
		}

		string_t toks[PARSER_MAX_TOKENS];
		size_t ntoks = string_tok(line, toks, PARSER_MAX_TOKENS, ' ');

		cmdref_t const* cmd = find_command(&toks[0]);

		if(cmd == NULL) {
			ERR("Unknown command at line %zu of %s", nline, argv[1]);
		}
		if(cmd->nargs != ntoks - 1) {
			ERR("Invalid arguments count (%zu) line %zu of %s (%zu expected)\n", 
				ntoks - 1, nline, argv[1], cmd->nargs);
		}

		parser_err_t err = cmd->parser(toks + 1, ofile);
		if(err.errc != PARSER_ERR_OK) {
			ERR("Parser error at line %zu (%s)", nline, err.message);
		}
	}

	fclose(ofile);
	free(lines);
	free(source_text);

}
