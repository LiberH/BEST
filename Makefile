CC      = powerpc-linux-gnu-gcc-5
AS	= powerpc-linux-gnu-as
CFLAGS	= -m32 -std=c89 -Wall -Wextra -Werror -pedantic -ggdb
LDFLAGS = -Wl,-T$(SRC)/script.ld,$(OBJ)/crt0.o -nostdlib

SRC = src
OBJ = obj
BIN = bin

# Default:
all: dirs binaries

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
BINS = $(SRCS:$(SRC)/%.c=$(BIN)/%.elf)
binaries: crt0 $(BINS)
$(BIN)/%.elf: $(SRC)/%.c
	$(CC) -o $@ $< $(CFLAGS) $(LDFLAGS)

# Cleaning:
clean:
	rm -f $(OBJ)/*
	rm -f $(BIN)/*
