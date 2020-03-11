#!/bin/bash

# BEST
BEST_PATH="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
export BEST_BENCHMARKS_PATH=${BEST_PATH}/benchmarks
export BEST_CORE_PATH=${BEST_PATH}/core
export BEST_MODELS_PATH=${BEST_PATH}/models
export BEST_PLUGINS_PATH=${BEST_PATH}/plugins

export BEST_PLUGIN=e200z4

# UPPAAL
export UPPAAL_PATH=
