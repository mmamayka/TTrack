#include <ttrack/dbg.h>
#include "disasmerr.h"

// ---------- DISASSEMBLER ERROR TYPE ---------- //

int const disasm_errt_ok(disasm_errt_t errt) 
{
	return errt >= 0 && errt < CMD_DISASM_NTYPES;
}
void disasm_errt_assert(disasm_errt_t errt)
{$_
	ASSERT(disasm_errt_ok(errt));
$$
}

char const* disasm_errt_str(disasm_errt_t errt)
{$_
	disasm_errt_assert(errt);

	static char const* TABLE[CMD_DISASM_NTYPES] = {
		"ok",
		"error in the binary buffer",
		"error in the label dictionary",
		"invalid opcode",
		"invalid offset",
		"invalid register id",
		"invalid value",
		"stdio error"
	};
	RETURN(TABLE[errt]);
}

// ---------- DISASSEMBLER ERROR PROCESSING ----------- //

static disasm_err_t ERROR = { CMD_DISASM_ERRT_OK };

void disasm_err_binbuf(opcode_t opcode, size_t pos, binbuf_err_t err) 
{$_
	ERROR.type = CMD_DISASM_ERRT_BINBUF;
	ERROR.opcode = opcode;
	ERROR.pos = pos;
	ERROR.binbuf_err = err;
$$
}

void disasm_err_labeldic(opcode_t opcode, size_t pos, labeldic_err_t err)
{$_
	ERROR.type = CMD_DISASM_ERRT_LABELDIC;
	ERROR.opcode = opcode;
	ERROR.pos = pos;
	ERROR.labeldic_err = err;
$$
}

void disasm_err_invopcode(opcode_t opcode, size_t pos)
{$_
	ERROR.type = CMD_DISASM_ERRT_INVOPCODE;
	ERROR.opcode = opcode;
	ERROR.pos = pos;
$$
}

void disasm_err_invoffset(opcode_t opcode, size_t pos, offset_t offset)
{$_
	ERROR.type = CMD_DISASM_ERRT_INVOPCODE;
	ERROR.opcode = opcode;
	ERROR.pos = pos;
	ERROR.offset = offset;
$$
}

void disasm_err_invregid(opcode_t opcode, size_t pos, regid_t regid)
{$_
	ERROR.type = CMD_DISASM_ERRT_INVOPCODE;
	ERROR.opcode = opcode;
	ERROR.pos = pos;
	ERROR.regid = regid;
$$
}

void disasm_err_stdio(opcode_t opcode, size_t pos) 
{$_
	ERROR.type = CMD_DISASM_ERRT_STDIO;
	ERROR.opcode = opcode;
	ERROR.pos = pos;
}

disasm_err_t const disasm_err_get()
{$_
	RETURN(ERROR);
$$
}

void disasm_err_print(disasm_err_t err)
{$_
	disasm_errt_assert(err.type);
	switch(err.type) {
	case CMD_DISASM_ERRT_OK:
		fprintf(stderr, "[DISASSEMBLER] All is ok at %zX (opcode %hhx)\n",
				(size_t)err.pos, err.opcode);
		break;

	case CMD_DISASM_ERRT_BINBUF:
		fprintf(stderr, "[DISASSEMBLER] Error occured in the binary buffer "
				"at %zX (opcode %hhx)\n", (size_t)err.pos, err.opcode);
		switch(err.binbuf_err) {
		case BINBUF_ERR_OK:
			fprintf(stderr, "\t[BINBUF] All is ok\n");
			break;

		case BINBUF_ERR_MEM:
			fprintf(stderr, "\t[BINBUF] Out of memory\n");
			break;

		default:
			fprintf(stderr, "\t[BINBUF] Data corrupted\n");
			break;
		}
		break;

	case CMD_DISASM_ERRT_LABELDIC:
		fprintf(stderr, "[DISASSEMBLER] Error occured in the label dictionary "
				"at %zX (opcode %hhx)\n", (size_t)err.pos, err.opcode);
		switch(err.labeldic_err) {
		case LABELDIC_ERR_OK:
			fprintf(stderr, "\t[LABELDIC] All is ok\n");
			break;

		case LABELDIC_ERR_OVERFLOW:
			fprintf(stderr, "\t[LABELDIC] Too many labels\n");
			break;

		case LABELDIC_ERR_MEM:
			fprintf(stderr, "\t[LABELDIC] Out of memory\n");
			break;

		default:
			fprintf(stderr, "\t[LABELDIC] Data corrupted\n");
			
		}
		break;

	case CMD_DISASM_ERRT_INVOPCODE:
		fprintf(stderr, "[DISASSEMBLER] Invalid opcode %hhx at %zX\n", 
				err.opcode, err.pos);
		break;

	case CMD_DISASM_ERRT_INVOFFSET:
		fprintf(stderr, "[DISASSEMBLER] Invalid offset %zX at %zX (opcode %hhx)\n",
				(size_t)err.offset, (size_t)err.pos, err.opcode);
		break;

	case CMD_DISASM_ERRT_INVREGID:
		fprintf(stderr, "[DISASSEMBLER] Invalid register id %zX at %zX (opcode %hhx)\n",
				(size_t)err.regid, (size_t)err.pos, err.opcode);
		break;

	case CMD_DISASM_ERRT_STDIO:
		fprintf(stderr, "[DISASSEMBLER] Error in stdio at %zX (opcode %hhx)\n",
				(size_t)err.pos, err.opcode);
		break;

	default:
		break;
	}
$$
}

void disasm_err_print_this() 
{$_
	disasm_err_print(ERROR);
$$
}
