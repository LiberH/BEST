CC      = powerpc-linux-gnu-gcc-5
AS	= powerpc-linux-gnu-as
OD      = powerpc-linux-gnu-objdump
CFLAGS	= -g
LDFLAGS = -Wl,-T$(SRC)/script.ld,$(OBJ)/crt0.o -nostdlib

SRC = src
OBJ = obj
BIN = bin

BENCH =	expint		\
	fibcall		\
	janne_complex	\
	ud
all: $(BENCH)

# Directories:
DIRS = $(OBJ) $(BIN)
dirs: | $(OBJ)
$(DIRS):
	mkdir -p $(DIRS)

crt0: dirs $(OBJ)/crt0.o
$(OBJ)/crt0.o: $(SRC)/crt0.S
	$(AS) $< -o $@

# Object files:
%: $(SRC)/%.c dirs crt0
	@echo "Making ELF files for ... $@"
	@$(CC) $< -o $(BIN)/$@-O0.elf -O0 $(CFLAGS) $(LDFLAGS)
	@$(CC) $< -o $(BIN)/$@-O1.elf -O1 $(CFLAGS) $(LDFLAGS)
	@$(CC) $< -o $(BIN)/$@-O2.elf -O2 $(CFLAGS) $(LDFLAGS)
	@$(CC) $< -o $(BIN)/$@-O3.elf -O3 $(CFLAGS) $(LDFLAGS)

# Clean:
clean:
	rm -rf ./$(OBJ)
	rm -rf ./$(BIN)
