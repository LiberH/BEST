GENERATE = $(MAKE) --no-print-directory -C $(BEST_BENCHMARKS_PATH) generate
VERIFY   = $(MAKE) --no-print-directory -C $(BEST_BENCHMARKS_PATH) verify

MODELS_DIR          = $(CURDIR)/src
BINS_DIR            = $(CURDIR)/bin
ALWAYS_NOTTAKEN_DIR = $(MODELS_DIR)/always_not-taken
BACKWARD_TAKEN_DIR  = $(MODELS_DIR)/backward_taken
DYNAMIC_PRED_DIR    = $(MODELS_DIR)/dynamic_pred
DEFAULT_DIR         = $(ALWAYS_NOTTAKEN_DIR)
DIRS                = $(ALWAYS_NOTTAKEN_DIR) $(BACKWARD_TAKEN_DIR) $(DYNAMIC_PRED_DIR)

# Generates:
TEMPLATE   = $(BEST_PLUGINS_PATH)/$(BEST_PLUGIN)/template_opt.xml
BENCH_BINS = $(BEST_BENCHMARKS_PATH)/bin/gcc/malardalen
generate:
	@cp $(BENCH_BINS)/*.elf $(BINS_DIR)
	@rm -f $(BINS_DIR)/nsichneu-O?.elf    # Too big to process with BEST

	@$(GENERATE) BIN_PATH=$(BINS_DIR)  \
             OUTPUT_DIR=$(DEFAULT_DIR) \
             BEST_TEMPLATE=$(TEMPLATE)

	@for model in $(DEFAULT_DIR)/*-model.xml; do                                  \
       basename=`basename $$model`;                                               \
       backward_taken=`echo $(BACKWARD_TAKEN_DIR)/$$basename`;                    \
       cp $$model $$backward_taken;                                               \
       sed -i 's/BACKWARD_TAKEN = false/BACKWARD_TAKEN = true/' $$backward_taken; \
                                                                                  \
       dynamic_pred=`echo $(DYNAMIC_PRED_DIR)/$$basename`;                        \
       cp $$model $$dynamic_pred;                                                 \
       sed -i 's/BACKWARD_TAKEN = false/BACKWARD_TAKEN = true/' $$dynamic_pred;   \
       sed -i 's/DYNAMIC_PRED = false/DYNAMIC_PRED = true/' $$dynamic_pred;       \
     done

	@for model_sliced in $(DEFAULT_DIR)/*-model_sliced.xml; do                           \
       basename=`basename $$model_sliced`;                                               \
       backward_taken_sliced=`echo $(BACKWARD_TAKEN_DIR)/$$basename`;                    \
       cp $$model_sliced $$backward_taken_sliced;                                        \
       sed -i 's/BACKWARD_TAKEN = false/BACKWARD_TAKEN = true/' $$backward_taken_sliced; \
                                                                                         \
       dynamic_pred_sliced=`echo $(DYNAMIC_PRED_DIR)/$$basename`;                        \
       cp $$model_sliced $$dynamic_pred_sliced;                                          \
       sed -i 's/BACKWARD_TAKEN = false/BACKWARD_TAKEN = true/' $$dynamic_pred_sliced;   \
       sed -i 's/DYNAMIC_PRED = false/DYNAMIC_PRED = true/' $$dynamic_pred_sliced;       \
     done

	@for dir in $(DIRS); do                                                  \
       for model in $${dir}/*-model.xml; do                                  \
         model_monitored=`echo $${model%-model.xml}-model_monitored.xml`;    \
         cp $$model $$model_monitored;                                       \
         sed -i 's/MONITORING = false/MONITORING = true/' $$model_monitored; \
       done;                                                                 \
     done

	@for dir in $(DIRS); do                \
       for model_sliced in $${dir}/*-model_sliced.xml; do                                             \
         model_sliced_monitored=`echo $${model_sliced%-model_sliced.xml}-model_sliced_monitored.xml`; \
         cp $$model_sliced $$model_sliced_monitored;                                                  \
         sed -i 's/MONITORING = false/MONITORING = true/' $$model_sliced_monitored;                   \
       done;                                                                                          \
     done

verify:
	@for dir in $(DIRS); do                                 \
       echo "Verify for $${dir}:";                          \
       output_file=`echo $${dir}/results.csv`;              \
       $(VERIFY) INPUT_DIR=$$dir OUTPUT_FILE=$$output_file; \
     done

# Cleaning:
clean:
	rm -f $(BINS_DIR)/*
	rm -f $(ALWAYS_NOTTAKEN_DIR)/*
	rm -f $(BACKWARD_TAKEN_DIR)/*
	rm -f $(DYNAMIC_PRED_DIR)/*
