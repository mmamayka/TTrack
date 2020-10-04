LDFLAGS := -lm
CFLAGS  := -D TESTS

DOCPATH := doc-html
OBJPATH := obj
SRCPATH := src
BINPATH := bin

BINNAME := squaresolver.out
DOXCONF := doxygen

all: build doc

build: $(BINPATH)/$(BINNAME)

doc: $(DOCPATH)

clean:
	-rm -rf $(OBJPATH)
	-rm -rf $(BINPATH)
	-rm -rf $(DOCPATH)


_CFILES := $(wildcard $(SRCPATH)/*.c)
_HFILES := $(wildcard $(SRCPATH)/*.h)
_OFILES := $(patsubst $(SRCPATH)/%.c, $(OBJPATH)/%.o, $(_CFILES))
_DFILES := $(patsubst $(SRCPATH)/%.c, $(OBJPATH)/%.d, $(_CFILES))

include $(_DFILES)

$(OBJPATH):
	mkdir $(OBJPATH)

$(BINPATH):
	mkdir $(BINPATH)

$(DOCPATH): $(_CFILES) $(_HFILES)
	doxygen $(DOXCONF)

$(OBJPATH)/%.o: $(SRCPATH)/%.c $(OBJPATH)
	$(CC) -c $< -o $@ $(CFLAGS)

$(OBJPATH)/%.d: $(SRCPATH)/%.c $(OBJPATH)
	gcc -MM $< | sed 's/.*:/$(OBJPATH)\/$*.o $(OBJPATH)\/$*.d:/g' > $@

$(BINPATH)/$(BINNAME): $(_OFILES) $(BINPATH)
	$(CC) -o $@ $(_OFILES) $(LDFLAGS)

.PHONY: all clean doc build

