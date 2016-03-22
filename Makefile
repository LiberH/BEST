CC       = gcc
CPPFLAGS = -std=c89 -Wall -Werror -pedantic
LDFLAGS  = 

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

# Executable files:
SRCS = $(wildcard $(SRC)/*.c)
BINS = $(SRCS:$(SRC)/%.c=$(BIN)/%.elf)
binaries: $(BINS)
$(BIN)/%.elf: $(SRC)/%.c
	$(CC) -o $@ $< $(CPPFLAGS) $(LDFLAGS)

# Cleaning:
clean:
	rm -f $(OBJ)/*
	rm -f $(BIN)/*
