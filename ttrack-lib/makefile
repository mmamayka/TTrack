
LDFLAGS := \
	-lm

CFLAGS  := \
	-Wall -Wextra \
	-g \
	-DSTACKTRACE \

OBJPATH := obj
SRCPATH := src
BINPATH := lib
HDRPATH := hdr/ttrack

BINNAME := ttrack-lib.a

_CFILES := $(wildcard $(SRCPATH)/*.c)
_HFILES := $(wildcard $(SRCPATH)/*.h)
_OFILES := $(patsubst $(SRCPATH)/%.c, $(OBJPATH)/%.o, $(_CFILES))
_DFILES := $(patsubst $(SRCPATH)/%.c, $(OBJPATH)/%.d, $(_CFILES))

_HEADERS := $(patsubst $(SRCPATH)/%, $(HDRPATH)/%, $(_HFILES))

build: $(BINPATH)/$(BINNAME) $(_HEADERS)

clean:
	-rm -rf $(OBJPATH)/*
	-rm -rf $(BINPATH)/*
	-rm -rf $(DOCPATH)
	-rm -rf $(HDRPATH)/*

$(OBJPATH)/%.d: $(SRCPATH)/%.c
	$(CC) -MM $< | sed 's/.*:/$(OBJPATH)\/$*.o $(OBJPATH)\/$*.d:/g' > $@
	echo -e '\t$$(CC) -c $$< -o $$@ $$(CFLAGS)' >> $@

$(BINPATH)/$(BINNAME): $(_OFILES)
	ar rcs $(BINPATH)/$(BINNAME) $^

$(HDRPATH)/%.h: $(SRCPATH)/%.h
	cp $< $@

include $(_DFILES)

.PHONY: all clean doc build

