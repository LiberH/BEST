## **BEST**: a Binary Executable Slicing Tool

### Building BEST:

* Set `BEST_PLUGINS_PATH` and `BEST_PLUGIN`;
* Make the executable.

Example :

```bash
$ export BEST_PLUGINS_PATH=/path/to/best/plugins
$ export BEST_PLUGIN=e200z4
$ make
```

### Testing BEST:

#### Using the benchmark suite for BEST:

* Set `BEST_PLUGINS_PATH` and `BEST_PLUGIN`;
* Set `BEST_BENCHMARKS_PATH` and `BEST_BENCHMARK`;
* Run the benchmark suite.
 
Example :

```bash
$ export BEST_PLUGINS_PATH=/path/to/best/plugins
$ export BEST_PLUGIN=e200z4
$ export BEST_BENCHMARKS_PATH=/path/to/best/benchmarks
$ export BEST_BENCHMARK=malardalen
$ make benchmarks
```

#### Using an arbitrary binary executable file:

BEST needs an architecure-specific UPPAAL template file to produce an output
UPPAAL model. Such template files can be found in the BEST plugins directories.

Example :

```bash
$ ./bin/main --template=/path/to/best/plugins/template.xml arbitrary.elf
```
