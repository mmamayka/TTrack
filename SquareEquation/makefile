LDFLAGS := -lm
CFLAGS  :=

DOCPATH := doc-html
OBJPATH := obj
SRCPATH := src
BINPATH := bin

BINNAME := squaresolver.out

SRCLISTSNAME := list

$(OBJPATH)/$(SRCLISTSNAME): magic
	echo "CLIST := $$(find . -name '*.c' | sed -e 's/^/\t/g' -e 's/$$/ \\/g')" > $@
	echo "HLIST := $$(find . -name '*.h' | sed -e 's/^/\t/g' -e 's/$$/ \\/g')" >> $@

include $(PBJPATH/$(SRCLISTNAME)

$(CLIST): $(OBJPATH)/%.d: $(RCPATH)/%.c:
	touch $@
