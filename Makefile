CC       = g++
CPPFLAGS = -I$(INC) -Wall -Werror -pedantic
CPPFLAGS += -DDEBUG -g3
LDFLAGS  = -lemon
#-L$(LIB) -lelf -lp2a

INC  = inc
SRC  = src
OBJ  = obj
BIN  = bin
#LIB  = lib
TEST = test

# Default:
all: dirs target

# Directories:
DIRS = $(OBJ) $(BIN) $(TEST)
dirs: | $(OBJ)
$(DIRS):
	mkdir -p $(DIRS)

# Object files:
#	$(OBJ)/DFSTree.o	
#	$(OBJ)/PDT.o		
OBJS =	$(OBJ)/Inst.o		\
	$(OBJ)/BB.o		\
	$(OBJ)/CFG.o		\
	$(OBJ)/ICFG.o		\
	$(OBJ)/Dot.o		\
	$(OBJ)/main.o
objs: dirs $(OBJS)
$(OBJ)/Dot.o: $(SRC)/Dot.cpp
	$(CC) -o $@ -c $< $(CPPFLAGS) -Wno-write-strings `pkg-config libgvc --cflags`
$(OBJ)/%.o: $(SRC)/%.cpp
	$(CC) -o $@ -c $< $(CPPFLAGS)

# Executable file:
TRGT = $(BIN)/main
target: dirs objs $(TRGT)
$(TRGT): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS) `pkg-config libgvc --libs`

# Run:
run: dirs target
	$(TRGT)

# PNG file from Dot export:
PNGS =	$(TEST)/bb0.png		\
	$(TEST)/cfg0.png	\
	$(TEST)/icfg.png
dot: dirs run $(PNGS)
$(TEST)/%.png: $(TEST)/%.dot
	dot -Tpng $< > $@

# Clean:
clean:
	rm -rf ./$(OBJ)/*
	rm -rf ./$(BIN)/*
	rm -rf ./$(TEST)/*
