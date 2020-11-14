run: example.bin
	./Disassembler/bin/disassembler example.bin disasm.asm
	./Emulator/bin/emulator example.bin

build: example.bin;

example.bin: example.asm
	./Assembler/bin/assembler example.asm example.bin

