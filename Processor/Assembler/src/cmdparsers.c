#include <ttrack/dbg.h>
#include <ttrack/binbuf.h>
#include <libcommon/opcodes.h>
#include <libcommon/reginfo.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include <stdlib.h>

#include "labeldic.h"
#include "cmdparsers.h"

cmd_parser_err_t cmd_parser_make_err(cmd_parser_errt_t errt, char const* str, size_t pos)
{$_
	cmd_parser_err_t err = { errt, str, pos };
	RETURN(err);
$$
}

int CMD_PARSER_PEDANTIC_LABELS = 0;

#define GEN_TRIVIAL_PARSER(name, opcode) 								\
	cmd_parser_err_t const name (char const* args[], size_t nargs) 		\
	{$_																	\
		(void)args;														\
		opcode_t opc = opcode;											\
		if(nargs > 0) {													\
			RETURN(cmd_parser_make_err(CMD_PARSER_ERRT_NARGS, NULL, 0));\
		}																\
		if(binbuf_write_value(opcode_t, opc) != BINBUF_ERR_OK) {		\
			RETURN(cmd_parser_make_err(CMD_PARSER_ERRT_BINBUF, NULL, 0));\
		}																\
		RETURN(cmd_parser_make_err(CMD_PARSER_ERRT_OK, NULL, 0));		\
	$$																	\
	}

#define GEN_LABELED_PARSER(name, opcode)								\
	cmd_parser_err_t const name (char const* args[], size_t nargs)		\
	{$_																	\
		ASSERT(args != NULL);											\
																		\
		opcode_t opc = opcode;											\
																		\
		if(nargs != 1) {												\
			RETURN(cmd_parser_make_err(CMD_PARSER_ERRT_NARGS, NULL, 0));\
		}																\
		size_t addr = 0;												\
		if(labeldic_getaddr(args[0], &addr) != LABELDIC_ERR_OK) {		\
			RETURN(cmd_parser_make_err(CMD_PARSER_ERRT_LABELDIC, NULL, 0));\
		}																\
		if(addr == LABELDIC_POISON && CMD_PARSER_PEDANTIC_LABELS) {		\
			RETURN(cmd_parser_make_err(CMD_PARSER_ERRT_INVLABEL, NULL, 0));\
		}																\
		offset_t gaddr = (offset_t)addr;								\
		if(binbuf_write_value(opcode_t, opc) != BINBUF_ERR_OK ||		\
		   binbuf_write_value(offset_t, gaddr) != BINBUF_ERR_OK) {		\
			RETURN(cmd_parser_make_err(CMD_PARSER_ERRT_BINBUF, NULL, 0));\
		}																\
		RETURN(cmd_parser_make_err(CMD_PARSER_ERRT_OK, NULL, 0));		\
	}

cmd_parser_err_t const cmd_push_parser(char const* args[], size_t nargs)
{$_
	ASSERT(args != NULL);

	if(nargs != 1) {
		RETURN(cmd_parser_make_err(CMD_PARSER_ERRT_NARGS, NULL, 0));
	}

	regid_t regid = get_regid(args[0]);
	if(regid != UCHAR_MAX) {
		opcode_t opc = OPCODE_PUSHR;
		if(binbuf_write_value(opcode_t, opc) != BINBUF_ERR_OK ||
		   binbuf_write_value(regid_t, regid) != BINBUF_ERR_OK) {
			RETURN(cmd_parser_make_err(CMD_PARSER_ERRT_BINBUF, NULL, 0));
		}
	}
	else {
		opcode_t opc = OPCODE_PUSHV;
		char* pend;
		double v = strtod(args[0], &pend); // TODO: range check
		if(v == HUGE_VAL || pend != args[0] + strlen(args[0])) {
			RETURN(cmd_parser_make_err(CMD_PARSER_ERRT_ARGFMT, 
				   "register or double value excepted", 0));
		}
		if(binbuf_write_value(opcode_t, opc) != BINBUF_ERR_OK ||
		   binbuf_write_value(double, v) != BINBUF_ERR_OK) {
			RETURN(cmd_parser_make_err(CMD_PARSER_ERRT_BINBUF, NULL, 0));
		}
	}
	RETURN(cmd_parser_make_err(CMD_PARSER_ERRT_OK, NULL, 0));
}

cmd_parser_err_t const cmd_pop_parser(char const* args[], size_t nargs)
{$_
	ASSERT(args != NULL);
	if(nargs == 0) {
		opcode_t opc = OPCODE_POPV;
		if(binbuf_write_value(opcode_t, opc) != BINBUF_ERR_OK) {
			RETURN(cmd_parser_make_err(CMD_PARSER_ERRT_BINBUF, NULL, 0));
		}
	}
	else if(nargs == 1) {
		regid_t regid = get_regid(args[0]);
		if(regid != UCHAR_MAX) {
			opcode_t opc = OPCODE_POPR;
			if(binbuf_write_value(opcode_t, opc) != BINBUF_ERR_OK ||
			   binbuf_write_value(regid_t, regid) != BINBUF_ERR_OK) {
				RETURN(cmd_parser_make_err(CMD_PARSER_ERRT_BINBUF, NULL, 0));
			}
		}
		else {
			RETURN(cmd_parser_make_err(CMD_PARSER_ERRT_ARGFMT, 
				   "double value excepted", 0));
		}
	}
	else {
		RETURN(cmd_parser_make_err(CMD_PARSER_ERRT_NARGS, NULL, 0));
	}
	RETURN(cmd_parser_make_err(CMD_PARSER_ERRT_OK, NULL, 0));
}


GEN_TRIVIAL_PARSER(cmd_hlt_parser, OPCODE_HLT);
GEN_TRIVIAL_PARSER(cmd_in_parser, OPCODE_IN);
GEN_TRIVIAL_PARSER(cmd_out_parser, OPCODE_OUT);
GEN_TRIVIAL_PARSER(cmd_add_parser, OPCODE_ADD);
GEN_TRIVIAL_PARSER(cmd_sub_parser, OPCODE_SUB);
GEN_TRIVIAL_PARSER(cmd_mul_parser, OPCODE_MUL);
GEN_TRIVIAL_PARSER(cmd_div_parser, OPCODE_DIV);
GEN_TRIVIAL_PARSER(cmd_sin_parser, OPCODE_SIN);
GEN_TRIVIAL_PARSER(cmd_cos_parser, OPCODE_COS);
GEN_TRIVIAL_PARSER(cmd_sqrt_parser, OPCODE_SQRT);
GEN_TRIVIAL_PARSER(cmd_ret_parser, OPCODE_RET);

GEN_LABELED_PARSER(cmd_je_parser, OPCODE_JE);
GEN_LABELED_PARSER(cmd_jl_parser, OPCODE_JL);
GEN_LABELED_PARSER(cmd_jg_parser, OPCODE_JG);
GEN_LABELED_PARSER(cmd_jn_parser, OPCODE_JN);
GEN_LABELED_PARSER(cmd_jle_parser, OPCODE_JLE);
GEN_LABELED_PARSER(cmd_jge_parser, OPCODE_JGE);
GEN_LABELED_PARSER(cmd_call_parser, OPCODE_CALL);
GEN_LABELED_PARSER(cmd_jmp_parser, OPCODE_JMP);

cmd_parser_t CMD_PARSERS[] = {
 	{ "hlt",	cmd_hlt_parser 		},
 	{ "push",	cmd_push_parser 	},
 	{ "pop",	cmd_pop_parser 		},
 	{ "in",		cmd_in_parser 		},
 	{ "out",	cmd_out_parser 		},
 	{ "add",	cmd_add_parser 		},
 	{ "sub",	cmd_sub_parser 		},
 	{ "mul",	cmd_mul_parser 		},
 	{ "div",	cmd_div_parser 		},
 	{ "sin",	cmd_sin_parser 		},
 	{ "cos",	cmd_cos_parser 		},
 	{ "sqrt",	cmd_sqrt_parser 	},
 	{ "je",		cmd_je_parser 		},
 	{ "jl",		cmd_jl_parser 		},
 	{ "jg",	 	cmd_jg_parser 		},
 	{ "jn", 	cmd_jn_parser 		},
 	{ "jle", 	cmd_jle_parser 		},
 	{ "jge", 	cmd_jge_parser 		},
 	{ "call", 	cmd_call_parser 	},
 	{ "ret", 	cmd_ret_parser 		},
 	{ "jmp", 	cmd_jmp_parser 		},
};

size_t CMD_PARSERS_COUNT = sizeof(CMD_PARSERS) / sizeof(CMD_PARSERS[0]);

cmd_parser_t* const find_cmd_parser(char const* name)
{$_
	ASSERT(name != NULL);

	for(size_t i = 0; i < CMD_PARSERS_COUNT; ++i) {
		if(strcmp(CMD_PARSERS[i].name, name) == 0) {
			RETURN(CMD_PARSERS + i);
		}
	}
	RETURN(NULL);
}

