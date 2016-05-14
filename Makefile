CC      = powerpc-linux-gnu-gcc-5
AS      = powerpc-linux-gnu-as
CFLAGS	= -m32 -std=c89 -ggdb -fno-tree-loop-distribute-patterns
LDFLAGS = -Wl,-T$(SRC)/script.ld,$(OBJ)/crt0.o -nostdlib

SRC = src
OBJ = obj
BIN_GCC = bin-gcc
BIN_CSM = bin-csm
BIN_TPL = bin-tpl

# Default:
all: dirs gcc-bins

# Directories:
DIRS = $(OBJ) $(BIN_GCC)
dirs: | $(OBJ)
$(DIRS):
	mkdir -p $(DIRS)

# crt0 file:
crt0: dirs $(OBJ)/crt0.o
$(OBJ)/crt0.o: $(SRC)/crt0.S
	$(AS) $< -o $@

# Executable files:
SRCS = $(wildcard $(SRC)/*.c)
BINS = $(SRCS:$(SRC)/%.c=$(BIN_GCC)/%)
gcc-bins: crt0 $(BINS)
	@rm -f $(BIN_GCC)/nsichneu*.elf    # too big for Dot export
	@rm -f $(BIN_GCC)/insertsort*.elf  # (HARMLESS-)Stall in -O3.elf
	@rm -f $(BIN_GCC)/ud*.elf          # bug at COSMIC compile time
$(BIN_GCC)/%: $(SRC)/%.c
	@echo "Making ELF files for ... `basename $@`"
	@$(CC) $< -o $@-O0.elf -O0 $(CFLAGS) $(LDFLAGS)
	@$(CC) $< -o $@-O1.elf -O1 $(CFLAGS) $(LDFLAGS)
	@$(CC) $< -o $@-O2.elf -O2 $(CFLAGS) $(LDFLAGS)
	@$(CC) $< -o $@-O3.elf -O3 $(CFLAGS) $(LDFLAGS)

# Dot files:
DOTS = $(wildcard $(BIN_GCC)/*.dot)
PNGS = $(DOTS:$(BIN_GCC)/%-cfg.dot=$(BIN_GCC)/%-cfg.png)
gcc-pngs: $(PNGS)
$(BIN_GCC)/%.png: $(BIN_GCC)/%.dot
	@dot -Tpng $< -o $@

DOTS = $(wildcard $(BIN_CSM)/*.dot)
PNGS = $(DOTS:$(BIN_CSM)/%-cfg.dot=$(BIN_CSM)/%-cfg.png)
csm-pngs: $(PNGS)
$(BIN_CSM)/%.png: $(BIN_CSM)/%.dot
	@dot -Tpng $< -o $@

DOTS = $(wildcard $(BIN_TPL)/*.dot)
PNGS = $(DOTS:$(BIN_TPL)/%-cfg.dot=$(BIN_TPL)/%-cfg.png)
tpl-pngs: $(PNGS)
$(BIN_TPL)/%.png: $(BIN_TPL)/%.dot
	@dot -Tpng $< -o $@

# Cleaning:
clean:
	rm -rf ./$(OBJ)
	rm -rf ./$(BIN_GCC)
	rm -rf ./$(BIN_CSM)/*.dot
	rm -rf ./$(BIN_CSM)/*.elf-*
	rm -rf ./$(BIN_TPL)/*.dot
	rm -rf ./$(BIN_TPL)/*.elf-*
