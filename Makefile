CC       = g++
CPPFLAGS = -I$(INC) -I$(HINC) -Wall -Werror -pedantic -Wno-long-long
LDFLAGS  = -L$(LIB) -lp2a -lelf -lemon `pkg-config libgvc --libs`

INC  = inc
SRC  = src
OBJ  = obj
BIN  = bin
LIB  = lib
TEST = test
HINC = inc/harmless

# Default:
all: dirs target

# Directories:
DIRS = $(OBJ) $(BIN) $(TEST)
dirs: | $(OBJ)
$(DIRS):
	mkdir -p $(DIRS)

# Object files:
OBJS =	$(OBJ)/Instruction.o	\
	$(OBJ)/BasicBlock.o	\
	$(OBJ)/Function.o	\
	$(OBJ)/CFG.o		\
	$(OBJ)/Dot.o		\
	$(OBJ)/DFS.o		\
	$(OBJ)/TestCase.o	\
	$(OBJ)/main.o
objs: dirs $(OBJS)
$(OBJ)/Dot.o: $(SRC)/Dot.cc
	$(CC) -o $@ -c $< $(CPPFLAGS) -Wno-write-strings `pkg-config libgvc --cflags`
$(OBJ)/main.o: $(SRC)/main.cc
	$(CC) -o $@ -c $< $(CPPFLAGS) -Wno-unused-but-set-variable
$(OBJ)/%.o: $(SRC)/%.cc
	$(CC) -o $@ -c $< $(CPPFLAGS)

# Executable file:
TRGT = $(BIN)/slicer
target: objs $(TRGT)
$(TRGT): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

# Run:
run: target
	$(TRGT)

# PNG file from Dot export:
dot: target
	$(TRGT) > $(TEST)/slice.dot
	dot -Tpng $(TEST)/slice.dot > $(TEST)/slice.png

# Clean:
clean:
	rm -rf ./$(OBJ)/*
	rm -rf ./$(BIN)/*
	rm -rf ./$(TEST)/*
