TARGET = $(MAKE) --no-print-directory -C $(BEST_BENCHMARKS_PATH) malardalen-verify

OPTS  = none default
PREDS = none static dynamic
USAGE = "Usage:	\n" \
	"  make OPT={none,default} PRED={none,static,dynamic}\n" \
        "  make forall"

# Experimental data:
LOG_FILE = results.csv
all:
	@case "$(OPT)"  in "none") opti_desc="not optimized"       ;; \
		        "default") opti_desc="optimized"           ;; \
				*) echo $(USAGE); exit 1           ; \
	 esac                                                      ; \
	 case "$(PRED)" in "none") pred_desc="no"                 ;; \
	                 "static") pred_desc="static"             ;; \
	                "dynamic") pred_desc="static and dynamic" ;; \
				*) echo $(USAGE); exit 1           ; \
	 esac                                                      ; \
	 echo "Producing experimental data for models with:"       ; \
	 echo " * $$opti_desc BTB model;"                          ; \
	 echo " * $$pred_desc branch prediction."                  ; \
	 echo ""

	$(eval BIN_PATH := `pwd`/opt-$(OPT)/pred-$(PRED))
	@BIN_PATH=$(BIN_PATH) $(TARGET) | tee $(BIN_PATH)/$(LOG_FILE)
	@echo ""

forall:
	@for opt in $(OPTS)                                        ; \
	 do for pred in $(PREDS)                                   ; \
	    do OPT=$$opt PRED=$$pred $(MAKE) --no-print-directory  ; \
	    done                                                   ; \
	 done

