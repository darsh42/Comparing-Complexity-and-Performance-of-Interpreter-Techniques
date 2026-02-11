### Binary Translators

Computer systems consist of CPU, Memory, Hardware accelerators and hardware peripherals.
CPUs are catagorised into families, these families are related due to the binary instruction set they share.
When compiling a program from source to machine code the compiler will create an executable targeting a CPU family, a memory architecture specification in the form of a linker script, allowing use of hardware accelerators that are expected and allowing use of hardware peripherals.
When you get this executable the code will only run a system with the similair specification to the target.
But what if you want to run it on a different system?
There are a number of different techniques for running foreign executables on a system.
