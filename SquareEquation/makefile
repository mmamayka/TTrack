LDFLAGS := -lm
CFLAGS  := -D TESTS

DOCPATH := doc-html
OBJPATH := obj
SRCPATH := src
BINPATH := bin

BINNAME := squaresolver.out

all: $(BINPATH)/$(BINNAME)

clean:
	-rm -f $(OBJPATH)/*
	-rm -f $(BINPATH)/*

$(OBJPATH)/%.o: $(SRCPATH)/%.c
	$(CC) -c $< -o $@ $(CFLAGS)

_CFILES := $(wildcard $(SRCPATH)/*.c)
_OFILES := $(patsubst $(SRCPATH)/%.c, $(OBJPATH)/%.o, $(_CFILES))
_DFILES := $(patsubst $(SRCPATH)/%.c, $(OBJPATH)/%.d, $(_CFILES))

include $(_DFILES)

$(OBJPATH)/%.d: $(SRCPATH)/%.c
	gcc -MM $< | sed 's/.*:/$(OBJPATH)\/$*.o $(OBJPATH)\/$*.d:/g' > $@

$(BINPATH)/$(BINNAME): $(_OFILES)
	$(CC) -o $@ $^ $(LDFLAGS)

.PHONY: all clean

