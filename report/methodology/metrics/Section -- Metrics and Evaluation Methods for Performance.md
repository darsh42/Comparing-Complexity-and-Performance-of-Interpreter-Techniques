# Performance

###
**What is the central question? What are we trying to measure?**

The overall question is:     "Can an interpreter approach JIT like performance
                              without JIT like complexity."

The performance question is: "What is the performance of an Interpreter and 
                              how does it compare to a JIT compiler or native
                              system?"

The measurement question is: "What metrics are required to adequetly measure the 
                              performance of an Interpreter or JIT compiler?"

Timing:
    user timing
    system timing

Resource metrics
    io-ops
    memory usage
    function utilisation (flame graphs)

Hardware counters:
    
    cpu cycles
    ipc
    context switches
    
    intel cpu counters:
    retiring            - % of uOPs completed
        expectations:
        switch-based    - low percentage in branchless code, medium in branchy code
        computed-goto   - medium percentage in branchless code, high in branchy code
        decoupled       - high percentage in branchless, low in branchy code

    bad-specualtion     - % of uOPs 
        expectations:
        
    frontend-bound      - % of uOPS that are restricted by decoding stalls
        expectations:
        switch-based    - high since more difficult to precompute jumps
        computed-goto   - low easiest to pre-compute jumps 
        decoupled       - medium uses heavy data structures and glue code

    backend-bound       - % of uOPs that are restricted by resource stalls or execution speed
        expectations:
        switch-based    - low percentage compared to any JIT compiler
        computed-goto   - low percentage compared to any JIT compiler
        decoupled       - low percentage compared to any JIT compiler

The base metrics like total time spent, memory utilisation and function utilisation will give a good overview of 
the basic behaviours of each of the models. The timing comparisions will make it clear which model is the most 
effective in each of the designed benchmarks. The memory requirement will indicate the system resource utilisation.
The function frequency metrics will make it clear what is the underlying bottleneck for some of the more complex models.

Whilst the above measures are simple and give a nice overview of each of the interpreters, to get a clearer idea of the 
underlying behaviours of each of the models, looking into the hardware counters and comparing the differences between each
of them will better highlight the benefits and drawbacks of each of the models.

Straight forware hardware metrics like cycle count and ipc should reflect the conclusions from the timing and resource metrics.
