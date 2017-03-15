## **BEST**: a Binary Executable Slicing Tool

### Building BEST:

* Set **BEST_PLUGIN** and **BEST_PLUGIN_PATH** ;
* Make the executable.

Example :

> \$ export BEST_PLUGIN=ppc
> \$ export BEST_PLUGIN_PATH=/path/to/best-ppc
> \$ make -j5
> [...]
> \$ ls ./bin
> main

### Testing BEST:

#### Using the benchmark suite for BEST:

* Set **BEST_BENCHMARKS** and **BEST_BENCHMARKS_PATH** ;
* Set **BEST_PLUGIN_PATH** ;
* Run the benchmark suite.
 
Example :

> \$ export BEST_BENCHMARKS=malardalen
> \$ export BEST_BENCHMARKS_PATH=/path/to/best-benchmarks
> \$ export BEST_PLUGIN_PATH=/path/to/best-ppc
> \$ make benchmarks
> [...]

#### Using an arbitrary binary executable file:

Example :

> \$ ./bin/main --template=/path/to/template.xml arbitrary.elf
> [...]
