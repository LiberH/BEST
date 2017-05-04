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

QUERIES = queries
WCET_QUERY = $(QUERIES)/wcet.q
PRED_QUERY = $(QUERIES)/pred.q

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
ifndef BEST_BIN
  BEST_BIN = $(BEST_PATH)/bin/main
endif

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
ifndef OUTPUT_DIR
	@$(BEST_BIN) --template-file=$(BEST_TEMPLATE) --cfg-only $(BIN_PATH)/$*.elf
	@dot -Tdot $(BIN_PATH)/$*.elf-cfg.dot -o $(BIN_PATH)/$*.elf.dot
	@$(BEST_BIN) --template-file=$(BEST_TEMPLATE) $(BIN_PATH)/$*.elf
else
	@$(BEST_BIN) --output-dir=$(OUTPUT_DIR) --template-file=$(BEST_TEMPLATE) --cfg-only $(BIN_PATH)/$*.elf
	@dot -Tdot $(OUTPUT_DIR)/$*.elf-cfg.dot -o $(OUTPUT_DIR)/$*.elf.dot
	@$(BEST_BIN) --output-dir=$(OUTPUT_DIR) --template-file=$(BEST_TEMPLATE) $(BIN_PATH)/$*.elf
endif

# Verifying:
VERIFYTA = timeout 60 $(UPPAAL_PATH)/bin-Linux/verifyta
malardalen-verify: labels
	@rm -f $(BIN_PATH)/*-O0*      # Needs stack data
	@rm -f $(BIN_PATH)/*-O3*      # Results not used

	@rm -f $(BIN_PATH)/cnt-*      # Needs stack data
	@rm -f $(BIN_PATH)/crc-*      # Needs stack data
	@rm -f $(BIN_PATH)/matmult-*  # Needs stack data

	@rm -f $(BIN_PATH)/adpcm-*    # Too big to debug
	@rm -f $(BIN_PATH)/compress-* # Too big to debug
	@rm -f $(BIN_PATH)/ndes-*     # Too big to debug

	@for f in $(BIN_PATH)/*.elf-model.xml      ; \
	 do basename=`basename $$f`                ; \
	    name=`echo $$basename | cut -d'.' -f1` ; \
	    target="malardalen-verify/$$name"      ; \
	    $(MAKE) --no-print-directory $$target  ; \
	 done

# TODO: check if file exist.
malardalen-verify/%:
	@__model="$*.elf-model.xml"                ; \
	 __file=`echo $$__model | cut -d'.' -f1`   ; \
	 __temp=`mktemp /tmp/best-makefile.XXXXXX` ; \
	 $(VERIFYTA) -u $(BIN_PATH)/$$__model \
				$(WCET_QUERY) > $$__temp 2> /dev/null  ; \
	 if [ "$$?" -eq 124 ]                                          ; \
	   then echo "TIMEOUT"; exit 0; fi                             ; \
	 __abrt=`grep 'Aborted' $$__temp`                              ; \
	 if [ "$$__abrt" != "" ]                                       ; \
	   then echo "ABORTED"; exit 0; fi                             ; \
	 __WCET=`grep "_clock <= "                  $$__temp | sed -e 's/_clock <= \([0-9]\+\)/\1/'`                      ; \
	 __staS=`grep " -- States stored : "        $$__temp | sed -e 's/ -- States stored : \([0-9]\+\) states/\1/'`     ; \
	 __staE=`grep " -- States explored : "      $$__temp | sed -e 's/ -- States explored : \([0-9]\+\) states/\1/'`   ; \
	 __cpuT=`grep " -- CPU user time used : "   $$__temp | sed -e 's/ -- CPU user time used : \([0-9]\+\) ms/\1/'`    ; \
	 __memV=`grep " -- Virtual memory used : "  $$__temp | sed -e 's/ -- Virtual memory used : \([0-9]\+\) KiB/\1/'`  ; \
	 __memR=`grep " -- Resident memory used : " $$__temp | sed -e 's/ -- Resident memory used : \([0-9]\+\) KiB/\1/'` ; \
\
	 m_model="$*.elf-model_monitored.xml" ; \
	 m_temp=`mktemp /tmp/best-makefile.XXXXXX` ; \
	 $(VERIFYTA) -u $(BIN_PATH)/$$m_model \
				$(PRED_QUERY) > $$m_temp 2> /dev/null  ; \
	 if [ "$$?" -eq 124 ]                                          ; \
	   then echo "TIMEOUT"; exit 0; fi                             ; \
	 m_abrt=`grep 'Aborted' $$m_temp`                              ; \
	 if [ "$$m_abrt" != "" ]                                       ; \
	   then echo "ABORTED"; exit 0; fi                             ; \
	 m_BTBow=`grep "_M_BTBOverwriting <= "                              $$m_temp | sed -e 's/_M_BTBOverwriting <= \([0-9]\+\)/\1/'`  ; \
	 m_sIPNT=`grep "_M_StaticPred.incorrectly_predicted_not_taken <= "  $$m_temp | sed -e 's/_M_StaticPred.incorrectly_predicted_not_taken <= \([0-9]\+\)/\1/'`  ; \
	 m_sIP_T=`grep "_M_StaticPred.incorrectly_predicted_taken <= "      $$m_temp | sed -e 's/_M_StaticPred.incorrectly_predicted_taken <= \([0-9]\+\)/\1/'`      ; \
	 m_sCPNT=`grep "_M_StaticPred.correctly_predicted_not_taken <= "    $$m_temp | sed -e 's/_M_StaticPred.correctly_predicted_not_taken <= \([0-9]\+\)/\1/'`    ; \
	 m_sCP_T=`grep "_M_StaticPred.correctly_predicted_taken <= "        $$m_temp | sed -e 's/_M_StaticPred.correctly_predicted_taken <= \([0-9]\+\)/\1/'`        ; \
	 m_dIPNT=`grep "_M_DynamicPred.incorrectly_predicted_not_taken <= " $$m_temp | sed -e 's/_M_DynamicPred.incorrectly_predicted_not_taken <= \([0-9]\+\)/\1/'` ; \
	 m_dIP_T=`grep "_M_DynamicPred.incorrectly_predicted_taken <= "     $$m_temp | sed -e 's/_M_DynamicPred.incorrectly_predicted_taken <= \([0-9]\+\)/\1/'`     ; \
	 m_dCPNT=`grep "_M_DynamicPred.correctly_predicted_not_taken <= "   $$m_temp | sed -e 's/_M_DynamicPred.correctly_predicted_not_taken <= \([0-9]\+\)/\1/'`   ; \
	 m_dCP_T=`grep "_M_DynamicPred.correctly_predicted_taken <= "       $$m_temp | sed -e 's/_M_DynamicPred.correctly_predicted_taken <= \([0-9]\+\)/\1/'`       ; \
	 m_dItrg=`grep "_M_DynamicPred.incorrect_target <= "                $$m_temp | sed -e 's/_M_DynamicPred.incorrect_target <= \([0-9]\+\)/\1/'`                ; \
\
	 echo -n "$$__file,$$__WCET,$$__staS,$$__staE,$$__cpuT,$$__memV,$$__memR," ; \
	 echo -n "$$m_BTBow,"                                                      ; \
	 echo -n "$$m_sIPNT,$$m_sIP_T,$$m_sCPNT,$$m_sCP_T,"                        ; \
	 echo -n "$$m_dIPNT,$$m_dIP_T,$$m_dCPNT,$$m_dCP_T,"                        ; \
	 echo    "$$m_dItrg"


# 	@s_model="$*.elf-model_sliced.xml"          ; \
# 	 __model="$*.elf-model.xml"                 ; \
# 	    file=`echo $$s_model | cut -d'.' -f1`   ; \
# 	  s_temp=`mktemp /tmp/best-makefile.XXXXXX` ; \
# 	  __temp=`mktemp /tmp/best-makefile.XXXXXX` ; \
# \
# 	 $(VERIFYTA) -u $(BIN_PATH)/$$s_model \
# 				$(WCET_QUERY) > $$s_temp 2> /dev/null  ; \
# 	 if [ "$$?" -eq 124 ]                                          ; \
# 	   then echo "TIMEOUT"; exit 0; fi                             ; \
# 	 s_abrt=`grep 'Aborted' $$s_temp`                              ; \
# 	 if [ "$$s_abrt" != "" ]                                       ; \
# 	   then echo "ABORTED"; exit 0; fi                             ; \
# 	 s_WCET=`grep "_clock <= "                  $$s_temp | sed -e 's/_clock <= \([0-9]\+\)/\1/'`                      ; \
# 	 s_staS=`grep " -- States stored : "        $$s_temp | sed -e 's/ -- States stored : \([0-9]\+\) states/\1/'`     ; \
# 	 s_staE=`grep " -- States explored : "      $$s_temp | sed -e 's/ -- States explored : \([0-9]\+\) states/\1/'`   ; \
# 	 s_cpuT=`grep " -- CPU user time used : "   $$s_temp | sed -e 's/ -- CPU user time used : \([0-9]\+\) ms/\1/'`    ; \
# 	 s_memV=`grep " -- Virtual memory used : "  $$s_temp | sed -e 's/ -- Virtual memory used : \([0-9]\+\) KiB/\1/'`  ; \
# 	 s_memR=`grep " -- Resident memory used : " $$s_temp | sed -e 's/ -- Resident memory used : \([0-9]\+\) KiB/\1/'` ; \
# \
# 	 $(VERIFYTA) -u $(BIN_PATH)/$$__model \
# 				$(WCET_QUERY) > $$__temp 2> /dev/null  ; \
# 	 if [ "$$?" -eq 124 ]                                          ; \
# 	   then echo "TIMEOUT"; exit 0; fi                             ; \
# 	 __abrt=`grep 'Aborted' $$__temp`                              ; \
# 	 if [ "$$__abrt" != "" ]                                       ; \
# 	   then echo "ABORTED"; exit 0; fi                             ; \
# 	 __WCET=`grep "_clock <= "                  $$s_temp | sed -e 's/_clock <= \([0-9]\+\)/\1/'`                      ; \
# 	 __staS=`grep " -- States stored : "        $$s_temp | sed -e 's/ -- States stored : \([0-9]\+\) states/\1/'`     ; \
# 	 __staE=`grep " -- States explored : "      $$s_temp | sed -e 's/ -- States explored : \([0-9]\+\) states/\1/'`   ; \
# 	 __cpuT=`grep " -- CPU user time used : "   $$s_temp | sed -e 's/ -- CPU user time used : \([0-9]\+\) ms/\1/'`    ; \
# 	 __memV=`grep " -- Virtual memory used : "  $$s_temp | sed -e 's/ -- Virtual memory used : \([0-9]\+\) KiB/\1/'`  ; \
# 	 __memR=`grep " -- Resident memory used : " $$s_temp | sed -e 's/ -- Resident memory used : \([0-9]\+\) KiB/\1/'` ; \
# \
# 	 if [ "$$s_WCET" -ne "$$__WCET" ]    ; \
# 	   then echo "DIVERGING"; exit 0; fi ; \
# 	 echo "$$file,$$__WCET,$$__staS,$$__staE,$$__cpuT,$$__memV,$$__memR"

labels:
	@echo -n "Benchmark,WCET (cycles),States stored,States explored,CPU user time used (ms),Virtual memory used (KiB),Resident memory used (KiB),"
	@echo -n "BTB overwites,"
	@echo -n "Incorrect pred. not-taken (static),Incorrect pred. taken (static),Correct pred. not-taken (static),Correct pred. taken (static),"
	@echo -n "Incorrect pred. not-taken (dynamic),Incorrect pred. taken (dynamic),Correct pred. not-taken (dynamic),Correct pred. taken (dynamic),"
	@echo    "Incorrect target (dynamic)"

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

