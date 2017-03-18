CC       = g++
CPPFLAGS = -Wall -Werror -ggdb -O2 -I./inc -I$(BEST_PLUGINS_PATH)/$(BEST_PLUGIN)/core
LDFLAGS  = -lemon -L./lib -ltinyxml2 -L$(BEST_PLUGINS_PATH)/$(BEST_PLUGIN)/core/lib -l$(BEST_PLUGIN) -lelf

SRC = src
OBJ = obj
BIN = bin

# Default:
all: check-env dirs objs target

# Directories:
DIRS = $(OBJ) $(BIN)
dirs: | $(OBJ)
$(DIRS):
	mkdir -p $(DIRS)

# Check ENV variables:
check-env:
	@if [ "${BEST_PLUGINS_PATH}" = "" ] ||     \
	    [ "${BEST_PLUGIN}"       = "" ]; then  \
	  echo "Environment variable(s) not set:"; \
	  echo "    BEST_PLUGINS_PATH           "; \
	  echo " or BEST_PLUGIN                 "; \
	  exit 1;                                  \
	fi

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

# Run the benchmark suite:
benchmarks: target
	@BEST_TRGT=`pwd`/$(TRGT) make -C $(BEST_BENCHMARKS_PATH) $(BEST_BENCHMARK)

benchmarks/%: target
	@BEST_TRGT=`pwd`/$(TRGT) make -C $(BEST_BENCHMARKS_PATH) $(BEST_BENCHMARK)/$*

# Clean:
clean:
	rm -rf ./$(OBJ)
	rm -rf ./$(BIN)
