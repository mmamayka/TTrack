#include <ttrack/dbg.h>
#include "opcodes.h"

int const opcode_ok(opcode_t opcode) {
	return opcode < OPCODES_COUNT;
}
void opcode_assert(opcode_t opcode)
{$_
	ASSERT(opcode_ok(opcode));
$$
}

char const* opcode_str(opcode_t opcode)
{$_
	ASSERT(opcode < OPCODES_COUNT);

#ifdef COOL_MODE
	char const* TABLE[OPCODES_COUNT] = {
		"забудь",
		"расплатись",
		"сдача",
		"прибавь",
		"отними",
		"лошадь",
		"раздупли",
		"грех",
		"сероокись", // Carbon Oxide Sulphide = cos
		"корень",
		"возложи",
		"возложи",
		"прибери",
		"прибери",
		"вспомни",
		"помири",
		"раззадорь",
		"принизь",
		"возвысь",
		"неунизь",
		"невозвысь",
		"припомни",
		"отпусти"
	};
#else
	char const* TABLE[OPCODES_COUNT] = {
		"hlt",
		"in",
		"out",
		"add",
		"sub",
		"mul",
		"div",
		"sin",
		"cos",
		"sqrt",
		"push",
		"push",
		"pop",
		"pop",
		"jmp",
		"je",
		"jn",
		"jl",
		"jg",
		"jge",
		"jle",
		"call",
		"ret",
		"push",
		"push",
		"gpu_clear",
		"gpu_point"
	};
#endif

	return TABLE[opcode];
$$
}
