#ifndef OPCODES_H
#define OPCODES_H

#define OPCODE_MAX_LENGTH 128

typedef enum {
	OPCODE_HLT = 0,
	OPCODE_IN,		// 01
	OPCODE_OUT,		// 02
	OPCODE_ADD,		// 03
	OPCODE_SUB,		// 04
	OPCODE_MUL,		// 05
	OPCODE_DIV,		// 06
	OPCODE_SIN,		// 07
	OPCODE_COS,		// 08
	OPCODE_SQRT,	// 09
	OPCODE_PUSHV,	// 0A
	OPCODE_PUSHR,	// 0B
	OPCODE_POPV,	// 0C
	OPCODE_POPR,	// 0D
	OPCODE_JMP,		// 0E
	OPCODE_JE,		// 0F
	OPCODE_JN,		// 10
	OPCODE_JL,		// 11
	OPCODE_JG,		// 12
	OPCODE_JGE,		// 13
	OPCODE_JLE,		// 14
	OPCODE_CALL,	// 15
	OPCODE_RET,		// 16
} opcode_s;

typedef unsigned char opcode_t;
typedef unsigned short offset_t;

#endif
