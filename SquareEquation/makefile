LDFLAGS := -lm
CFLAGS  :=

# documentation
DOCD := doc-html
# object and prerequsites files
OBJD := obj
# source code
SRCD := src
# binary files
BIND := bin
# name of the program
BINN := squaresolver.out

# build the project
build: $(BIND)/$(BINN);

# run binary
run: $(BIND)/$(BINN)
	./$<

# generate doxygen
docs: $(DOCD);

# clear the project
clean:
	-rm -rf $(BIND)
	-rm -rf $(OBJD)
	-rm -rf doc-html

# ========== ALL MAGIC STARTS HERE ========== #

# source, header and object files lists
_CFILES := $(wildcard $(SRCD)/*.c)
_HFILES := $(wildcard $(SRCD)/*.h)
_OFILES = $(patsubst $(SRCD)/%.c,$(OBJD)/%.o,$(_CFILES))
# prereqiusites files
_DFILES = $(patsubst $(SRCD)/%.c,$(OBJD)/%.d,$(_CFILES))

# generate doxygen
$(DOCD): $(_CFILES) $(_HFILES)
	doxygen doxygen

$(OBJD):
	mkdir $(OBJD)

$(BIND):
	mkdir $(BIND)

# link the program
$(BIND)/$(BINN): $(_OFILES) $(BIND)
	$(CC) -o $@ $(_OFILES) $(LDFLAGS)


# generation of source files prerequisites and saving them to 
# object files directory
$(OBJD)/%.d: $(SRCD)/%.c $(OBJD)
	set -e;
	echo "$(OBJD)/$$(gcc -MM $<)" \
		| sed 's/:/ $(OBJD)\/$*.d:/g' > $@
	echo '	$$(CC) -c -o $$@ $$(CFLAGS) $$<' >> $@

# include all prerequisites
include $(_DFILES)

.PHONY: build run docs clean;
.SECONDARY: $(OBJD)/%.d;

