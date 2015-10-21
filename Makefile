CC       = g++
CPPFLAGS = -I$(INC) -Wall -Werror -pedantic -g3 -DDEBUG
LDFLAGS  = -L$(LIB) -lemon #-lelf -lp2a

INC  = inc
SRC  = src
OBJ  = obj
BIN  = bin
LIB  = lib
TEST = test

# Default:
all: dirs target

# Directories:
DIRS = $(OBJ) $(BIN) $(TEST)
dirs: | $(OBJ)
$(DIRS):
	mkdir -p $(DIRS)

# Object files:
OBJS =	$(OBJ)/Inst.o		\
	$(OBJ)/BB.o		\
	$(OBJ)/CFG.o		\
	$(OBJ)/DFSTree.o	\
	$(OBJ)/PDT.o		\
	$(OBJ)/ICFG.o		\
	$(OBJ)/Dot.o		\
	$(OBJ)/main.o
objs: dirs $(OBJS)
$(OBJ)/Dot.o: $(SRC)/Dot.cc
	$(CC) -o $@ -c $< $(CPPFLAGS) `pkg-config libgvc --cflags` -Wno-write-strings
$(OBJ)/%.o: $(SRC)/%.cc
	$(CC) -o $@ -c $< $(CPPFLAGS)

# Executable file:
TRGT = $(BIN)/slicer
target: objs $(TRGT)
$(TRGT): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS) `pkg-config libgvc --libs`

# Run:
run: target
	$(TRGT)

# PNG file from Dot export:
PNGS =	$(TEST)/cfg.png		\
	$(TEST)/rcfg.png	\
	$(TEST)/dfsg.png	\
	$(TEST)/dfst.png	\
	$(TEST)/pdt.png
dot: run $(PNGS)
$(TEST)/%.png: $(TEST)/%.dot
	dot -Tpng $< > $@

# Clean:
clean:
	rm -rf ./$(OBJ)/*
	rm -rf ./$(BIN)/*
	rm -rf ./$(TEST)/*
