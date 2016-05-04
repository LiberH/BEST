CC      = powerpc-linux-gnu-gcc-5
AS      = powerpc-linux-gnu-as
CFLAGS	= -m32 -std=c89 -ggdb -fno-tree-loop-distribute-patterns
LDFLAGS = -Wl,-T$(SRC)/script.ld,$(OBJ)/crt0.o -nostdlib

SRC = src
OBJ = obj
BIN = bin

# Default:
all: dirs binaries
	@rm -f $(BIN)/nsichneu-O?.elf   # too big
	@rm -f $(BIN)/insertsort-O?.elf # 'Stall' in -O3.elf

# Directories:
DIRS = $(OBJ) $(BIN)
dirs: | $(OBJ)
$(DIRS):
	mkdir -p $(DIRS)

# crt0 file:
crt0: dirs $(OBJ)/crt0.o
$(OBJ)/crt0.o: $(SRC)/crt0.S
	$(AS) $< -o $@

# Executable files:
SRCS = $(wildcard $(SRC)/*.c)
BINS = $(SRCS:$(SRC)/%.c=$(BIN)/%)
binaries: crt0 $(BINS)
$(BIN)/%: $(SRC)/%.c
	@echo "Making ELF files for ... `basename $@`"
	@$(CC) $< -o $@-O0.elf -O0 $(CFLAGS) $(LDFLAGS)
	@$(CC) $< -o $@-O1.elf -O1 $(CFLAGS) $(LDFLAGS)
	@$(CC) $< -o $@-O2.elf -O2 $(CFLAGS) $(LDFLAGS)
	@$(CC) $< -o $@-O3.elf -O3 $(CFLAGS) $(LDFLAGS)

# Dot files:
DOTS = $(wildcard $(BIN)/*.dot)
PNGS = $(DOTS:$(BIN)/%-cfg.dot=$(BIN)/%-cfg.png)
pngs: $(PNGS)
$(BIN)/%.png: $(BIN)/%.dot
	@dot -Tpng $< -o $@

# Cleaning:
clean:
	rm -rf ./$(OBJ)
	rm -rf ./$(BIN)
