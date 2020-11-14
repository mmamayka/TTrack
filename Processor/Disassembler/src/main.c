#ifdef __GNUC__
#	pragma GCC diagnostic ignored "-Wignored-qualifiers"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ttrack/binbuf.h>
#include <ttrack/dbg.h>
#include <libcommon/labeldic.h>
#include <libcommon/opcodes.h>
#include <libcommon/reginfo.h>

#define MAX_LINE_LEN 1024

typedef int const (*cmd_disassembler_f) (char* line);

typedef struct {
	opcode_t opcode;
	char const* name;
	cmd_disassembler_f disasm;
} cmd_disassembler_t;

int const cmd_disasm_trivial(char* line)
{$_
	ASSERT(line != NULL);
	*line = '\0';
	RETURN(1);
$$
}

int const cmd_disasm_reg(char* line) 
{$_
	ASSERT(line != NULL);
	regid_t regid;
	if(binbuf_read_value(regid_t, regid) != BINBUF_ERR_OK) {
		RETURN(0);
	}
	char const* regname = get_regname(regid);
	if(regname == NULL) {
		RETURN(0);
	}
	if(snprintf(line, MAX_LINE_LEN, regname) < 0) {
		RETURN(0);
	}

	RETURN(1);
}

int const cmd_disasm_val(char* line)
{$_
	ASSERT(line != NULL);
	double v;
	if(binbuf_read_value(double, v) != BINBUF_ERR_OK) {
		RETURN(0);
	}
	if(snprintf(line, MAX_LINE_LEN, "%lf", v) < 0) {
		RETURN(0);
	}
	RETURN(1);
}

#define LABELNAME_LEN 10

int const gen_labelname(char* name) 
{$_
	if(snprintf(name, LABELNAME_LEN, "L_%.6zu", labeldic_size()) < 0) {
		RETURN(0);
	}
	RETURN(1);
}

int const cmd_disasm_label(char* line)
{$_
	offset_t offset;
	if(binbuf_read_value(offset_t, offset) != BINBUF_ERR_OK) {
		RETURN(0);
	}
	char const* labelname = NULL;
	if((labelname = labeldic_addrname((size_t)offset)) != NULL) {
		if(snprintf(line, MAX_LINE_LEN, labelname) < 0) {
			RETURN(0);
		}
	}
	else {
		char nlabelname[LABELNAME_LEN];
		if(!gen_labelname(nlabelname)) {
			RETURN(0);
		}
		if(labeldic_setaddr(nlabelname, (size_t)offset) != LABELDIC_ERR_OK) {
			RETURN(0);
		}
		if(snprintf(line, MAX_LINE_LEN, nlabelname, (size_t)offset) < 0) {
			RETURN(0);
		}
	}
	RETURN(1);
}

cmd_disassembler_t cmd_disasms[OPCODES_COUNT] = {
	{ OPCODE_HLT, 	"hlt", 	cmd_disasm_trivial 	},
	{ OPCODE_IN, 	"in", 	cmd_disasm_trivial 	},
	{ OPCODE_OUT, 	"out", 	cmd_disasm_trivial 	},
	{ OPCODE_ADD, 	"add", 	cmd_disasm_trivial 	},
	{ OPCODE_SUB, 	"sub", 	cmd_disasm_trivial 	},
	{ OPCODE_MUL, 	"mul", 	cmd_disasm_trivial 	},
	{ OPCODE_DIV, 	"div", 	cmd_disasm_trivial 	},
	{ OPCODE_SIN, 	"sin", 	cmd_disasm_trivial 	},
	{ OPCODE_COS, 	"cos", 	cmd_disasm_trivial 	},
	{ OPCODE_SQRT, 	"sqrt", cmd_disasm_trivial 	},
	{ OPCODE_PUSHV, "push", cmd_disasm_val 		},
	{ OPCODE_PUSHR, "push", cmd_disasm_reg 		},
	{ OPCODE_POPV, 	"pop", 	cmd_disasm_val 		},
	{ OPCODE_POPR, 	"pop", 	cmd_disasm_reg 		},
	{ OPCODE_JMP, 	"jmp", 	cmd_disasm_label 	},
	{ OPCODE_JE, 	"je", 	cmd_disasm_label 	},
	{ OPCODE_JN, 	"jn", 	cmd_disasm_label 	},
	{ OPCODE_JL, 	"jl", 	cmd_disasm_label 	},
	{ OPCODE_JG, 	"jg", 	cmd_disasm_label 	},
	{ OPCODE_JGE, 	"jge", 	cmd_disasm_label	},
	{ OPCODE_JLE, 	"jle", 	cmd_disasm_label 	},
	{ OPCODE_CALL, 	"call", cmd_disasm_label 	},
	{ OPCODE_RET, 	"ret", 	cmd_disasm_trivial 	}
};
size_t const CMD_DISASMS_COUNT = sizeof(cmd_disasms) / sizeof(cmd_disasms[0]);

cmd_disassembler_t* const find_disasm(opcode_t opcode) 
{$_
	for(size_t i = 0; i < CMD_DISASMS_COUNT; ++i) {
		if(cmd_disasms[i].opcode == opcode) {
			RETURN(cmd_disasms + i);
		}
	}
	RETURN(NULL);
}

int const disasm_pass(FILE* stream, int write) 
{$_
	opcode_t opcode;
	char buf[MAX_LINE_LEN];
	size_t opcode_pos;
	while((opcode_pos = binbuf_pos()), 
		  binbuf_read_value(opcode_t, opcode) == BINBUF_ERR_OK) 
	{
		cmd_disassembler_t* disasm = find_disasm(opcode);
		if(disasm == NULL) {
			fprintf(stderr, "[ERROR] Unknown opcode %zu\n", (size_t)opcode);
			RETURN(0);
		}
		disasm->disasm(buf);

		if(write) {
			char const* name = labeldic_addrname(opcode_pos);
			if(name != NULL) {
				fprintf(stream, "%s:\n", name);
			}

			fprintf(stream, "\t%s %s\n", disasm->name, buf);
		}
	}
	if(binbuf_error() == BINBUF_ERR_RANGE) {
		binbuf_clearerr();
		RETURN(1);
	}
	RETURN(0);
}


int main(int argc, char* argv[]) 
{$_
	if(argc != 3) {
		fprintf(stderr, 
				"[ERROR] Excepted format \'disassembler <input_file> <output_file>\'\n");
		RETURN(EXIT_FAILURE);
	}

	FILE* ifile = fopen(argv[1], "rb");
	if(ifile == NULL) {
		fprintf(stderr, "[ERROR] File \'%s\' not found\n", argv[1]);
		RETURN(EXIT_FAILURE);
	}

	if(binbuf_initf(ifile) != BINBUF_ERR_OK) {
		fclose(ifile);
		fprintf(stderr, "[ERROR] Failed to read input file\n");
		RETURN(EXIT_FAILURE);
	}
	
	fclose(ifile);

	binbuf_reset();

	FILE* ofile = fopen(argv[2], "w");

	disasm_pass(NULL, 0);

	binbuf_reset();
	disasm_pass(ofile, 1);

	fclose(ofile);

	labeldic_free();

	binbuf_clearerr();
	binbuf_free();
$$
}
