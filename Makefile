CC       = g++
CPPFLAGS = -I./inc -I$(BEST_MODULE_PATH) -Wall -Werror -O3 #-ggdb
LDFLAGS  = -lemon -L$(BEST_MODULE_PATH)/lib -lppcmod -lelf

SRC = src
OBJ = obj
BIN = bin

BENCH_GCC = $(BEST_BENCH_PATH)/bin-gcc
BENCH_CSM = $(BEST_BENCH_PATH)/bin-csm
BENCH_TPL = $(BEST_BENCH_PATH)/bin-tpl

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

# Benchmarks:
bench-gcc: target
	@make -C $(BEST_BENCH_PATH) gcc-bins
	@for f in $(BENCH_GCC)/*.elf; do \
	  $(TRGT) $$f 2> /dev/null; \
	done
bench-gcc-pngs:
	@make -C $(BEST_BENCH_PATH) gcc-pngs

bench-csm: target
	@for f in $(BENCH_CSM)/*.elf; do \
	  $(TRGT) $$f 2> /dev/null; \
	done
bench-csm-pngs:
	@make -C $(BEST_BENCH_PATH) csm-pngs

bench-tpl: target
	$(TRGT) $(BENCH_TPL)/testActivateTask.elf 2> /dev/null
bench-tpl-pngs:
	@make -C $(BEST_BENCH_PATH) tpl-pngs

# Clean:
clean:
	rm -rf ./$(OBJ)
	rm -rf ./$(BIN)
