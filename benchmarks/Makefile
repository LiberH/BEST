CC      = powerpc-linux-gnu-gcc-5
AS      = powerpc-linux-gnu-as
CFLAGS	= -m32 -std=c89 -ggdb -fno-tree-loop-distribute-patterns
LDFLAGS = -Wl,-T$(SRC)/script.ld,$(OBJ)/crt0.o -nostdlib

SRC = src
OBJ = obj
BIN = bin
BIN_CSM = $(BIN)/cosmic
BIN_GCC = $(BIN)/gcc
BIN_MDL = $(BIN_GCC)/malardalen
BIN_TPL = $(BIN_GCC)/trampoline

# Default:
all: dirs bin-mdl

# Directories:
DIRS = $(OBJ) $(BIN_MDL)
dirs: | $(OBJ)
$(DIRS):
	mkdir -p $(DIRS)

# crt0 file:
crt0: dirs $(OBJ)/crt0.o
$(OBJ)/crt0.o: $(SRC)/crt0.S
	$(AS) $< -o $@

# Executable files:
SRCS = $(wildcard $(SRC)/*.c)
BINS = $(SRCS:$(SRC)/%.c=$(BIN_MDL)/%)
bin-mdl: crt0 $(BINS)
	@rm -f $(BIN_MDL)/nsichneu*.elf    # too big for Dot export
	@rm -f $(BIN_MDL)/insertsort*.elf  # (HARMLESS-)Stall in -O3.elf
	@rm -f $(BIN_MDL)/ud*.elf          # bug at COSMIC compile time
$(BIN_MDL)/%: $(SRC)/%.c
	@echo "Making ELF files for ... `basename $@`"
	@$(CC) $< -o $@-O0.elf -O0 $(CFLAGS) $(LDFLAGS)
	@$(CC) $< -o $@-O1.elf -O1 $(CFLAGS) $(LDFLAGS)
	@$(CC) $< -o $@-O2.elf -O2 $(CFLAGS) $(LDFLAGS)
	@$(CC) $< -o $@-O3.elf -O3 $(CFLAGS) $(LDFLAGS)

# Benchmark suite:
ifndef BIN_PATH
  BIN_PATH = $(BIN_MDL)
endif

malardalen:
	@for f in $(BIN_PATH)/*.elf                ; \
	 do basename=`basename $$f`                ; \
	    name=`echo $$basename | cut -d'.' -f1` ; \
	    target="malardalen/$$name"             ; \
	    $(MAKE) --no-print-directory $$target  ; \
	 done

malardalen/%:
	@$(BEST_PATH)/bin/main --template=$(BEST_PLUGINS_PATH)/$(BEST_PLUGIN)/$(BEST_TEMPLATE) $(BIN_PATH)/$*.elf --cfg-only
	@dot -Tdot $(BIN_PATH)/$*.elf-cfg.dot -o $(BIN_PATH)/$*.elf.dot
	@$(BEST_PATH)/bin/main --template=$(BEST_PLUGINS_PATH)/$(BEST_PLUGIN)/$(BEST_TEMPLATE) $(BIN_PATH)/$*.elf

# Verifying:
VERIFYTA = timeout 20 $(UPPAAL_PATH)/bin-Linux/verifyta
malardalen-verify:
	@rm -f $(BIN_PATH)/*-O0*      # Needs stack data
	@rm -f $(BIN_PATH)/*-O3*      # Results not used

	@rm -f $(BIN_PATH)/cnt-*      # Needs stack data
	@rm -f $(BIN_PATH)/crc-*      # Needs stack data
	@rm -f $(BIN_PATH)/matmult-*  # Needs stack data

	@rm -f $(BIN_PATH)/adpcm-*    # Too big to debug
	@rm -f $(BIN_PATH)/compress-* # Too big to debug
	@rm -f $(BIN_PATH)/ndes-*     # Too big to debug

	@for f in $(BIN_PATH)/*.elf-model_sliced.xml ; \
	 do basename=`basename $$f`                 ; \
	    name=`echo $$basename | cut -d'.' -f1`  ; \
	    target="malardalen-verify/$$name"       ; \
	    $(MAKE) --no-print-directory $$target   ; \
	 done

# TODO: check if file exist.
malardalen-verify/%:
	@s_model="$*.elf-model_sliced.xml"          ; \
	 __model="$*.elf-model.xml"                 ; \
	    file=`echo $$s_model | cut -d'.' -f1`   ; \
	  s_temp=`mktemp /tmp/best-makefile.XXXXXX` ; \
	  __temp=`mktemp /tmp/best-makefile.XXXXXX` ; \
\
	 echo -n "WCET for $$file .. "                                 ; \
	 $(VERIFYTA) $(BIN_PATH)/$$s_model > $$s_temp 2> /dev/null      ; \
	 if [ "$$?" -eq 124 ]                                          ; \
	   then echo "TIMEOUT"; exit 0; fi                             ; \
	 s_abrt=`grep 'Aborted' $$s_temp`                              ; \
	 if [ "$$s_abrt" != "" ]                                       ; \
	   then echo "ABORTED"; exit 0; fi                             ; \
	 s_line=`grep "_clock <= " $$s_temp`                           ; \
	 s_wcet=`echo $$s_line | sed -e 's/_clock <= \([0-9]\+\)/\1/'` ; \
\
	 $(VERIFYTA) $(BIN_PATH)/$$__model > $$__temp 2> /dev/null      ; \
	 if [ "$$?" -eq 124 ]                                          ; \
	   then echo "TIMEOUT"; exit 0; fi                             ; \
	 __abrt=`grep 'Aborted' $$__temp`                              ; \
	 if [ "$$__abrt" != "" ]                                       ; \
	   then echo "ABORTED"; exit 0; fi                             ; \
	 __line=`grep "_clock <= " $$__temp`                           ; \
	 __wcet=`echo $$__line | sed -e 's/_clock <= \([0-9]\+\)/\1/'` ; \
\
	 if [ "$$s_wcet" -ne "$$__wcet" ]    ; \
	   then echo "DIVERGING"; exit 0; fi ; \
	 echo "$$s_wcet cycles"

# Dot export:
malardalen-export:
	@for elf in $(BIN_PATH)/*.elf; do \
	  dot -Tpng $$elf-cfg.dot -o $$elf-cfg.png; \
	done

malardalen-export/%:
	@dot -Tpng $(BIN_PATH)/$*.elf-cfg.dot -o $(BIN_PATH)/$*.elf-cfg.png

# Cleaning:
clean:
	rm -rf ./$(OBJ)

	rm -rf ./$(BIN_CSM)/*.dot
	rm -rf ./$(BIN_CSM)/*.elf-*
	rm -rf ./$(BIN_CSM)/*.xml

	rm -rf ./$(BIN_MDL)/*.dot
	rm -rf ./$(BIN_MDL)/*.elf-*
	rm -rf ./$(BIN_MDL)/*.xml

	rm -rf ./$(BIN_TPL)/*.dot
	rm -rf ./$(BIN_TPL)/*.elf-*
	rm -rf ./$(BIN_TPL)/*.xml

