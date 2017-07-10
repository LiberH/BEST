CC      = powerpc-linux-gnu-gcc-5
AS      = powerpc-linux-gnu-as
CFLAGS	= -m32 -std=c89 -ggdb -ffreestanding
LDFLAGS = -Wl,-T$(SRC)/script.ld,$(OBJ)/crt0.o -nostdlib

SRC = $(CURDIR)/src
OBJ = $(CURDIR)/obj
BIN = $(CURDIR)/bin

SRC_MALARDALEN = $(SRC)/malardalen
SRC_TACLE      = $(SRC)/tacle
SRC_TRAMPOLINE = $(SRC)/trampoline

BIN_GCC             = $(BIN)/gcc
BIN_GCC_MALARDALEN  = $(BIN_GCC)/malardalen
BIN_GCC_TACLE       = $(BIN_GCC)/tacle
#BIN_GCC_TRAMPOLINE  = $(BIN_GCC)/trampoline

#BIN_COSMIC            = $(BIN)/cosmic
#BIN_COSMIC_MALARDALEN = $(BIN_COSMIC)/malardalen
#BIN_COSMIC_TACLE      = $(BIN_COSMIC)/tacle
#BIN_COSMIC_TRAMPOLINE = $(BIN_COSMIC)/trampoline

all: dirs gcc #cosmic
DIRS =	$(OBJ)			\
	$(BIN)			\
	$(BIN_GCC)		\
	$(BIN_GCC_MALARDALEN)	\
	$(BIN_GCC_TACLE)	
####### $(BIN_GCC_TRAMPOLINE)
dirs: | $(OBJ)
$(DIRS):
	mkdir -p $(DIRS)

# Compiling with GCC:
gcc: gcc-crt0 gcc-malardalen gcc-tacle #gcc-trampoline

gcc-crt0: dirs $(OBJ)/crt0.o
$(OBJ)/crt0.o: $(SRC)/crt0.S
	$(AS) $< -o $@

GCC_MALARDALEN_BINS =	$(BIN_GCC_MALARDALEN)/adpcm		\
			$(BIN_GCC_MALARDALEN)/bs		\
			$(BIN_GCC_MALARDALEN)/bsort100		\
			$(BIN_GCC_MALARDALEN)/cnt		\
			$(BIN_GCC_MALARDALEN)/compress		\
			$(BIN_GCC_MALARDALEN)/crc		\
			$(BIN_GCC_MALARDALEN)/edn		\
			$(BIN_GCC_MALARDALEN)/expint		\
			$(BIN_GCC_MALARDALEN)/fdct		\
			$(BIN_GCC_MALARDALEN)/fibcall		\
			$(BIN_GCC_MALARDALEN)/fir		\
			$(BIN_GCC_MALARDALEN)/insertsort	\
			$(BIN_GCC_MALARDALEN)/janne_complex	\
			$(BIN_GCC_MALARDALEN)/jfdctint		\
			$(BIN_GCC_MALARDALEN)/matmult		\
			$(BIN_GCC_MALARDALEN)/ndes		\
			$(BIN_GCC_MALARDALEN)/ns		\
			$(BIN_GCC_MALARDALEN)/nsichneu		\
			$(BIN_GCC_MALARDALEN)/prime		\
			$(BIN_GCC_MALARDALEN)/ud
gcc-malardalen: gcc-crt0 $(GCC_MALARDALEN_BINS)
$(BIN_GCC_MALARDALEN)/%: $(SRC_MALARDALEN)/%.c
	@echo -n "Compiling `basename $@.c` ."
	@$(CC) $< -o $@-O0.elf -O0 $(CFLAGS) $(LDFLAGS); echo -n "."
	@$(CC) $< -o $@-O1.elf -O1 $(CFLAGS) $(LDFLAGS); echo -n "."
	@$(CC) $< -o $@-O2.elf -O2 $(CFLAGS) $(LDFLAGS); echo -n "."
	@$(CC) $< -o $@-O3.elf -O3 $(CFLAGS) $(LDFLAGS); echo    " "

GCC_TACLE_BINS =	$(BIN_GCC_TACLE)/adpcm_dec	\
			$(BIN_GCC_TACLE)/adpcm_enc	\
			$(BIN_GCC_TACLE)/ammunition	\
			$(BIN_GCC_TACLE)/binarysearch	\
			$(BIN_GCC_TACLE)/bsort		\
			$(BIN_GCC_TACLE)/cjpeg_transupp	\
			$(BIN_GCC_TACLE)/cjpeg_wrbmp	\
			$(BIN_GCC_TACLE)/countnegative	\
			$(BIN_GCC_TACLE)/dijkstra	\
			$(BIN_GCC_TACLE)/h264_dec	\
			$(BIN_GCC_TACLE)/huff_dec	\
			$(BIN_GCC_TACLE)/huff_enc	\
			$(BIN_GCC_TACLE)/insertsort	\
			$(BIN_GCC_TACLE)/jfdctint	\
			$(BIN_GCC_TACLE)/lift		\
			$(BIN_GCC_TACLE)/matrix1	\
			$(BIN_GCC_TACLE)/md5		\
			$(BIN_GCC_TACLE)/mpeg2		\
			$(BIN_GCC_TACLE)/ndes		\
			$(BIN_GCC_TACLE)/petrinet	\
			$(BIN_GCC_TACLE)/prime		\
			$(BIN_GCC_TACLE)/test3          
gcc-tacle: gcc-crt0 $(GCC_TACLE_BINS)
$(BIN_GCC_TACLE)/ammunition:
	$(eval AMMUNITION_LIBC_C = $(SRC_TACLE)/ammunition/ammunition_libc.c)
	$(eval ARITHM_C          = $(SRC_TACLE)/ammunition/arithm.c)
	$(eval BITS_C            = $(SRC_TACLE)/ammunition/bits.c)
	$(eval AMMUNITION_C      = $(SRC_TACLE)/ammunition/ammunition.c)

	@echo -n "Compiling `basename $@.c` ."
	@$(CC) -c $(AMMUNITION_LIBC_C) -o $(OBJ)/ammunition_libc-O0.o -O0 $(CFLAGS)
	@$(CC) -c $(ARITHM_C)          -o $(OBJ)/arithm-O0.o          -O0 $(CFLAGS)
	@$(CC) -c $(BITS_C)            -o $(OBJ)/bits-O0.o            -O0 $(CFLAGS)
	@$(CC) -c $(AMMUNITION_C)      -o $(OBJ)/ammunition-O0.o      -O0 $(CFLAGS)
	@$(CC) $(OBJ)/ammunition_libc-O0.o	\
	       $(OBJ)/arithm-O0.o		\
	       $(OBJ)/bits-O0.o			\
	       $(OBJ)/ammunition-O0.o		-o $@-O0.elf $(LDFLAGS)
	@echo -n "."

	@$(CC) -c $(AMMUNITION_LIBC_C) -o $(OBJ)/ammunition_libc-O1.o -O1 $(CFLAGS)
	@$(CC) -c $(ARITHM_C)          -o $(OBJ)/arithm-O1.o          -O1 $(CFLAGS)
	@$(CC) -c $(BITS_C)            -o $(OBJ)/bits-O1.o            -O1 $(CFLAGS)
	@$(CC) -c $(AMMUNITION_C)      -o $(OBJ)/ammunition-O1.o      -O1 $(CFLAGS)
	@$(CC) $(OBJ)/ammunition_libc-O1.o	\
	       $(OBJ)/arithm-O1.o		\
	       $(OBJ)/bits-O1.o			\
	       $(OBJ)/ammunition-O1.o		-o $@-O1.elf $(LDFLAGS)
	@echo -n "."

	@$(CC) -c $(AMMUNITION_LIBC_C) -o $(OBJ)/ammunition_libc-O2.o -O2 $(CFLAGS)
	@$(CC) -c $(ARITHM_C)          -o $(OBJ)/arithm-O2.o          -O2 $(CFLAGS)
	@$(CC) -c $(BITS_C)            -o $(OBJ)/bits-O2.o            -O2 $(CFLAGS)
	@$(CC) -c $(AMMUNITION_C)      -o $(OBJ)/ammunition-O2.o      -O2 $(CFLAGS)
	@$(CC) $(OBJ)/ammunition_libc-O2.o	\
	       $(OBJ)/arithm-O2.o		\
	       $(OBJ)/bits-O2.o			\
	       $(OBJ)/ammunition-O2.o		-o $@-O2.elf $(LDFLAGS)
	@echo -n "."

	@$(CC) -c $(AMMUNITION_LIBC_C) -o $(OBJ)/ammunition_libc-O3.o -O3 $(CFLAGS)
	@$(CC) -c $(ARITHM_C)          -o $(OBJ)/arithm-O3.o          -O3 $(CFLAGS)
	@$(CC) -c $(BITS_C)            -o $(OBJ)/bits-O3.o            -O3 $(CFLAGS)
	@$(CC) -c $(AMMUNITION_C)      -o $(OBJ)/ammunition-O3.o      -O3 $(CFLAGS)
	@$(CC) $(OBJ)/ammunition_libc-O3.o	\
	       $(OBJ)/arithm-O3.o		\
	       $(OBJ)/bits-O3.o			\
	       $(OBJ)/ammunition-O3.o		-o $@-O3.elf $(LDFLAGS)
	@echo ""

$(BIN_GCC_TACLE)/cjpeg_wrbmp:
	$(eval INPUT_C    = $(SRC_TACLE)/cjpeg_wrbmp/input.c)
	$(eval CJPEG_WRBMP_C = $(SRC_TACLE)/cjpeg_wrbmp/cjpeg_wrbmp.c)

	@echo -n "Compiling `basename $@.c` ."
	@$(CC) -c $(INPUT_C)       -o $(OBJ)/input-O0.o       -O0 $(CFLAGS)
	@$(CC) -c $(CJPEG_WRBMP_C) -o $(OBJ)/cjpeg_wrbmp-O0.o -O0 $(CFLAGS)
	@$(CC) $(OBJ)/input-O0.o	\
	       $(OBJ)/cjpeg_wrbmp-O0.o	-o $@-O0.elf $(LDFLAGS)

	@echo -n "."
	@$(CC) -c $(INPUT_C)       -o $(OBJ)/input-O1.o       -O1 $(CFLAGS)
	@$(CC) -c $(CJPEG_WRBMP_C) -o $(OBJ)/cjpeg_wrbmp-O1.o -O1 $(CFLAGS)
	@$(CC) $(OBJ)/input-O1.o	\
	       $(OBJ)/cjpeg_wrbmp-O1.o	-o $@-O1.elf $(LDFLAGS)

	@echo -n "."
	@$(CC) -c $(INPUT_C)       -o $(OBJ)/input-O2.o       -O2 $(CFLAGS)
	@$(CC) -c $(CJPEG_WRBMP_C) -o $(OBJ)/cjpeg_wrbmp-O2.o -O2 $(CFLAGS)
	@$(CC) $(OBJ)/input-O2.o	\
	       $(OBJ)/cjpeg_wrbmp-O2.o	-o $@-O2.elf $(LDFLAGS)

	@echo -n "."
	@$(CC) -c $(INPUT_C)       -o $(OBJ)/input-O3.o       -O3 $(CFLAGS)
	@$(CC) -c $(CJPEG_WRBMP_C) -o $(OBJ)/cjpeg_wrbmp-O3.o -O3 $(CFLAGS)
	@$(CC) $(OBJ)/input-O3.o	\
	       $(OBJ)/cjpeg_wrbmp-O3.o	-o $@-O3.elf $(LDFLAGS)
	@echo ""

$(BIN_GCC_TACLE)/dijkstra:
	$(eval INPUT_C    = $(SRC_TACLE)/dijkstra/input.c)
	$(eval DIJKSTRA_C = $(SRC_TACLE)/dijkstra/dijkstra.c)

	@echo -n "Compiling `basename $@.c` ."
	@$(CC) -c $(INPUT_C)    -o $(OBJ)/input-O0.o    -O0 $(CFLAGS)
	@$(CC) -c $(DIJKSTRA_C) -o $(OBJ)/dijkstra-O0.o -O0 $(CFLAGS)
	@$(CC) $(OBJ)/input-O0.o	\
	       $(OBJ)/dijkstra-O0.o	-o $@-O0.elf $(LDFLAGS)

	@echo -n "."
	@$(CC) -c $(INPUT_C)    -o $(OBJ)/input-O1.o    -O1 $(CFLAGS)
	@$(CC) -c $(DIJKSTRA_C) -o $(OBJ)/dijkstra-O1.o -O1 $(CFLAGS)
	@$(CC) $(OBJ)/input-O1.o	\
	       $(OBJ)/dijkstra-O1.o	-o $@-O1.elf $(LDFLAGS)

	@echo -n "."
	@$(CC) -c $(INPUT_C)    -o $(OBJ)/input-O2.o    -O2 $(CFLAGS)
	@$(CC) -c $(DIJKSTRA_C) -o $(OBJ)/dijkstra-O2.o -O2 $(CFLAGS)
	@$(CC) $(OBJ)/input-O2.o	\
	       $(OBJ)/dijkstra-O2.o	-o $@-O2.elf $(LDFLAGS)

	@echo -n "."
	@$(CC) -c $(INPUT_C)    -o $(OBJ)/input-O3.o    -O3 $(CFLAGS)
	@$(CC) -c $(DIJKSTRA_C) -o $(OBJ)/dijkstra-O3.o -O3 $(CFLAGS)
	@$(CC) $(OBJ)/input-O3.o	\
	       $(OBJ)/dijkstra-O3.o	-o $@-O3.elf $(LDFLAGS)
	@echo ""

$(BIN_GCC_TACLE)/h264_dec:
	$(eval H264_DECINPUT_C = $(SRC_TACLE)/h264_dec/h264_decinput.c)
	$(eval H264_DEC_C      = $(SRC_TACLE)/h264_dec/h264_dec.c)

	@echo -n "Compiling `basename $@.c` ."
	@$(CC) -c $(H264_DECINPUT_C) -o $(OBJ)/h264_decinput-O0.o -O0 $(CFLAGS)
	@$(CC) -c $(H264_DEC_C)      -o $(OBJ)/h264_dec-O0.o      -O0 $(CFLAGS)
	@$(CC) $(OBJ)/h264_decinput-O0.o	\
	       $(OBJ)/h264_dec-O0.o		-o $@-O0.elf $(LDFLAGS)

	@echo -n "."
	@$(CC) -c $(H264_DECINPUT_C) -o $(OBJ)/h264_decinput-O1.o -O1 $(CFLAGS)
	@$(CC) -c $(H264_DEC_C)      -o $(OBJ)/h264_dec-O1.o      -O1 $(CFLAGS)
	@$(CC) $(OBJ)/h264_decinput-O1.o	\
	       $(OBJ)/h264_dec-O1.o		-o $@-O1.elf $(LDFLAGS)

	@echo -n "."
	@$(CC) -c $(H264_DECINPUT_C) -o $(OBJ)/h264_decinput-O2.o -O2 $(CFLAGS)
	@$(CC) -c $(H264_DEC_C)      -o $(OBJ)/h264_dec-O2.o      -O2 $(CFLAGS)
	@$(CC) $(OBJ)/h264_decinput-O2.o	\
	       $(OBJ)/h264_dec-O2.o		-o $@-O2.elf $(LDFLAGS)

	@echo -n "."
	@$(CC) -c $(H264_DECINPUT_C) -o $(OBJ)/h264_decinput-O3.o -O3 $(CFLAGS)
	@$(CC) -c $(H264_DEC_C)      -o $(OBJ)/h264_dec-O3.o      -O3 $(CFLAGS)
	@$(CC) $(OBJ)/h264_decinput-O3.o	\
	       $(OBJ)/h264_dec-O3.o		-o $@-O3.elf $(LDFLAGS)
	@echo ""

$(BIN_GCC_TACLE)/lift:
	$(eval LIFTLIBCONTROL_C = $(SRC_TACLE)/lift/liftlibcontrol.c)
	$(eval LIFTLIBIO_C      = $(SRC_TACLE)/lift/liftlibio.c)
	$(eval LIFT_C           = $(SRC_TACLE)/lift/lift.c)

	@echo -n "Compiling `basename $@.c` ."
	@$(CC) -c $(LIFTLIBCONTROL_C) -o $(OBJ)/liftlibcontrol-O0.o -O0 $(CFLAGS)
	@$(CC) -c $(LIFTLIBIO_C)      -o $(OBJ)/liftlibio-O0.o      -O0 $(CFLAGS)
	@$(CC) -c $(LIFT_C)           -o $(OBJ)/lift-O0.o           -O0 $(CFLAGS)
	@$(CC) $(OBJ)/liftlibcontrol-O0.o	\
	       $(OBJ)/liftlibio-O0.o		\
	       $(OBJ)/lift-O0.o			-o $@-O0.elf $(LDFLAGS)

	@echo -n "."
	@$(CC) -c $(LIFTLIBCONTROL_C) -o $(OBJ)/liftlibcontrol-O1.o -O1 $(CFLAGS)
	@$(CC) -c $(LIFTLIBIO_C)      -o $(OBJ)/liftlibio-O1.o      -O1 $(CFLAGS)
	@$(CC) -c $(LIFT_C)           -o $(OBJ)/lift-O1.o           -O1 $(CFLAGS)
	@$(CC) $(OBJ)/liftlibcontrol-O1.o	\
	       $(OBJ)/liftlibio-O1.o		\
	       $(OBJ)/lift-O1.o			-o $@-O1.elf $(LDFLAGS)

	@echo -n "."
	@$(CC) -c $(LIFTLIBCONTROL_C) -o $(OBJ)/liftlibcontrol-O2.o -O2 $(CFLAGS)
	@$(CC) -c $(LIFTLIBIO_C)      -o $(OBJ)/liftlibio-O2.o      -O2 $(CFLAGS)
	@$(CC) -c $(LIFT_C)           -o $(OBJ)/lift-O2.o           -O2 $(CFLAGS)
	@$(CC) $(OBJ)/liftlibcontrol-O2.o	\
	       $(OBJ)/liftlibio-O2.o		\
	       $(OBJ)/lift-O2.o			-o $@-O2.elf $(LDFLAGS)

	@echo -n "."
	@$(CC) -c $(LIFTLIBCONTROL_C) -o $(OBJ)/liftlibcontrol-O3.o -O3 $(CFLAGS)
	@$(CC) -c $(LIFTLIBIO_C)      -o $(OBJ)/liftlibio-O3.o      -O3 $(CFLAGS)
	@$(CC) -c $(LIFT_C)           -o $(OBJ)/lift-O3.o           -O3 $(CFLAGS)
	@$(CC) $(OBJ)/liftlibcontrol-O3.o	\
	       $(OBJ)/liftlibio-O3.o		\
	       $(OBJ)/lift-O3.o			-o $@-O3.elf $(LDFLAGS)
	@echo ""

$(BIN_GCC_TACLE)/%:
	$(eval DIRNAME  = $(shell basename $@))
	$(eval FILENAME = $(shell echo "`basename $@`.c"))
	$(eval FILE     = $(SRC_TACLE)/$(DIRNAME)/$(FILENAME))

	@echo -n "Compiling `basename $@.c` ."
	@$(CC) $(FILE) -o $@-O0.elf -O0 $(CFLAGS) $(LDFLAGS); echo -n "."
	@$(CC) $(FILE) -o $@-O1.elf -O1 $(CFLAGS) $(LDFLAGS); echo -n "."
	@$(CC) $(FILE) -o $@-O2.elf -O2 $(CFLAGS) $(LDFLAGS); echo -n "."
	@$(CC) $(FILE) -o $@-O3.elf -O3 $(CFLAGS) $(LDFLAGS); echo    " "


# Cleaning:
clean:
	rm -rf $(OBJ)
	rm -rf $(BIN_GCC_MALARDALEN)
	rm -rf $(BIN_GCC_TACLE)
####### rm -rf $(BIN_GCC_TRAMPOLINE)
####### rm -rf $(BIN_COSMIC)

##############
##############
##############

QUERIES = queries
WCET_QUERY = $(QUERIES)/wcet.q
DATA_QUERY = $(QUERIES)/data.q

MAKE_NPD    = $(MAKE) --no-print-directory
VERIFYTA    = $(UPPAAL_PATH)/bin-Linux/verifyta

BEST        = $(BEST_PATH)/bin/main
BIN_PATH    = $(BIN_MALARDALEN)
OUTPUT_DIR  = $(BIN_PATH)
INPUT_DIR   = $(BIN_PATH)
TMP_FILE    = 
OUTPUT_FILE = /dev/null

# Generating:
generate:
	@for f in $(BIN_PATH)/*.elf                ; \
	 do basename=`basename $$f`                ; \
	    name=`echo $$basename | cut -d'.' -f1` ; \
	    target="generate/$$name"               ; \
	    $(MAKE_NPD) $$target  ; \
	 done

generate/%:
	@$(BEST) --output-dir=$(OUTPUT_DIR) --cfg-only $(BIN_PATH)/$*.elf
	@dot -Tdot $(OUTPUT_DIR)/$*.elf-cfg.dot -o $(OUTPUT_DIR)/$*.elf.dot
	@$(BEST) --output-dir=$(OUTPUT_DIR) --template-file=$(BEST_TEMPLATE) $(BIN_PATH)/$*.elf

# Verifying:
verify:
	$(eval TMP_FILE = $(shell mktemp /tmp/best-benchmark.XXXXX))
	@$(MAKE_NPD) labels > $(OUTPUT_FILE)
	@for f in $(INPUT_DIR)/*_sliced.xml; do                                             \
	   basename=`basename $$f`;                                                         \
	   name=`echo $$basename | cut -d'.' -f1`;                                          \
	   $(MAKE_NPD)   TMP_FILE=$(TMP_FILE)        verify/$$name;                         \
	   $(MAKE_NPD) INPUT_FILE=$(TMP_FILE) parse-results/$$name | tee -a $(OUTPUT_FILE); \
	 done

TIMEOUT=500
verify/%:
ifndef TMP_FILE
	$(eval TMP_FILE := `mktemp /tmp/best-benchmark.XXXXX`)
endif

	$(eval MODEL_WCET := $(INPUT_DIR)/"$*.elf-model_sliced.xml")
	$(eval MODEL_DATA := $(INPUT_DIR)/"$*.elf-model_monitored.xml")
	@timeout $(TIMEOUT) $(VERIFYTA) -u $(MODEL_WCET) $(WCET_QUERY)  > $(TMP_FILE) 2> /dev/null
	@timeout $(TIMEOUT) $(VERIFYTA)    $(MODEL_DATA) $(DATA_QUERY) >> $(TMP_FILE) 2> /dev/null

parse-results/%:
	@           wcet=`grep "_clock <= "                                         $(INPUT_FILE) | sed -e 's/_clock <= \([0-9]\+\)/\1/'`;                                         \
	   states_stored=`grep " -- States stored : "                               $(INPUT_FILE) | sed -e 's/ -- States stored : \([0-9]\+\) states/\1/'`;                        \
	 states_explored=`grep " -- States explored : "                             $(INPUT_FILE) | sed -e 's/ -- States explored : \([0-9]\+\) states/\1/'`;                      \
	        cpu_time=`grep " -- CPU user time used : "                          $(INPUT_FILE) | sed -e 's/ -- CPU user time used : \([0-9]\+\) ms/\1/'`;                       \
	     virtual_mem=`grep " -- Virtual memory used : "                         $(INPUT_FILE) | sed -e 's/ -- Virtual memory used : \([0-9]\+\) KiB/\1/'`;                     \
	    resident_mem=`grep " -- Resident memory used : "                        $(INPUT_FILE) | sed -e 's/ -- Resident memory used : \([0-9]\+\) KiB/\1/'`;                    \
	                                                                                                                                                                           \
	      icache_hit=`grep "Monitoring.icache_hit <= "                                   $(INPUT_FILE) | sed -e 's/Monitoring.icache_hit <= \([0-9]\+\)/\1/'`;                                   \
	     icache_miss=`grep "Monitoring.icache_miss <= "                                  $(INPUT_FILE) | sed -e 's/Monitoring.icache_miss <= \([0-9]\+\)/\1/'`;                                  \
	                                                                                                                                                                           \
	      static_cpt=`grep "Monitoring.static_pred.correctly_predicted_taken <= "        $(INPUT_FILE) | sed -e 's/Monitoring.static_pred.correctly_predicted_taken <= \([0-9]\+\)/\1/'`;        \
	      static_ipt=`grep "Monitoring.static_pred.incorrectly_predicted_taken <= "      $(INPUT_FILE) | sed -e 's/Monitoring.static_pred.incorrectly_predicted_taken <= \([0-9]\+\)/\1/'`;      \
	     static_cpnt=`grep "Monitoring.static_pred.correctly_predicted_not_taken <= "    $(INPUT_FILE) | sed -e 's/Monitoring.static_pred.correctly_predicted_not_taken <= \([0-9]\+\)/\1/'`;    \
	     static_ipnt=`grep "Monitoring.static_pred.incorrectly_predicted_not_taken <= "  $(INPUT_FILE) | sed -e 's/Monitoring.static_pred.incorrectly_predicted_not_taken <= \([0-9]\+\)/\1/'`;  \
	                                                                                                                                                                           \
	     dynamic_cpt=`grep "Monitoring.dynamic_pred.correctly_predicted_taken <= "       $(INPUT_FILE) | sed -e 's/Monitoring.dynamic_pred.correctly_predicted_taken <= \([0-9]\+\)/\1/'`;       \
	     dynamic_ipt=`grep "Monitoring.dynamic_pred.incorrectly_predicted_taken <= "     $(INPUT_FILE) | sed -e 's/Monitoring.dynamic_pred.incorrectly_predicted_taken <= \([0-9]\+\)/\1/'`;     \
	    dynamic_cpnt=`grep "Monitoring.dynamic_pred.correctly_predicted_not_taken <= "   $(INPUT_FILE) | sed -e 's/Monitoring.dynamic_pred.correctly_predicted_not_taken <= \([0-9]\+\)/\1/'`;   \
	    dynamic_ipnt=`grep "Monitoring.dynamic_pred.incorrectly_predicted_not_taken <= " $(INPUT_FILE) | sed -e 's/Monitoring.dynamic_pred.incorrectly_predicted_not_taken <= \([0-9]\+\)/\1/'`; \
	                                                                                                                                                                           \
	      dynamic_it=`grep "Monitoring.dynamic_pred.incorrect_target <= "                $(INPUT_FILE) | sed -e 's/Monitoring.dynamic_pred.incorrect_target <= \([0-9]\+\)/\1/'`;                \
	  btb_overwrites=`grep "Monitoring.btb_overwriting <= "                              $(INPUT_FILE) | sed -e 's/Monitoring.btb_overwriting <= \([0-9]\+\)/\1/'`;                              \
	                                                                                                                                                                           \
	     ibuff_poped=`grep "Monitoring.ibuff_pop <= "                                    $(INPUT_FILE) | sed -e 's/Monitoring.ibuff_pop <= \([0-9]\+\)/\1/'`;                                    \
	     ibuff_flush=`grep "Monitoring.ibuff_flush <= "                                  $(INPUT_FILE) | sed -e 's/Monitoring.ibuff_flush <= \([0-9]\+\)/\1/'`;                                  \
	   ibuff_flushed=`grep "Monitoring.ibuff_flushed_inst <= "                           $(INPUT_FILE) | sed -e 's/Monitoring.ibuff_flushed_inst <= \([0-9]\+\)/\1/'`;                            \
	   pipeline_exec=`grep "Monitoring.executed_inst <= "                                $(INPUT_FILE) | sed -e 's/Monitoring.executed_inst <= \([0-9]\+\)/\1/'`;                                \
	                                                                                                                                                                           \
	 echo            "$*," \
	             "$$wcet," \
	    "$$states_stored," \
	  "$$states_explored," \
	         "$$cpu_time," \
	      "$$virtual_mem," \
	     "$$resident_mem," \
	       "$$icache_hit," \
	      "$$icache_miss," \
	       "$$static_cpt," \
	       "$$static_ipt," \
	      "$$static_cpnt," \
	      "$$static_ipnt," \
	      "$$dynamic_cpt," \
	      "$$dynamic_ipt," \
	     "$$dynamic_cpnt," \
	     "$$dynamic_ipnt," \
	       "$$dynamic_it," \
	   "$$btb_overwrites," \
	      "$$ibuff_poped," \
	      "$$ibuff_flush," \
	    "$$ibuff_flushed," \
	    "$$pipeline_exec "
labels:
	@echo	     "bench.," \
	               "WCET," \
	         "st. stored," \
	       "st. explored," \
	      "CPU time (ms)," \
	   "virt. mem. (KiB)," \
	    "res. mem. (KiB)," \
	          "cache hit," \
	         "cache miss," \
	   "OK stat. pred. T," \
	   "KO stat. pred. T," \
	  "OK stat. pred. NT," \
	  "KO stat. pred. NT," \
	    "OK dyn. pred. T," \
	    "KO dyn. pred. T," \
	   "OK dyn. pred. NT," \
	   "KO dyn. pred. NT," \
	    "KO dyn. pred. @," \
	      "BTB overwites," \
	  "IBuff poped inst.," \
	  "IBuff flush count," \
	"IBuff flushed inst.," \
             "executed inst. "

##############
##############
##############



