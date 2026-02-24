# dissertation
This project is an implementation of different interpreter techniques for the MIPS R3000 processor.

## Pre-requisites
Download the required build tools
```bash
apt install cmake make
```

## Build
Clone source 
```bash
https://github.com/darsh42/dissertation
cd dissertation
```

compile interpreters
```bash
cmake -S . -B build
make -C build
```

compile examples
```bash
cd samples
cmake -S . -B ../build-samples
cd ..
make -C build-samples
```


## Usage
Currently there are three binaries compiled with the default build command.

### interpreter
```
"switch" is the switch based interpreter
"computed-goto" is the computed goto based interpreter
"blocked" is the block based interpreter
"blocked_chaining" is the block based chaining interpreter
```
```bash
make -C build
```

### testsuite
"testsuite", is a collection of tests that are used for development.
```bash
./build/testsuite
```

### TODO

    PLAN:
        [*] SYSCALLS        - enables complex sample programs for performance testing
        [*] SDK             - enables the creation of programs for performance testing
        [*] LOADER          - allows to load all programs created
        [-] SAMPLE PROGRAMS - samples to test the interpreters and jitcompilers
        [-] BENCHMARKING    - allows for comparing interpreter versions while developing
        [*] SWITCH          - create a interpreter as a base line
        [*] COMPUTED-GOTO   - create a interpreter as a base line
        [*] BLOCK           - create a interpreter as a base line
        [*] BLOCK CHANING   - create a interpreter as a base line
        [ ] TOOLS           - visualise the results
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
