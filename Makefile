CC       = g++
CPPFLAGS = -I./inc -I../p2a -Wall -Werror -ggdb
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
	done

# Benchmarks:
bench: target
	@for f in $(BENCH)/*.elf; do \
	  echo -n "`basename $$f` ... "; \
	  $(TRGT) $$f 2> /dev/null; \
	  if [ $$? -ne 0 ]; then \
	    echo "Fail"; \
	  else \
	    echo "Pass"; \
	  fi; \
	done

# Clean:
clean: testclean benchclean
	rm -rf ./$(OBJ)
	rm -rf ./$(BIN)

testclean:
	rm -f ./$(TEST)/*.dmp
	rm -f ./$(TEST)/*.dot
	rm -f ./$(TEST)/*.png
	rm -f ./$(TEST)/*.xml

benchclean:
	rm -f ./$(BENCH)/*.dmp
	rm -f ./$(BENCH)/*.dot
	rm -f ./$(BENCH)/*.png
	rm -f ./$(BENCH)/*.xml
