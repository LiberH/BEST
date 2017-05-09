GENERATE = $(MAKE) --no-print-directory -C $(BEST_BENCHMARKS_PATH) generate
VERIFY   = $(MAKE) --no-print-directory -C $(BEST_BENCHMARKS_PATH) verify

MODELS_DIR          = $(CURDIR)/src
BINS_DIR            = $(CURDIR)/bin
ALWAYS_NOTTAKEN_DIR = $(MODELS_DIR)/always_not-taken
BACKWARD_TAKEN_DIR  = $(MODELS_DIR)/backward_taken
DYNAMIC_PRED_DIR    = $(MODELS_DIR)/dynamic_pred
DEFAULT_DIR	    = $(ALWAYS_NOTTAKEN_DIR)
DIRS                = $(ALWAYS_NOTTAKEN_DIR) $(BACKWARD_TAKEN_DIR) $(DYNAMIC_PRED_DIR)

# Generates:
TEMPLATE   = $(BEST_PLUGINS_PATH)/$(BEST_PLUGIN)/template_opt.xml
BENCH_BINS = $(BEST_BENCHMARKS_PATH)/bin/gcc/malardalen
generate:
	@cp $(BENCH_BINS)/*.elf $(BINS_DIR)
	@rm -f $(BINS_DIR)/*-O3.elf           # Not used
	@rm -f $(BINS_DIR)/*-O0.elf           # Needs stack access
	@rm -f $(BINS_DIR)/*-O0.elf           # Needs stack access
	@rm -f $(BINS_DIR)/cnt-O1.elf         # Needs stack access
	@rm -f $(BINS_DIR)/ud-O1.elf          # Needs stack access
	@rm -f $(BINS_DIR)/insertsort-O1.elf  # Stall bug (from HARMLESS)
	@rm -f $(BINS_DIR)/adpcm-O?.elf       # Too big to verify (?)
	@rm -f $(BINS_DIR)/compress-O?.elf    # Too big to verify (?)
	@rm -f $(BINS_DIR)/matmult-O?.elf     # Too big to verify (?)
	@rm -f $(BINS_DIR)/ndes-O?.elf        # Too big to verify (?)

	@$(GENERATE) BIN_PATH=$(BINS_DIR)	\
		     OUTPUT_DIR=$(DEFAULT_DIR)	\
		     BEST_TEMPLATE=$(TEMPLATE)

	@rm -f $(DEFAULT_DIR)/*-model.xml
	@rm -f $(DEFAULT_DIR)/*.dot

	@for default_sliced in $(DEFAULT_DIR)/*_sliced.xml; do                               \
	   basename=`basename $$default_sliced`;                                             \
	   backward_taken_sliced=`echo $(BACKWARD_TAKEN_DIR)/$$basename`;                    \
	   cp $$default_sliced $$backward_taken_sliced;                                      \
	   sed -i 's/BACKWARD_TAKEN = false/BACKWARD_TAKEN = true/' $$backward_taken_sliced; \
                                                                                             \
	   dynamic_pred_sliced=`echo $(DYNAMIC_PRED_DIR)/$$basename`;                        \
	   cp $$default_sliced $$dynamic_pred_sliced;                                        \
	   sed -i 's/BACKWARD_TAKEN = false/BACKWARD_TAKEN = true/' $$dynamic_pred_sliced;   \
	   sed -i 's/DYNAMIC_PRED = false/DYNAMIC_PRED = true/' $$dynamic_pred_sliced;       \
	 done

	@for dir in $(DIRS); do                \
	   for model_sliced in $${dir}/*_sliced.xml; do                          \
	     model_monitored=`echo $${model_sliced%_*}_monitored.xml`;           \
	     cp $$model_sliced $$model_monitored;                                \
	     sed -i 's/MONITORING = false/MONITORING = true/' $$model_monitored; \
	   done;                                                                 \
	 done

verify:
	@for dir in $(DIRS); do                    \
           echo "Verify for $${dir}:";             \
	   output_file=`echo $${dir}/results.csv`; \
	   $(VERIFY) INPUT_DIR=$$dir               \
		     OUTPUT_FILE=$$output_file;    \
	 done

# Cleaning:
clean:
	rm -f $(BINS_DIR)/*
	rm -f $(ALWAYS_NOTTAKEN_DIR)/*
	rm -f $(BACKWARD_TAKEN_DIR)/*
	rm -f $(DYNAMIC_PRED_DIR)/*
