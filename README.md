# dissertation
This project is an implementation of different interpreter techniques for the MIPS R3000 processor.

## Pre-requisites
Download the required build tools
```bash
sudo apt update && sudo apt install -y \
    build-essential \
    cmake \
    git \
    wget \
    xz-utils \
    libgmp-dev \
    libmpfr-dev \
    libmpc-dev \
    bison \
    flex \
    texinfo \
    gawk \
    python3
```

## Build 
### Interpreters + Toolchain
Clone source 
```bash
git clone https://github.com/darsh42/dissertation
cd dissertation
```
compile interpreters, sample programs and toolchain
```bash
cmake -S . -B build
make -C build
```

### Samples
build samples for interpreters
```bash
make -C build/samples
```

### PIN-TOOL
compile pin-tool for efficiency calculations
```
Install Intel Pin Tool from here (Tool was tested on version 4.2):
https://www.intel.com/content/www/us/en/developer/articles/tool/pin-a-binary-instrumentation-tool-downloads.html
unzip pin-tool
```

Set pin-tool root environment variable
```bash
export PIN_ROOT=path/to/pin-tool
```

compile the pin-tool
```bash
cd dissertation/pintool
make obj-intel64/InterpreterEfficiency.so
```

## Binaries
interpreter binaries:
These are used for performance and pipeline utilisation tests
```
switch
tail-call
computed-goto
blocked
```

profiling binaries:
These are used for the efficiency testing (by injecting pin tool stubs)
```
profile_switch
profile_tail-call
profile_computed-goto
profile_blocked
```

toolchain:
the tools used to analyse the sample binaries can be found here
```
dissertation/build/tools/
```

## Benchmarking
Within the root directory there is a measurements directory, here is where all the benchmarking scripts reside.

#### Environment
Setup the environment
```bash
cd dissertation/measurement
python -m venv venv
source ./venv/bin/activate
pip install -r requirements.txt
```

#### Performance
execute performance tests on all benchmarks, sudo is required for cpu govenor and ASLR
```bash
(venv) cd dissertation/measurement
(venv) sudo ./venv/bin/python performance.py
```
if you want to limit the memory available (can take a lot of memory)
```bash
(venv) cd dissertation/measurement
(venv) limit_memory <MemoryMax> sudo ./venv/bin/python performance.py
```

#### Efficiency
execute efficiency tests on all benchmarks
```bash
(venv) cd dissertation/measurement
(venv) python efficiency.py
```
if you want to limit the memory available (can take a lot of memory)
```bash
(venv) cd dissertation/measurement
(venv) limit_memory <MemoryMax> python efficiency.py
```

#### Complexity
calculate complexity of interpreters
```bash
(venv) cd dissertation/measurement
(venv) python complexity.py
```

## Misc
macro expanded sources:
These are used to verify the validitiy of the X-Macro templates
```
expanded_switch
expanded_tail-call
expanded_computed-goto
expanded_blocked
```

### TODO

    PLAN:
        [*] SYSCALLS        - enables complex sample programs for performance testing
        [*] SDK             - enables the creation of programs for performance testing
        [*] LOADER          - allows to load all programs created
        [*] SAMPLE PROGRAMS - samples to test the interpreters and jitcompilers
        [*] BENCHMARKING    - allows for comparing interpreter versions while developing
        [*] SWITCH          - create a interpreter as a base line
        [*] COMPUTED-GOTO   - create a interpreter as a base line
        [*] BLOCK           - create a interpreter as a base line
        [*] BLOCK CHANING   - create a interpreter as a base line
        [*] TOOLS           - visualise the results
        [*] TESTS           - ensure validity of all instructions

    SYSCALLS     - allow interpreter to create and access files
    SDK          - development libraries for program creation
    INTERPRETERS - create all interpreters
    BENCHMARKING - create benchmarking tools to measure different 
                   performance metrics. Time, ops, stalls etc.
    TOOLS        - create tools to transform data from benchmarking to 
                   usable graphs and diagrams
    LOADER       - implement the remaining parts of the loader, loading
                   initial register values, muliple sections etc.
    TESTS        - create more indepth tests. Aiming to catch  more obscure
                   mips behaviour
