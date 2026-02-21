# Benchmarks

## Performance 

The interpreters are going to be tested using 6 different test programs. Each of the programs will apply a different load on the interpreters so that each of the designs can be assessed fairly. The test programs are split into 3 categories, simple ALU stress tests, simple branching tests, and a realistic workload.

The programs designed are the following:
    binary_search.c
    crc.c
    daxpy.c
    linked_list.c
    quicksort.c
    NPB SERIAL Integer Sort (IS)

### AXPY
**what is AXPY?**
AXPY is a routine in the Basic Linear Algebra Suite (BLAS) libraries. 

The routine is a simple addition of two vectors X and Y, only vector X is scaled by a factor of A before addition.
    
    Z = A * X + Y

**why should AXPY be used?**

Simple, easy to verify using the dissassembler.
Targets ALU operations giving clear info about throughput

**what are we expecting in terms of performance from running AXPY?**

Assuming the AXPY test is going to be using tight for loops:

Switch          | lowest performance | muliple jump calculations, no amortization of decoding costs
Computed-Goto   | medium performance | cpu can predict jumps, mulitple jump calculations
Blocked         | higest performance | cpu can predict jumps in decode, amortization of decoding costs, uses hashmaps
Blocked-Chained | higest performance | cpu can predict jumps in decode, amortization of decoding costs, doesn't use hashmaps

The key factor here is the tight loop, since the code is being run over and over again, assuming blocks are split based on jumps and branches the blocked versions will see the highest throughput since they are not repeatedly decoding the instructions.

### Cyclic Redundance Check (CRC)
### Binary Search
**what is Binary Search**
**why should Binary Search be used?**
**what are the expected performance results of Binary Search?**
### Linked List
### Quick Sort

### NAS NASA NPB Serial Integer Sort
To create a realistic workload on the interpreters the NAS NASA benchmarking suite will be used, the benchmarks are intended for testing parallel systems but there are also serial versions of each of the benchmarks. Using the benchmarks, the designs of each of the interpreters can be evaluated on a realistic workload.

## Complexity

### SLOC
**what is SLOC**
**why should SLOC be used?**
**what are the expected results of SLOC analysis?**
### Cyclometric Complexity
**what is Cyclometric Complexity**
**why should Cyclometric Complexity be used?**
**what are the expected results of Cyclometric Complexity analysis?**
### Heuristic Evaluation (Possibly)
**what is Heuristic Evaluation**
**why should Heuristic Evaluation be used?**
**what are the expected results of Heuristic Evaluation analysis?**
