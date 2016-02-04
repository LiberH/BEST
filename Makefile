CC       = g++
CPPFLAGS = -I./inc -I../p2a -Wall -Werror -g3
LDFLAGS  = -lemon -L../p2a/lib -lp2a -lelf

SRC  = src
OBJ  = obj
BIN  = bin
TEST = test

# Default:
all: dirs target

# Directories:
DIRS = $(OBJ) $(BIN) $(TEST)
dirs: | $(OBJ)
$(DIRS):
	mkdir -p $(DIRS)

# Object files:
OBJS =	$(OBJ)/main.o	\
	$(OBJ)/Inst.o	\
	$(OBJ)/BB.o	\
	$(OBJ)/CFG.o	\
	$(OBJ)/DFS.o	\
	$(OBJ)/DT.o	\
	$(OBJ)/PDT.o	\
	$(OBJ)/CDG.o	\
	$(OBJ)/DDG.o	\
	$(OBJ)/PDG.o	\
	$(OBJ)/Slicer.o	
objs: dirs $(OBJS)
$(OBJ)/%.o: $(SRC)/%.cpp
	$(CC) -o $@ -c $< $(CPPFLAGS) -Wno-write-strings

# Executable file:
TRGT = $(BIN)/main
target: dirs objs $(TRGT)
$(TRGT): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS) `pkg-config libgvc --libs`

# Run:
run: dirs target
	$(TRGT) fibcall.elf

# PNG file from Dot export:
dot: dirs run $(PNGS)
	for f in $(TEST)/*.dot; do \
	  dot -Tpng $$f > $${f%.*}.png; \
	done

# Clean:
clean:
	rm -rf ./$(OBJ)/*.o
	rm -rf ./$(BIN)/*
	rm -rf ./$(TEST)/*.dot
	rm -rf ./$(TEST)/*.png

fullclean:
	rm -rf ./$(OBJ)
	rm -rf ./$(BIN)
	rm -rf ./$(TEST)
