CC       = g++
CPPFLAGS = -Wall -Werror -ggdb -O2 -I./inc -I$(BEST_PLUGIN_PATH)
LDFLAGS  = -lemon -L./lib -ltinyxml2 -L$(BEST_PLUGIN_PATH)/lib -lbest-$(BEST_PLUGIN) -lelf

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
	@if [ "${BEST_PLUGIN}"      = "" ] ||      \
            [ "${BEST_PLUGIN_PATH}" = "" ]; then   \
	  echo "Environment variable(s) not set:"; \
          echo "    BEST_PLUGIN                 "; \
	  echo " or BEST_PLUGIN_PATH            "; \
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
	BEST_TRGT=`pwd`/$(TRGT) make -C $(BEST_BENCHMARKS_PATH) $(BEST_BENCHMARKS)

# Clean:
clean:
	rm -rf ./$(OBJ)
	rm -rf ./$(BIN)
