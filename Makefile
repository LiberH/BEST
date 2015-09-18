CC       = g++
CPPFLAGS = -Wall -Werror -pedantic -I$(INC)
CPPFLAGS += -DDEBUG -g3
LDFLAGS  = -lemon `pkg-config libgvc --libs`

INC = ./inc
SRC = ./src
OBJ = ./obj
BIN = ./bin

# Default:
all: dirs target

# Directories:
DIRS = $(OBJ) $(BIN)
dirs: | $(DIRS)
$(DIRS):
	mkdir -p $(DIRS)

# Object files:
OBJS =	$(OBJ)/Instruction.o	\
	$(OBJ)/BasicBlock.o	\
	$(OBJ)/Function.o	\
	$(OBJ)/CFG.o		\
	$(OBJ)/dot_utils.o	\
	$(OBJ)/main.o		
$(OBJ)/%.o: $(SRC)/%.cc
	$(CC) $(CPPFLAGS) `pkg-config libgvc --cflags` -o $@ -c $<

# Executable file:
TRGT =	$(BIN)/slicer
target: $(TRGT)
$(TRGT): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

# Clean:
clean:
	rm -f $(TRGT)
	rm -f $(OBJ)/*



