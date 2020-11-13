#ifndef CMD_PARSERS_H
#define CMD_PARSERS_H


typedef enum {
	CMD_PARSER_ERRT_OK = 0,
	CMD_PARSER_ERRT_BINBUF,
	CMD_PARSER_ERRT_LABELDIC,
	CMD_PARSER_ERRT_NARGS,
	CMD_PARSER_ERRT_ARGFMT,
	CMD_PARSER_ERRT_INVLABEL,
} cmd_parser_errt_t;

typedef struct {
	cmd_parser_errt_t type;
	char const* str;
	size_t pos;
} cmd_parser_err_t;

cmd_parser_err_t cmd_parser_make_err(cmd_parser_errt_t errt, char const* str, size_t pos);

typedef cmd_parser_err_t const (*cmd_parser_f) (char const* args[], size_t nargs);

typedef struct {
	char const* name;
	cmd_parser_f parser;
} cmd_parser_t;

extern int CMD_PARSER_PEDANTIC_LABELS;

extern cmd_parser_t CMD_PARSERS[];
extern size_t CMD_PARSERS_COUNT;

cmd_parser_t* const find_cmd_parser(char const* name);

#endif
