CC       = g++
CPPFLAGS = -I./inc -I../p2a -Wall -Werror -g3
LDFLAGS  = -lemon -L../p2a/lib -lp2a -lelf

SRC   = src
OBJ   = obj
BIN   = bin
TEST  = test
BENCH = bench/bin

# Default:
all: dirs target

# Directories:
DIRS = $(OBJ) $(BIN)
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
target: objs $(TRGT)
$(TRGT): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS) `pkg-config libgvc --libs`

# Test:
test: target
	@for f in $(TEST)/*.elf; do \
	  echo "Testing ... `basename $$f`"; \
	  $(TRGT) $$f; \
	  for g in $(TEST)/`basename $$f .elf`*.dot; do \
	    dot -Tpng $$g > $${g%.*}.png; \
	  done; \
	done

# Benchmarks:
bench: target
	@for f in $(BENCH)/*.elf; do \
	  echo "Benchmarking ... `basename $$f`"; \
	  $(TRGT) $$f; \
	  for g in $(BENCH)/`basename $$f .elf`*.dot; do \
	    dot -Tpng $$g > $${g%.*}.png; \
	  done; \
	done

# Clean:
clean:
	rm -f ./$(TEST)/*.dmp ./$(TEST)/*.dot ./$(TEST)/*.png
	rm -f ./$(BENCH)/*.dmp ./$(BENCH)/*.dot ./$(BENCH)/*.png

fullclean: clean
	rm -rf ./$(OBJ)
	rm -rf ./$(BIN)
