#ifndef DISASMERR_H
#define DISASMERR_H
#include <stdio.h>
#include <ttrack/binbuf.h>
#include <libcommon/labeldic.h>
#include <libcommon/opcodes.h>
#include <libcommon/reginfo.h>

// ---------- DISASSEMBLER ERROR TYPE ---------- //

typedef enum {
	CMD_DISASM_ERRT_OK,
	CMD_DISASM_ERRT_BINBUF,
	CMD_DISASM_ERRT_LABELDIC,
	CMD_DISASM_ERRT_INVOPCODE,
	CMD_DISASM_ERRT_INVOFFSET,
	CMD_DISASM_ERRT_INVREGID,
	CMD_DISASM_ERRT_INVVALUE,
	CMD_DISASM_ERRT_STDIO,
	CMD_DISASM_NTYPES
} disasm_errt_t;

int const disasm_errt_ok(disasm_errt_t errt);
void disasm_errt_assert(disasm_errt_t errt);
char const* disasm_errt_str(disasm_errt_t errt);

// ---------- DISASSEMBLER ERROR PROCESSING ----------- //

typedef struct {
	disasm_errt_t type;
	opcode_t opcode;
	size_t pos;
	union {
		offset_t offset;
		regid_t regid;
		binbuf_err_t binbuf_err;
		labeldic_err_t labeldic_err;

	};
} disasm_err_t;

void disasm_err_binbuf(opcode_t opcode, size_t pos, binbuf_err_t err);
void disasm_err_labeldic(opcode_t opcode, size_t pos, labeldic_err_t err);
void disasm_err_invopcode(opcode_t opcode, size_t pos);
void disasm_err_invoffset(opcode_t opcode, size_t pos, offset_t offset);
void disasm_err_invregid(opcode_t opcode, size_t pos, regid_t regid);
void disasm_err_stdio(opcode_t opcode, size_t pos);

disasm_err_t const disasm_err_get();

void disasm_err_print(disasm_err_t err);
void disasm_err_print_this();


#endif
