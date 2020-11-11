LDFLAGS := -lm
CFLAGS  := \
	-Wall -Wextra \
	-g \
	-DSTACKTRACE

DOCPATH := doc-html
OBJPATH := obj
SRCPATH := src
BINPATH := lib

HDRPATH := hdr/ttrack

BINNAME := ttrack-lib.a
DOXCONF := doxygen

_CFILES := $(wildcard $(SRCPATH)/*.c)
_HFILES := $(wildcard $(SRCPATH)/*.h)
_OFILES := $(patsubst $(SRCPATH)/%.c, $(OBJPATH)/%.o, $(_CFILES))
_DFILES := $(patsubst $(SRCPATH)/%.c, $(OBJPATH)/%.d, $(_CFILES))

_HEADERS := $(patsubst $(SRCPATH)/%, $(HDRPATH)/%, $(_HFILES))

build: $(BINPATH)/$(BINNAME) $(_HEADERS)

doc: $(DOCPATH)

clean:
	-rm -rf $(OBJPATH)/*
	-rm -rf $(BINPATH)/*
	-rm -rf $(DOCPATH)
	-rm -rf $(HDRPATH)/*

include $(_DFILES)

$(DOCPATH): $(_CFILES) $(_HFILES)
	doxygen $(DOXCONF)

$(OBJPATH)/%.o: $(SRCPATH)/%.c
	$(CC) -c $< -o $@ $(CFLAGS)

$(OBJPATH)/%.d: $(SRCPATH)/%.c
	$(CC) -MM $< | sed 's/.*:/$(OBJPATH)\/$*.o $(OBJPATH)\/$*.d:/g' > $@

$(BINPATH)/$(BINNAME): $(_OFILES)
	ar rcs $(BINPATH)/$(BINNAME) $^

$(HDRPATH)/%.h: $(SRCPATH)/%.h
	cp $< $@

.PHONY: all clean doc build

