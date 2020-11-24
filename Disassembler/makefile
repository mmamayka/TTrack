LDFLAGS := \
	../ttrack-lib/lib/ttrack-lib.a \
	../LibCommon/lib/libcommon.a \
	-lm

CFLAGS  := \
	-Wall -Wextra \
	-g \
	-I../ttrack-lib/hdr \
	-I../LibCommon/hdr \
	-DSTACKTRACE

DOCPATH := doc-html
OBJPATH := obj
SRCPATH := src
BINPATH := bin

BINNAME := disassembler
DOXCONF := doxygen

run: $(BINPATH)/$(BINNAME)


build: $(BINPATH)/$(BINNAME)

doc: $(DOCPATH)

clean:
	-rm -rf $(OBJPATH)/*
	-rm -rf $(BINPATH)/*
	-rm -rf $(DOCPATH)


_CFILES := $(wildcard $(SRCPATH)/*.c)
_HFILES := $(wildcard $(SRCPATH)/*.h)
_OFILES := $(patsubst $(SRCPATH)/%.c, $(OBJPATH)/%.o, $(_CFILES))
_DFILES := $(patsubst $(SRCPATH)/%.c, $(OBJPATH)/%.d, $(_CFILES))

include $(_DFILES)

$(DOCPATH): $(_CFILES) $(_HFILES)
	doxygen $(DOXCONF)

$(OBJPATH)/%.o: $(SRCPATH)/%.c
	$(CC) -c $< -o $@ $(CFLAGS)

$(OBJPATH)/%.d: $(SRCPATH)/%.c
	$(CC) -MM $< | sed 's/.*:/$(OBJPATH)\/$*.o $(OBJPATH)\/$*.d:/g' > $@

$(BINPATH)/$(BINNAME): $(_OFILES)
	$(CC) -o $@ $^ $(LDFLAGS)

.PHONY: all clean doc build

